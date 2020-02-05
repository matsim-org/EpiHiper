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

#include <cstring>
#include <jansson.h>

#include "math/CEdgeProperty.h"
#include "network/CEdge.h"
#include "traits/CTrait.h"
#include "actions/COperation.h"
#include "utilities/CLogger.h"

CEdgeProperty::CEdgeProperty()
  : CValueInterface(Type::boolean, NULL)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mNodeProperty()
  , mValid(true)
{}

CEdgeProperty::CEdgeProperty(const CEdgeProperty & src)
  : CValueInterface(src)
  , mpPropertyOf(src.mpPropertyOf)
  , mpCreateOperation(src.mpCreateOperation)
  , mNodeProperty(src.mNodeProperty)
  , mValid(src.mValid)
{}

CEdgeProperty::CEdgeProperty(const json_t * json)
  : CValueInterface(Type::boolean, NULL)
  , mpPropertyOf(NULL)
  , mpCreateOperation(NULL)
  , mNodeProperty()
  , mValid(true)
{
  fromJSON(json);
}

CEdgeProperty::~CEdgeProperty()
{}

// virtual
CValueInterface * CEdgeProperty::copy() const
{
  return new CEdgeProperty(*this);
}

void CEdgeProperty::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  json_t * pObject = json_object_get(json, "edge");

  if (!json_is_object(pObject))
    {
      return;
    }

  json_t * pValue = json_object_get(pObject, "property");

  if (json_is_string(pValue))
    {
      const char * Property = json_string_value(pValue);

      if (strcmp(Property, "weight") == 0)
        {
          mType = Type::number;
          mpPropertyOf = &CEdgeProperty::weight;
          mpCreateOperation = &CEdgeProperty::setWeight;
        }
      else if (strcmp(Property, "active") == 0)
        {
          mType = Type::boolean;
          mpPropertyOf = &CEdgeProperty::active;
          mpCreateOperation = &CEdgeProperty::setActive;
        }
      else if (strcmp(Property, "targetActivity") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::targetActivity;
          mpCreateOperation = &CEdgeProperty::setTargetActivity;
        }
      else if (strcmp(Property, "sourceActivity") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::sourceActivity;
          mpCreateOperation = &CEdgeProperty::setSourceActivity;
        }
      else if (strcmp(Property, "edgeTrait") == 0)
        {
          mType = Type::traitData;
          mpPropertyOf = &CEdgeProperty::edgeTrait;
          mpCreateOperation = &CEdgeProperty::setEdgeTrait;
        }
      else
        {
          CLogger::error() << "Edge property: Invalid property '" << Property << "'";
          return;
        }

      mValid = true;
      return;
    }
}

const bool & CEdgeProperty::isValid() const
{
  return mValid;
}

// static
CNode * CEdgeProperty::targetNode(CEdge * pEdge)
{
  return pEdge->pTarget;
}

// static
CNode * CEdgeProperty::sourceNode(CEdge * pEdge)
{
  return pEdge->pSource;
}

CValueInterface & CEdgeProperty::propertyOf(const CEdge * pEdge)
{
  return (this->*mpPropertyOf)(const_cast< CEdge * >(pEdge));
}

COperation * CEdgeProperty::createOperation(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return (this->*mpCreateOperation)(pEdge, value, pOperator);
}

bool CEdgeProperty::isReadOnly() const
{
  return mpPropertyOf == &CEdgeProperty::targetActivity
         || mpPropertyOf == &CEdgeProperty::sourceActivity;
}

CValueInterface & CEdgeProperty::targetActivity(CEdge * pEdge)
{
  mpValue = &pEdge->targetActivity;
  return *this;
}

CValueInterface & CEdgeProperty::sourceActivity(CEdge * pEdge)
{
  mpValue = &pEdge->sourceActivity;
  return *this;
}

CValueInterface & CEdgeProperty::edgeTrait(CEdge * pEdge)
{
  mpValue = &pEdge->edgeTrait;
  return *this;
}

CValueInterface & CEdgeProperty::active(CEdge * pEdge)
{
  mpValue = &pEdge->active;
  return *this;
}

CValueInterface & CEdgeProperty::weight(CEdge * pEdge)
{
  mpValue = &pEdge->weight;
  return *this;
}

COperation * CEdgeProperty::setTargetActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setTargetActivity);
}

COperation * CEdgeProperty::setSourceActivity(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setSourceActivity);
}

COperation * CEdgeProperty::setEdgeTrait(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CEdge, CTraitData::value >(pEdge, value.toTraitValue(), pOperator, &CEdge::setEdgeTrait);
}

COperation * CEdgeProperty::setActive(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CEdge, bool >(pEdge, value.toBoolean(), pOperator, &CEdge::setActive);
}

COperation * CEdgeProperty::setWeight(CEdge * pEdge, const CValueInterface & value, CValueInterface::pOperator pOperator)
{
  return new COperationInstance< CEdge, double >(pEdge, value.toNumber(), pOperator, &CEdge::setWeight);
}
