/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: osaJR3ForceSensor.cpp 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2012-02-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctQuaternionRotation3.h>
#include <sawJR3ForceSensor/osaJR3ForceSensor.h>

#include <errno.h>
#include <string.h>
#include <iomanip>
#include <math.h>

#if (CISST_OS == CISST_LINUX)
#include <comedi.h>
#include <comedilib.h>
#endif

// OS dependent structure
struct osaJR3ForceSensor::Internals{
#if (CISST_OS == CISST_LINUX)
    std::string devicefile;
    comedi_t* Device;
    int Subdevice;
    int NumChannels;
    comedi_range** Ranges;
#endif
};

osaJR3ForceSensor::osaJR3ForceSensor( const std::string& name,
                                      Units u,
                                      const osaJR3ForceSensor::Wrench& bias,
                                      const vctFrame4x4<double>& Rtst,
                                      double mass,
                                      const vctFixedSizeVector<double,3>& com ):
    internals(NULL),
    units( u ),
    bias( bias ),
    Rtst( Rtst ),
    mass( mass ),
    com( com ){

    try{ internals = new osaJR3ForceSensor::Internals; }
    catch( std::bad_alloc& )
        { CMN_LOG_RUN_ERROR << "Failed to allocate internals" << std::endl; }

#if (CISST_OS == CISST_LINUX)
    internals->devicefile = name;
#endif

}

osaJR3ForceSensor::Errno osaJR3ForceSensor::Open(){

#if (CISST_OS == CISST_LINUX)

    if( internals == NULL ){
        CMN_LOG_RUN_ERROR<<"osaJR3ForceSensor::Open: internals not allocated\n";
        return osaJR3ForceSensor::EFAILURE;
    }
    
    internals->Device = comedi_open( internals->devicefile.c_str() );
    internals->Subdevice = 0;
    internals->NumChannels = 0;
    internals->Ranges = NULL;
    
    if( internals->Device == NULL ){
        std::stringstream ss;
        ss << "osaJR3ForceSensor::Open: Failed to open device: " 
           << internals->devicefile << std::endl;
        CMN_LOG_INIT_ERROR << ss;
        cmnThrow(std::string(ss.str()));
    }

    CMN_LOG_INIT_VERBOSE<<"osaJR3ForceSensor::osaJR3ForceSensor: Device opened."
                        << std::endl;

    internals->NumChannels = comedi_get_n_channels( internals->Device, 
                                                    internals->Subdevice );
    internals->Ranges = new comedi_range*[internals->NumChannels];

    for( int i=0; i<internals->NumChannels; i++ ){
        internals->Ranges[i] = comedi_get_range( internals->Device, 
                                                 internals->Subdevice, i, 0 );
        if( internals->Ranges[i] == NULL ) {
            std::stringstream ss;
            ss << "osaJR3ForceSensor::Open: Failed to get range for channel " 
               << i << std::endl;
            CMN_LOG_INIT_ERROR << ss;
            cmnThrow( std::string(ss.str()) );
        }
        else{
            CMN_LOG_INIT_VERBOSE << "Channel: " << i << " "
                                 << internals->Ranges[i]->min << " " 
                                 << internals->Ranges[i]->max << " " 
                                 << internals->Ranges[i]->unit << std::endl;
        }
    }
    
    int flags = comedi_get_subdevice_flags( internals->Device, 
                                            internals->Subdevice );
    if( flags != -1 ){
        if( flags&SDF_BUSY )
            { CMN_LOG_INIT_VERBOSE << "SDF_BUSY "; }
        if( flags&SDF_BUSY_OWNER )
            { CMN_LOG_INIT_VERBOSE << "SDF_BUSY_OWNER "; }
        if( flags&SDF_LOCKED )
            { CMN_LOG_INIT_VERBOSE << "SDF_LOCKED "; }
        if( flags&SDF_LOCK_OWNER )
            { CMN_LOG_INIT_VERBOSE << "SDF_LOCK_OWNER "; }
        if( flags&SDF_MAXDATA )
            { CMN_LOG_INIT_VERBOSE << "SDF_MAXDATA "; }
        if( flags&SDF_FLAGS )
            { CMN_LOG_INIT_VERBOSE << "SDF_FLAGS "; }
        if( flags&SDF_RANGETYPE )
            { CMN_LOG_INIT_VERBOSE << "SDF_RANGETYPE "; }
        if( flags&SDF_CMD )
            { CMN_LOG_INIT_VERBOSE << "SDF_CMD "; }
        if( flags&SDF_SOFT_CALIBRATED )
            { CMN_LOG_INIT_VERBOSE << "SDF_SOFT_CALIBRATED "; }
        if( flags&SDF_READABLE )
            { CMN_LOG_INIT_VERBOSE << "SDF_READABLE "; }
        if( flags&SDF_WRITABLE )
            { CMN_LOG_INIT_VERBOSE << "SDF_WRITABLE "; }
        if( flags&SDF_INTERNAL )
            { CMN_LOG_INIT_VERBOSE << "SDF_INTERNAL "; }
        if( flags&SDF_GROUND )
            { CMN_LOG_INIT_VERBOSE << "SDF_GROUND "; }
        if( flags&SDF_COMMON )
         { CMN_LOG_INIT_VERBOSE << "SDF_COMMON "; }
        if( flags&SDF_DIFF )
           { CMN_LOG_INIT_VERBOSE << "SDF_DIFF "; }
        if( flags&SDF_OTHER )
            { CMN_LOG_INIT_VERBOSE << "SDF_OTHER "; }
        if( flags&SDF_DITHER )
            { CMN_LOG_INIT_VERBOSE << "SDF_DITHER "; }
        if( flags&SDF_DEGLITCH )
            { CMN_LOG_INIT_VERBOSE << "SDF_DEGLITCH "; }
        if( flags&SDF_RUNNING )
            { CMN_LOG_INIT_VERBOSE << "SDF_RUNNING "; }
        if( flags&SDF_LSAMPL )
            { CMN_LOG_INIT_VERBOSE << "SDF_LSAMPL "; }
        if( flags&SDF_PACKED )
            { CMN_LOG_INIT_VERBOSE << "SDF_PACKED "; }
        CMN_LOG_INIT_VERBOSE << std::endl;
    }

    switch( comedi_get_subdevice_type( internals->Device, 
                                       internals->Subdevice ) ){
    case COMEDI_SUBD_UNUSED:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_UNUSED\n"; }   break;
    case COMEDI_SUBD_AI:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_AI\n"; }       break;
    case COMEDI_SUBD_AO:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_AO\n"; }       break;
    case COMEDI_SUBD_DI:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_DI\n"; }       break;
    case COMEDI_SUBD_DO:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_DO\n"; }       break;
    case COMEDI_SUBD_DIO:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_DIO\n"; }      break;
    case COMEDI_SUBD_COUNTER:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_COUNTER\n"; }  break;
    case COMEDI_SUBD_TIMER:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_TIMER\n"; }    break;
    case COMEDI_SUBD_MEMORY:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_MEMORY\n"; }   break;
    case COMEDI_SUBD_CALIB:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_CALIB\n"; }    break;
    case COMEDI_SUBD_PROC:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_PROC\n"; }     break;
    case COMEDI_SUBD_SERIAL:
        { CMN_LOG_INIT_VERBOSE << "COMEDI_SUBD_SERIAL\n"; }   break;
    default:
        break;
    }

#endif

    return osaJR3ForceSensor::ESUCCESS;

}

osaJR3ForceSensor::Errno osaJR3ForceSensor::Close(){

#if (CISST_OS == CISST_LINUX)

    if( internals->Device != NULL ){
        if( comedi_close( internals->Device ) == -1 ){
            CMN_LOG_RUN_ERROR << "osaJR3ForceSensor::Closed: Failed to close."
                              << std::endl;
        }
        CMN_LOG_INIT_VERBOSE << "osaJR3ForceSensor::Close: Device closed." 
                             << std::endl;
    }

    if( internals->Ranges != NULL ){ delete[] internals->Ranges; }

#endif

    return osaJR3ForceSensor::ESUCCESS;

}

osaJR3ForceSensor::~osaJR3ForceSensor()
{ if( internals != NULL ) { delete internals; } }


void osaJR3ForceSensor::Zero( const vctFrame4x4<double>& Rtws ){

    vctQuaternionRotation3<double> qws( Rtws.Rotation(), VCT_NORMALIZE );
    vctMatrixRotation3<double> Rws( qws, VCT_NORMALIZE );

    osaJR3ForceSensor::Wrench w( 0.0 );
    SetBias( w ); // clear bias
    
    if( Read( w, false, 5 ) == osaJR3ForceSensor::ESUCCESS ){
        osaJR3ForceSensor::Wrench g = CompensateToolMass( Rws );
        SetBias( w - g ); 
    }

}

osaJR3ForceSensor::Errno 
osaJR3ForceSensor::Read
( osaJR3ForceSensor::Wrench& ft,
  const vctMatrixRotation3<double>& R,
  bool transform,
  int filterId ) const {
    
    osaJR3ForceSensor::Errno err = Read( ft, false, filterId);
    if( transform ){
        ft = Adjoint( Rtst ) * ( ft - CompensateToolMass( R ) );
        return err;
    }

    ft = ft - CompensateToolMass( R ); 
    return err;

}

osaJR3ForceSensor::Errno 
osaJR3ForceSensor::Read
( osaJR3ForceSensor::Wrench& ft, 
  bool transform,
  int filterId ) const {
    
    ft.SetAll(0.0);

    if( filterId<0 || 6<filterId ){ return osaJR3ForceSensor::EFAILURE; }
    filterId = 1;
#if (CISST_OS == CISST_LINUX)
    
    for( size_t i=0; i<6; i++ ){
        lsampl_t sample = 0;
        int cid = filterId * 8 + i;
        int err;
        err= comedi_data_read( internals->Device, 
                               internals->Subdevice, 
                               cid, 
                               0, 
                               AREF_GROUND, 
                               &sample );
        if( err < 0 ){
            const char* string = comedi_strerror( comedi_errno() );
            CMN_LOG_RUN_ERROR << "osaJR3ForceSensor::Read: " << string
                              << std::endl;
            return osaJR3ForceSensor::EFAILURE;
        } 
        else {
            double val = comedi_to_phys(sample, internals->Ranges[i], 32767);
            if( i <= 0 && i<=2 ) { ft[i] *= 4.448221628254617; }
            if (NAN == val){
                const char* string = comedi_strerror( comedi_errno() );
                CMN_LOG_RUN_ERROR << "osaJR3ForceSensor::Read: " << string 
                                  << std::endl;
            return osaJR3ForceSensor::EFAILURE;
            }
            else
                { ft[i] = val; }
        }

    }

#endif

    // rotate about X by pi and leave z intact since JR3ForceSensor is left handed
    ft[1] = -ft[1];
    ft[4] = -ft[4];
    
    if( transform ){
        ft = Adjoint( Rtst ) * ( US2SI( ft ) - bias );
        return osaJR3ForceSensor::ESUCCESS;
    }

    ft = US2SI( ft ) - bias;
    
    return osaJR3ForceSensor::ESUCCESS;

}

osaJR3ForceSensor::Wrench 
osaJR3ForceSensor::CompensateToolMass
( const vctMatrixRotation3<double>& R ) const {

    Wrench w( 0.0, 0.0, -9.81*mass, 0.0, 0.0, 0.0 );
    return Adjoint( vctFrame4x4<double>( R, -R*com ) ) * w;

}

osaJR3ForceSensor::Wrench 
osaJR3ForceSensor::US2SI
( const osaJR3ForceSensor::Wrench& wus ) const {

    osaJR3ForceSensor::Wrench wsi( 0.0 );

    wsi[0] = wus[0] * 4.44822162;
    wsi[1] = wus[1] * 4.44822162;
    wsi[2] = wus[2] * 4.44822162;

    wsi[3] = wus[3] * 0.112984829;
    wsi[4] = wus[4] * 0.112984829;
    wsi[5] = wus[5] * 0.112984829;

    return wsi;

}

osaJR3ForceSensor::vctAdjoint 
osaJR3ForceSensor::Adjoint
( const vctFrame4x4<double>& Rt )
{
    /*
      // Craig 3rd Edition Eq. 5.105
      [f1 t1]' = [R12 0; skew(P12)*R12 R12] * [f2 t2]'

      [f1 t1]: force torque w.r.t. frame 1
      [f2 t2]: force torque w.r.t. frame 2
      T12 = [R12 t12]: frame 2 w.r.t. frame 1

      Force Sensor:
        T12 = Rt, frame 1: JR3 frame, frame 2: tip frame,
        so it's an inverse problem:
        
        [f2 t2]' = [R12' 0; -R12'*skew(P12) R12'] * [f1 t1]'
     */
    
    vctFixedSizeMatrix<double,3,3> RT( 0.0 );
    
    // transpose of rotation
    for( size_t r=0; r<3; r++ ){
        for( size_t c=0; c<3; c++ ){
            RT[c][r] = Rt[r][c];
        }
    }

    vctFixedSizeVector<double,3> t = Rt.Translation();
    vctFixedSizeMatrix<double,3,3> r( 0.0 );

    r[0][0] =   0.0;   r[0][1] = -t[2];   r[0][2] =  t[1];
    r[1][0] =  t[2];   r[1][1] =   0.0;   r[1][2] = -t[0];
    r[2][0] = -t[1];   r[2][1] =  t[0];   r[2][2] =   0.0;
    
    vctFixedSizeMatrix<double,3,3> RTr = RT * r;
    
    vctAdjoint Ad( 0.0 );

    // upper left 3x3 block
    Ad[0][0] = RT[0][0];  Ad[0][1] = RT[0][1];  Ad[0][2] = RT[0][2]; 
    Ad[1][0] = RT[1][0];  Ad[1][1] = RT[1][1];  Ad[1][2] = RT[1][2]; 
    Ad[2][0] = RT[2][0];  Ad[2][1] = RT[2][1];  Ad[2][2] = RT[2][2]; 

    // lower left 3x3 block
    Ad[3][0] = -RTr[0][0];  Ad[3][1] = -RTr[0][1];  Ad[3][2] = -RTr[0][2]; 
    Ad[4][0] = -RTr[1][0];  Ad[4][1] = -RTr[1][1];  Ad[4][2] = -RTr[1][2]; 
    Ad[5][0] = -RTr[2][0];  Ad[5][1] = -RTr[2][1];  Ad[5][2] = -RTr[2][2]; 

    // lower right 3x3 block
    Ad[3][3] = RT[0][0];  Ad[3][4] = RT[0][1];  Ad[3][5] = RT[0][2]; 
    Ad[4][3] = RT[1][0];  Ad[4][4] = RT[1][1];  Ad[4][5] = RT[1][2]; 
    Ad[5][3] = RT[2][0];  Ad[5][4] = RT[2][1];  Ad[5][5] = RT[2][2]; 

    return Ad;

}
