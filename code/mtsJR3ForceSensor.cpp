/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: mtsJR3ForceSensor.cpp 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2013-07-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmPositionJointGet.h>
#include <sawJR3ForceSensor/mtsJR3ForceSensor.h>

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG( mtsJR3ForceSensor, 
                                       mtsComponent, 
                                       std::string )

mtsJR3ForceSensor::mtsJR3ForceSensor( const std::string& name ) : 
mtsComponent( name )
{}

mtsJR3ForceSensor::mtsJR3ForceSensor( const std::string& name,
                                      const std::string& robfile,
                                      const vctFrame4x4<double>& Rtw0,
                                      const std::string& devicename,
                                      osaJR3ForceSensor::Units u,
                                      const osaJR3ForceSensor::Wrench& bias,
                                      const vctFrame4x4<double>& Rtst,
                                      double mass,
                                      const vctFixedSizeVector<double,3>& com ):
    mtsComponent( name ),
    jr3( devicename, u, bias, Rtst, mass, com ),
    robot( robfile, Rtw0 ){

    jr3.Open();

    mtsInterfaceProvided* output = NULL;
    output = AddInterfaceProvided( "Output" );
    if( output != NULL ){ 
        output->AddCommandRead( &mtsJR3ForceSensor::Read, 
                                this,
                                "GetCartesianForceTorque" );
        output->AddCommandRead( &mtsJR3ForceSensor::ReadTransformed, 
                                this,
                                "GetCartesianForceTorqueTransformed" );
        output->AddCommandRead( &mtsJR3ForceSensor::ReadToolComp, 
                                this,
                                "GetCartesianForceTorqueToolCompensation" );
    }
    else{
        CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Output."
                                << std::endl;
    }
    
    mtsInterfaceRequired* input = NULL;
    input = AddInterfaceRequired( "Input", MTS_OPTIONAL );
    if( input != NULL ){
        input->AddFunction( "GetPositionJoint", GetRobotJoint );
    }
    else{
        CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Input"
                                << std::endl;
    }
    
    mtsInterfaceProvided* control = NULL;
    control = AddInterfaceProvided( "Control" );
    if( control != NULL ){
        control->AddCommandVoid( &mtsJR3ForceSensor::Zero, this, "Zero" );
    }
    else{
        CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Control."
                                << std::endl;
    }
    
}

mtsJR3ForceSensor::~mtsJR3ForceSensor()
{ jr3.Close(); }

void mtsJR3ForceSensor::Zero(){

    vctFrame4x4<double> Rtws;

    prmPositionJointGet prmq;
    GetRobotJoint( prmq );
        
    bool valid = false;
    prmq.GetValid( valid );
    if( valid ){

        vctDynamicVector<double> vctq;
        prmq.GetPosition( vctq );
        Rtws = robot.ForwardKinematics( vctq );
        
    }

    jr3.Zero( Rtws );
    
}

void mtsJR3ForceSensor::Read( prmForceCartesianGet& prmft ) const {
    
    osaJR3ForceSensor::Wrench w;
    if( jr3.Read( w, false, 2 ) != osaJR3ForceSensor::ESUCCESS ){
        bool valid = false;
        prmft.SetValid( valid );
    }
    else{
        bool valid = true;
        prmft.SetValid( valid );
        prmft.SetForce( w );
    }

}

void mtsJR3ForceSensor::ReadTransformed( prmForceCartesianGet& prmft ) const {

    osaJR3ForceSensor::Wrench w;
    if( jr3.Read( w, true, 2 ) != osaJR3ForceSensor::ESUCCESS ){
        bool valid = false;
        prmft.SetValid( valid );
    }
    else{
        bool valid = true;
        prmft.SetValid( valid );
        prmft.SetForce( w );
    }
    
}

void mtsJR3ForceSensor::ReadToolComp( prmForceCartesianGet& prmft ) const {
    
    //prmPositionCartesianGet prmRt;
    //GetToolPosition( prmRt );

    prmPositionJointGet prmq;
    GetRobotJoint( prmq );


    bool valid = false;
    prmq.GetValid( valid );

    if( valid ){

        vctDynamicVector<double> vctq;
        prmq.GetPosition( vctq );

        vctFrame4x4<double> Rt = robot.ForwardKinematics( vctq );
        vctQuaternionRotation3<double> q( Rt.Rotation(), VCT_NORMALIZE );
        vctMatrixRotation3<double> R( q, VCT_NORMALIZE );

        osaJR3ForceSensor::Wrench w;
        if( jr3.Read( w, R, true, 2 ) != osaJR3ForceSensor::ESUCCESS ){
            bool valid = false;
            prmft.SetValid( valid );
        }
        else{
            bool valid = true;
            prmft.SetValid( valid );
            prmft.SetForce( w );
        }

    }

}

