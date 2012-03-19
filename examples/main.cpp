/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: main.cpp 3181 2011-11-15 15:41:28Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-22

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

#include <sawJR3ForceSensor/mtsJR3ForceSensor.h>
#include <comedilib.h>

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#include <native/task.h>
#endif

class ProbeJR3 : public mtsTaskPeriodic
{
    mtsFunctionRead ReadRawFT;
    mtsFunctionQualifiedRead ReadFilteredFT;

public:
    ProbeJR3() : mtsTaskPeriodic("Probe", 100 * cmn_ms, true) {
        mtsInterfaceRequired* required = NULL;
        required = AddInterfaceRequired(mtsJR3ForceSensor::JR3InterfaceName);

        if (required) {
            required->AddFunction(mtsJR3ForceSensor::JR3CommandNames::ReadRawFT, ReadRawFT);
            required->AddFunction(mtsJR3ForceSensor::JR3CommandNames::ReadFilteredFT, ReadFilteredFT);
        }
    }
    ~ProbeJR3() {}

    void Configure( const std::string & CMN_UNUSED(s)) {}
    void Startup() {}
    void Cleanup() {}
    void Run(){ 
        ProcessQueuedEvents();
        mtsJR3ForceSensor::FTReading w;
        ReadRawFT(w);
        std::cout << "Filter0: " << w << std::endl;
        ReadFilteredFT(1, w); 
        std::cout << "Filter1: " << w << std::endl << std::endl;
    }
};

int main()
{
#if (CISST_OS == CISST_LINUX_XENOMAI)
    mlockall(MCL_CURRENT|MCL_FUTURE);
#endif

    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ALL);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();

#if (CISST_OS == CISST_LINUX_XENOMA) || (CISST_OS == CISST_LINUX)
    mtsJR3ForceSensor * jr3 = new mtsJR3ForceSensor("JR3", "/dev/comedi4", 0);
#elif (CISST_OS == CISST_QNX)
    mtsJR3ForceSensor * jr3 = new mtsJR3ForceSensor("JR3");
#endif

    taskManager->AddComponent(jr3);

    ProbeJR3 probe;
    taskManager->AddComponent(&probe);

    CONNECT_LOCAL(probe.GetName(), mtsJR3ForceSensor::JR3InterfaceName,
                  jr3->GetName(), mtsJR3ForceSensor::JR3InterfaceName);

    taskManager->CreateAll();
    taskManager->StartAll();

    std::cout << "Press any key to terminate" << std::endl;
    cmnGetChar();

    taskManager->KillAll();
    taskManager->Cleanup();

    delete jr3;

    return 0;
}
