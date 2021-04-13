// BEGIN: Copyright 
// Copyright (C) 2019 - 2021 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <jansson.h>

#include "db/CFieldValue.h"
#include "utilities/CLogger.h"

CFieldValue::CFieldValue(const size_t & id)
  : CValue(id)
{}

CFieldValue::CFieldValue(const double & number)
  : CValue(number)
{}

CFieldValue::CFieldValue(const int & integer)
  : CValue(integer)
{}

CFieldValue::CFieldValue(const std::string & str)
  : CValue(str)
{}

CFieldValue::CFieldValue(const json_t * json, const Type & hint)
  : CValue(false)
{
  destroyValue();
  mType = hint;
  fromJSON(json);
}

// virtual
CFieldValue::~CFieldValue()
{}

// virtual
void CFieldValue::fromJSON(const json_t * json)
{
  mValid = false; // DONE
  json_t * pValue = json_object_get(json, "value");

  if (pValue == NULL)
    {
      CLogger::error("Field value: Missing 'value'.");
    }

  if (json_is_real(pValue))
    {
      destroyValue();
      mValid = true;

      if (mType == Type::integer)
        mpValue = new int(json_integer_value(pValue));
      else if (mType == Type::number)
        mpValue = new double(json_real_value(pValue));
      else if (mType == Type::id)
        mpValue = new size_t(json_real_value(pValue));
      else
        mValid = false;

      return;
    }

  if (json_is_string(pValue))
    {
      destroyValue();
      mType = Type::string;
      mpValue = new std::string(json_string_value(pValue));
      mValid = true;
      return;
    }

  CLogger::error("Field value: Invalid type for 'value'.");

  return;
}
