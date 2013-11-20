/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: osaJR3ForceSensorExample.cpp 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2013-07-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnGetChar.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <sawJR3ForceSensor/osaJR3ForceSensor.h>

int main( int, char** argv ){

    cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
    cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
    cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

    vctMatrixRotation3<double> Rst( 0.9511,  0.0000,   0.3090,
                                    0.0000,  1.0000,   0.0000,
                                   -0.3090,  0.0000,   0.9511,
                                    VCT_NORMALIZE );
    vctFixedSizeVector<double,3> tst( 0.0, 0.0, 0.148 );
    vctFixedSizeVector<double,3> com( 0.0, 0.0, 0.03 );
    double mass = 0.145;

    osaJR3ForceSensor::Wrench zero( 0.0 );

    osaJR3ForceSensor jr3( argv[1],
                           osaJR3ForceSensor::METRIC,
                           zero,
                           vctFrame4x4<double>( Rst, tst ),
                           mass,
                           com );
    jr3.Open();
    
    jr3.Read( zero, false, 5 );
    zero[2] += 9.81 * mass;
    jr3.SetBias( zero );
    std::cout << "ZERO: " << zero << std::endl;

    std::cout << "Z up" << std::endl;
    cmnGetChar();
    {
        vctMatrixRotation3<double> R;
        osaJR3ForceSensor::Wrench ft;
        jr3.Read( ft, R, false, 5 );
        std::cout << ft << std::endl;
        usleep(10000);
    }

    std::cout << "Y up" << std::endl;
    cmnGetChar();
    {
        vctMatrixRotation3<double> R(  1.0,  0.0,  0.0,
                                       0.0,  0.0,  1.0,
                                       0.0, -1.0,  0.0,
                                       VCT_NORMALIZE );
        osaJR3ForceSensor::Wrench ft;
        jr3.Read( ft, R, false, 5 );
        std::cout << ft << std::endl;
        usleep(10000);
    }

    std::cout << "X up" << std::endl;
    cmnGetChar();
    {
        vctMatrixRotation3<double> R(  0.0,  0.0,  1.0,
                                       0.0,  1.0,  0.0,
                                       -1.0,  0.0,  0.0,
                                       VCT_NORMALIZE );
        osaJR3ForceSensor::Wrench ft;
        jr3.Read( ft, R, false, 5 );
        std::cout << ft << std::endl;
        usleep(10000);
    }

    std::cout << "Play" << std::endl;
    cmnGetChar();
    while(1){
        
        vctMatrixRotation3<double> R(-0.9511,  0.0000,  -0.3090,
                                     0.0000,  1.0000,   0.0000,
                                     0.3090,  0.0000,  -0.9511,
                                     VCT_NORMALIZE );
        osaJR3ForceSensor::Wrench ft;
        jr3.Read( ft, R, false, 5 );
        std::cout << ft << std::endl;
        usleep(10000);
    }

    jr3.Close();

    return 0;

}
