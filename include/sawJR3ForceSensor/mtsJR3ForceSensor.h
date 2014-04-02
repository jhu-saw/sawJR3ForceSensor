/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-  */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: mtsJR3ForceSensor.h 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2013-07-22

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

#ifndef _mtsJR3ForceSensor_h
#define _mtsJR3ForceSensor_h

#include <cisstRobot/robManipulator.h>

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFixedSizeVector.h>

#include <cisstParameterTypes/prmForceCartesianGet.h>

#include <sawJR3ForceSensor/osaJR3ForceSensor.h>
#include <sawJR3ForceSensor/sawJR3ForceSensorExport.h>

class CISST_EXPORT mtsJR3ForceSensor : public mtsComponent{

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);
    
 private:
    
    osaJR3ForceSensor jr3;

    mtsFunctionRead GetRobotJoint;
    robManipulator robot;

 protected:

    //! Set bias (zero reading)
    void Zero();

    //! Read ft readings
    void Read( prmForceCartesianGet& prmft ) const ;
    //! Read ft transformed to tip frame
    void ReadTransformed( prmForceCartesianGet& prmft ) const ;
    //! Read ft with tool compensation and transformed to tip frame
    void ReadToolComp( prmForceCartesianGet& prmft ) const ;
    
 public:

    //
    mtsJR3ForceSensor( const std::string& name );

    /*! Constructors and destructor */
    mtsJR3ForceSensor( const std::string& name,
                       const std::string& robfile,
                       const vctFrame4x4<double>& Rtw0,
                       const std::string& devicename,
                       osaJR3ForceSensor::Units u = osaJR3ForceSensor::METRIC,
                       const osaJR3ForceSensor::Wrench& bias = osaJR3ForceSensor::Wrench(0.0),
                       const vctFrame4x4<double>& Rtst = vctFrame4x4<double>(),
                       double mass = 0.0,
                       const vctFixedSizeVector<double,3>& com = vctFixedSizeVector<double,3>(0.0) );
    
    ~mtsJR3ForceSensor();
    
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsJR3ForceSensor);

#endif  //_mtsJR3ForceSensor_h
