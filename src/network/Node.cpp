// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include "diseaseModel/Model.h"
#include "diseaseModel/State.h"
#include "diseaseModel/Transmission.h"
#include "diseaseModel/Progression.h"
#include "traits/Trait.h"

#include "Node.h"

NodeData::NodeData()
  : id(-1)
  , pHealthState()
  , susceptibilityFactor(1.0)
  , susceptibility(0.0)
  , infectivityFactor(1.0)
  , infectivity(0.0)
  , nodeTrait()
  , Edges(NULL)
  , EdgesSize(0)
{}

// static
void Node::toBinary(std::ostream & os, const NodeData * pNode)
{
  Model::state_t Type = Model::stateToType(pNode->pHealthState);

  os.write(reinterpret_cast<const char *>(&pNode->id), sizeof(size_t));
  os.write(reinterpret_cast<const char *>(&Type), sizeof( Model::state_t));
  os.write(reinterpret_cast<const char *>(&pNode->susceptibilityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->susceptibility), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->infectivityFactor), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->infectivity), sizeof(double));
  os.write(reinterpret_cast<const char *>(&pNode->nodeTrait), sizeof(TraitData::base));
}

// static
void Node::fromBinary(std::istream & is, NodeData * pNode)
{
  Model::state_t Type;

  is.read(reinterpret_cast<char *>(&pNode->id), sizeof(size_t));
  is.read(reinterpret_cast<char *>(&Type), sizeof(Model::state_t));
  is.read(reinterpret_cast<char *>(&pNode->susceptibilityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->susceptibility), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->infectivityFactor), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->infectivity), sizeof(double));
  is.read(reinterpret_cast<char *>(&pNode->nodeTrait), sizeof(TraitData::base));

  pNode->pHealthState = Model::stateFromType(Type);
}

// static
NodeData Node::getDefault()
{
  NodeData Default;

  Default.id = -1;
  Default.pHealthState = &Model::getInitialState();
  Default.susceptibilityFactor = 1.0;
  Default.susceptibility = Default.pHealthState->getSusceptibility();
  Default.infectivityFactor = 1.0;
  Default.infectivity = Default.pHealthState->getInfectivity();
  Default.nodeTrait = Trait::NodeTrait->getDefault();
  Default.Edges = NULL;
  Default.EdgesSize = 0;

  return Default;
}

Node::Node(NodeData * pData)
  : mpData(pData)
{}

Node::Node(const Node & src)
  : mpData(src.mpData)
{}

Node::~Node()
{}

const NodeData * Node::getData() const
{
  return mpData;
}

void Node::toBinary(std::ostream & os) const
{
  toBinary(os, mpData);
}

void Node::fromBinary(std::istream & is)
{
  fromBinary(is, mpData);
}

bool Node::set(const Transmission * pTransmission, const Metadata & metadata)
{
  if (mpData->pHealthState == pTransmission->getExitState()) return false;

  mpData->pHealthState = pTransmission->getExitState();
  pTransmission->updateSusceptibilityFactor(mpData->susceptibilityFactor);
  mpData->susceptibility = mpData->pHealthState->getSusceptibility() * mpData->susceptibilityFactor;
  pTransmission->updateInfectivityFactor(mpData->susceptibilityFactor);
  mpData->infectivity = mpData->pHealthState->getInfectivity() * mpData->infectivity;

  // std::cout << mpData->id << "," << pTransmission->getEntryState() << "," << pTransmission->getExitState() << "," << pTransmission->getContactState() << std::endl;

  Model::stateChanged(mpData);

  return true;
}

bool Node::set(const Progression * pProgression, const Metadata & metadata)
{
  if (mpData->pHealthState == pProgression->getExitState()) return false;

  mpData->pHealthState = pProgression->getExitState();
  pProgression->updateSusceptibilityFactor(mpData->susceptibilityFactor);
  mpData->susceptibility = mpData->pHealthState->getSusceptibility() * mpData->susceptibilityFactor;
  pProgression->updateInfectivityFactor(mpData->susceptibilityFactor);
  mpData->infectivity = mpData->pHealthState->getInfectivity() * mpData->infectivityFactor;

  // std::cout << mpData->id << "," << pProgression->getEntryState() << "," << pProgression->getExitState() << std::endl;

  Model::stateChanged(mpData);

  return true;
}


