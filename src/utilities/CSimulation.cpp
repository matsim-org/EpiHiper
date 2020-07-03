// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "utilities/CSimulation.h"
#include "utilities/CLogger.h"
#include "actions/CActionQueue.h"
#include "actions/CChanges.h"
#include "diseaseModel/CModel.h"
#include "intervention/CInitialization.h"
#include "intervention/CIntervention.h"
#include "intervention/CTrigger.h"
#include "math/CDependencyGraph.h"
#include "network/CEdge.h"
#include "network/CNetwork.h"
#include "network/CNode.h"
#include "utilities/CCommunicate.h"
#include "utilities/CRandom.h"
#include "utilities/CSimConfig.h"
#include "utilities/CStatus.h"
#include "variables/CVariableList.h"

// initialize according to config
CSimulation::CSimulation()
  : valid(false)
  , startTick(0)
  , endTick(0)
{
  startTick = CSimConfig::getStartTick();
  endTick = CSimConfig::getEndTick();
}

CSimulation::~CSimulation() 
{}

// check all required components are ready
bool CSimulation::validate() {
  valid = (startTick <= endTick);
  
  return valid;
}

bool CSimulation::run()
{
  bool success = true;

  CLogger::warn() << "CSimulation::run: threads:" << omp_get_num_threads();

#pragma omp parallel default(shared)
  {
  CLogger::warn() << "CSimulation::run: threads:" << omp_get_num_threads();

#pragma omp single 
    {
      CActionQueue::setCurrentTick(startTick - 1);
      CChanges::setCurrentTick(startTick - 1);
      CChanges::initDefaultOutput();
      CModel::InitGlobalStateCountOutput();
      CDependencyGraph::buildGraph();
    }

#pragma omp barrier

    if (!CDependencyGraph::applyUpdateSequence())
      {
#pragma omp atomic
        success &= false;
      }

#pragma omp barrier
    if (!CInitialization::processAll())
      {
#pragma omp atomic
        success &= false;
      }

#pragma omp single
    CCommunicate::memUsage(CActionQueue::getCurrentTick());

    CActionQueue::processCurrentActions();

#pragma omp single
    {
      CActionQueue::incrementTick();
      CChanges::incrementTick();

      CChanges::writeDefaultOutput();
      CModel::UpdateGlobalStateCounts();
      CModel::WriteGlobalStateCounts();
      CNetwork::Context.Master().broadcastChanges();

      CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));
    }

#pragma omp barrier

    while (CActionQueue::getCurrentTick() < endTick && success)
      {
CLogger::warn() << "CSimulation::run: Tick:" << CActionQueue::getCurrentTick();

        CVariableList::INSTANCE.resetAll();

        if (!CDependencyGraph::applyUpdateSequence())
#pragma omp atomic
          success &= false;

        CModel::ProcessTransmissions();

        if (!CTrigger::processAll())
          success &= false;

#pragma omp barrier
        CIntervention::processAll();

#pragma omp single
        CCommunicate::memUsage(CActionQueue::getCurrentTick());

        CActionQueue::processCurrentActions();

#pragma omp single
        {
          CActionQueue::incrementTick();
          CChanges::incrementTick();

          CChanges::writeDefaultOutput();
          CModel::UpdateGlobalStateCounts();
          CModel::WriteGlobalStateCounts();
          CNetwork::Context.Master().broadcastChanges();

          CStatus::update("running", (100.0 * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1));
        }
      }
  }

  return success;
}
