// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <jansson.h>

#include "diseaseModel/CTransmission.h"
#include "diseaseModel/CHealthState.h"
#include "utilities/CLogger.h"

CTransmission::CTransmission()
  : CAnnotation()
  , mId()
  , mpEntryState(NULL)
  , mpExitState(NULL)
  , mpContactState(NULL)
  , mTransmissibility(0)
  , mSusceptibilityFactorOperation()
  , mInfectivityFactorOperation()
  , mValid(false)
{}

CTransmission::CTransmission(const CTransmission & src)
  : CAnnotation(src)
  , mId(src.mId)
  , mpEntryState(src.mpEntryState)
  , mpExitState(src.mpExitState)
  , mpContactState(src.mpContactState)
  , mTransmissibility(src.mTransmissibility)
  , mSusceptibilityFactorOperation(src.mSusceptibilityFactorOperation)
  , mInfectivityFactorOperation(src.mInfectivityFactorOperation)
  , mValid(false)
{}

// virtual
CTransmission::~CTransmission()
{}

void CTransmission::fromJSON(const json_t * json, const std::map< std::string, CHealthState * > & states)
{
  mValid = true;

  std::map< std::string, CHealthState * >::const_iterator found;
  std::map< std::string, CHealthState * >::const_iterator notFound = states.end();

  json_t * pValue = json_object_get(json, "id");

  if (json_is_string(pValue))
    {
      mId = json_string_value(pValue);
      mAnnId = mId;
    }

  if (mId.empty())
    {
      CLogger::error("Transmission: Invalid or missing 'id'.");
      return;
    }

  CAnnotation::fromJSON(json);

  pValue = json_object_get(json, "entryState");
  mpEntryState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpEntryState = found->second;
    }

  if (mpEntryState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'entryState'.");
      return;
    }

  pValue = json_object_get(json, "exitState");
  mpExitState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpExitState = found->second;
    }

  if (mpExitState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'exitState'.");
      return;
    }

  pValue = json_object_get(json, "contactState");
  mpContactState = NULL;

  if (json_is_string(pValue)
      && (found = states.find(json_string_value(pValue))) != notFound)
    {
      mpContactState = found->second;
    }

  if (mpContactState == NULL)
    {
      CLogger::error("Transmission: Invalid or missing 'contactState'.");
      return;
    }

  pValue = json_object_get(json, "transmissibility");
  mTransmissibility = -1.0;

  if (json_is_real(pValue))
    {
      mTransmissibility = json_real_value(pValue);
    }

  if (mTransmissibility < 0.0)
    {
      CLogger::error("Transmission: Invalid or missing 'transmissibility'.");
      return;
    }

  pValue = json_object_get(json, "susceptibilityFactorOperation");

  if (json_is_object(pValue))
    {
      mSusceptibilityFactorOperation.fromJSON(pValue);

      if (!mSusceptibilityFactorOperation.isValid())
        {
          CLogger::error("Transmission: Invalid 'susceptibilityFactorOperation'.");
          return;
        }
    }

  pValue = json_object_get(json, "infectivityFactorOperation");

  if (json_is_object(pValue))
    {
      mInfectivityFactorOperation.fromJSON(pValue);

      if (!mInfectivityFactorOperation.isValid())
        {
          CLogger::error("Transmission: Invalid 'infectivityFactorOperation'.");
          return;
        }
    }

  mValid = true;
}

const std::string & CTransmission::getId() const
{
  return mId;
}

const CHealthState * CTransmission::getEntryState() const
{
  return mpEntryState;
}

const CHealthState * CTransmission::getContactState() const
{
  return mpContactState;
}

const CHealthState * CTransmission::getExitState() const
{
  return mpExitState;
}

const double & CTransmission::getTransmissibility() const
{
  return mTransmissibility;
}

const bool & CTransmission::isValid() const
{
  return mValid;
}

void CTransmission::updateSusceptibilityFactor(double & factor) const
{
  mSusceptibilityFactorOperation.apply(factor);
}

void CTransmission::updateInfectivityFactor(double & factor) const
{
  mInfectivityFactorOperation.apply(factor);
}
