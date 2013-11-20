/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: osaJR3ForceSensor.h 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2013-06-21

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief cisst Component for JR3ForceSensor Force & Torque Sensor
*/

#ifndef _osaJR3ForceSensor_h
#define _osaJR3ForceSensor_h

#include <comedilib.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFrame4x4.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <sawJR3ForceSensor/sawJR3ForceSensorExport.h>

class osaJR3ForceSensor{

public:

    enum Errno { ESUCCESS, EFAILURE };

	// Typedef for force and torque readings
    typedef vctFixedSizeVector< double, 6 > Wrench;
    typedef vctFixedSizeMatrix< double, 6, 6 > vctAdjoint;

    enum Units{ METRIC };

private:

    struct Internals;
    osaJR3ForceSensor::Internals* internals;

    Units units;

    Wrench bias;

    vctFrame4x4<double> Rtst;

    double mass;
    vctFixedSizeVector<double,3> com;

public:
    
    //
    osaJR3ForceSensor(){}

    //! Constructors
    osaJR3ForceSensor( const std::string& name,
                       Units u = METRIC,
                       const osaJR3ForceSensor::Wrench& bias = Wrench( 0.0 ),
                       const vctFrame4x4<double>& Rtst = vctFrame4x4<double>(),
                       double mass = 0.0,
                       const vctFixedSizeVector<double,3>& com = vctFixedSizeVector<double,3>(0.0) );
    
    //! Destructor
    ~osaJR3ForceSensor();
    
    //! Open 
    osaJR3ForceSensor::Errno Open();
    
    //! Close 
    osaJR3ForceSensor::Errno Close();
    
    
    //! Read with tool compenstation
    osaJR3ForceSensor::Errno Read( osaJR3ForceSensor::Wrench& wrench,
                                   const vctMatrixRotation3<double>& R,
                                   bool transform = true,
                                   int filterId = 0 ) const;
        
    //! Read without tool compenstation
    osaJR3ForceSensor::Errno Read( osaJR3ForceSensor::Wrench& wrench,
                                   bool transform = true,
                                   int filterId = 0 ) const;
    
    //! Set tool mass
    void SetMass( double m ) { mass = m; }

    //! Set tool center of mass
    void SetCOM( const vctFixedSizeVector<double,3>& xyz ) { com = xyz; }

    //! Set bias
    void SetBias( const Wrench& vw ) { bias = vw; }

    //! Set bias
    void Zero( const vctFrame4x4<double>& Rtws );

    //! Set frame transformation
    void SetTransform( const vctFrame4x4<double>& Rt ){ Rtst = Rt; }

    //! Convert US to SI
    Wrench US2SI( const Wrench& wus ) const;

    //! Compensate for tool mass
    Wrench CompensateToolMass( const vctMatrixRotation3<double>& R ) const;

    //
    static vctAdjoint Adjoint( const vctFrame4x4<double>& Rt );

};

#endif  //_osaJR3ForceSensor_h
