/*-*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-   */
/*ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab:*/

/*
  $Id: mtsJR3ForceSensorWAMExample.cpp 3181 2011-11-15 15:41:28Z sleonard $

  Author(s):  Simon Leonard
  Created on: 2013-07-27

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <sawCANBus/osaSocketCAN.h>
#include <sawBarrett/mtsWAM.h>
#include <sawControllers/mtsGravityCompensation.h>
#include <sawKeyboard/mtsKeyboard.h>
#include <sawJR3ForceSensor/mtsJR3ForceSensor.h>

class JR3ForceSensorClient : public mtsTaskPeriodic {

private:

    mtsFunctionRead GetForceCartesian;
    mtsFunctionVoid Zero;

public:

    JR3ForceSensorClient() : mtsTaskPeriodic( "JR3ForceSensorClient", 
                                              0.002, 
                                              true ){
        
        mtsInterfaceRequired* input = NULL;
        input = AddInterfaceRequired( "Input", MTS_OPTIONAL );
        if( input != NULL )
            { input->AddFunction( "GetCartesianForceTorqueToolCompensation", 
                                  GetForceCartesian ); }
        else{
            CMN_LOG_CLASS_RUN_ERROR << "Failed to create the interface Input"
                                    << std::endl;
        }
        
        mtsInterfaceRequired* control = NULL;
        control = AddInterfaceRequired( "Control", MTS_OPTIONAL );
        if( control != NULL )
            { control->AddFunction( "Zero", Zero ); }
    }
    
    void Configure( const std::string& ){}
    
    void Startup(){         
    }
    
    void Run(){
        
        ProcessQueuedCommands();

        static bool first = true;
        if( first ){
            Zero();
            first = false;
        }

        prmForceCartesianGet prmft;
        GetForceCartesian( prmft );
        
        bool valid = false;
        prmft.GetValid( valid );
        if( valid ){
            vctDouble6 vctft;
            prmft.GetForce( vctft );
            std::cout << vctft << std::endl;
        }
        
    }
    
    void Cleanup(){}
    
};

int main( int argc, char** argv ){
    
    mtsTaskManager* taskManager = mtsTaskManager::GetInstance();
    
    cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
    cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
    cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );
    
    if( argc != 3 ){
        std::cout << "Usage: " << argv[0] << " can[0-1] robfile" << std::endl;
        return -1;
    }
    
    mtsKeyboard kb;
    kb.SetQuitKey( 'q' );
    kb.AddKeyWriteFunction( 'G', "GCEnable", "Enable", true );
    taskManager->AddComponent( &kb );

    osaSocketCAN can( argv[1], osaCANBus::RATE_1000 );
    if( can.Open() != osaCANBus::ESUCCESS ){
        CMN_LOG_RUN_ERROR << argv[0] << "Failed to open " << argv[1] 
                          << std::endl;
        return -1;
    }
    
    mtsWAM WAM( "WAM", &can, osaWAM::WAM_7DOF, OSA_CPU4, 80 );
    WAM.Configure();
    WAM.SetPositions( vctDynamicVector<double>(7, 
                                               0.0, -cmnPI_2, 0.0, cmnPI, 
                                               0.0, 0.0, 0.0 ) );
    taskManager->AddComponent( &WAM );
    
    // Rotate the base
    vctMatrixRotation3<double> Rw0(  0.0,  0.0, -1.0,
                                     0.0,  1.0,  0.0,
                                     1.0,  0.0,  0.0 );
    vctFixedSizeVector<double,3> tw0(0.0);
    vctFrame4x4<double> Rtw0( Rw0, tw0 );
    
    mtsGravityCompensation GC( "GC", 
                               0.002, 
                               argv[2],
                               Rtw0,
                               OSA_CPU2 );
    taskManager->AddComponent( &GC );
    
    vctMatrixRotation3<double> Rst( 0.9511,  0.0000,  -0.3090,
                                    0.0000,  1.0000,   0.0000,
                                    0.3090,  0.0000,   0.9511,
                                    VCT_NORMALIZE );
    vctFixedSizeVector<double,3> tst( 0.0, 0.0, 0.09 );
    vctFixedSizeVector<double,3> com( 0.0, 0.0, 0.03 );
    double mass = 0.145;
    
    osaJR3ForceSensor::Wrench zero( 0.0 );
    mtsJR3ForceSensor jr3( "JR3ForceSensor",
                           argv[2],
                           Rtw0,
                           "/dev/comedi0",
                           osaJR3ForceSensor::METRIC,
                           zero,
                           vctFrame4x4<double>( Rst, tst ),
                           mass,
                           com );
    taskManager->AddComponent( &jr3 );
    
    JR3ForceSensorClient client;
    taskManager->AddComponent( &client );
    
    if( !taskManager->Connect( kb.GetName(), "GCEnable",
                               GC.GetName(), "Control") ){
        std::cout << "Failed to connect: "
                  << kb.GetName() << "::GCEnable to "
                  << GC.GetName() << "::Control" << std::endl;
        return -1;
    }
    
    if( !taskManager->Connect( WAM.GetName(), "Input",
                               GC.GetName(),  "Output" ) ){
        std::cout << "Failed to connect: "
                  << WAM.GetName() << "::Input to "
                  << GC.GetName()  << "::Output" << std::endl;
        return -1;
    }
    
    if( !taskManager->Connect( WAM.GetName(), "Output",
                               GC.GetName(),  "Input" ) ){
        std::cout << "Failed to connect: "
                  << WAM.GetName() << "::Output to "
                  << GC.GetName()  << "::Input" << std::endl;
        return -1;
    }
    
    if( !taskManager->Connect( WAM.GetName(), "Output",
                               jr3.GetName(), "Input" ) ){
        std::cout << "Failed to connect: "
                  << WAM.GetName() << "::Output to "
                  << jr3.GetName() << "::Input" << std::endl;
        return -1;
    }
    
    if( !taskManager->Connect( jr3.GetName(),    "Output",
                               client.GetName(), "Input" ) ){
        std::cout << "Failed to connect: "
                  << jr3.GetName() << "::Output to "
                  << client.GetName() << "::Input" << std::endl;
        return -1;
    }

    if( !taskManager->Connect( jr3.GetName(),    "Control",
                               client.GetName(), "Control" ) ){
        std::cout << "Failed to connect: "
                  << jr3.GetName() << "::Output to "
                  << client.GetName() << "::Input" << std::endl;
        return -1;
    }

    taskManager->CreateAll();
    taskManager->StartAll();
    
    std::cout << "Press 'G' to enable/disable gravity compensation."
              << std::endl;
    pause();

  return 0;
}
