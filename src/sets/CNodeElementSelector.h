// BEGIN: Copyright 
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_SETS_CNODEELEMENTSELECTOR_H_
#define SRC_SETS_CNODEELEMENTSELECTOR_H_

#include <string>

#include "math/CNodeProperty.h"
#include "math/CValueList.h"
#include "sets/CSetContent.h"

class CFieldValue;
class CFieldValueList;
class CObservable;

class CNodeElementSelector: public CSetContent
{
public:
  CNodeElementSelector();

  CNodeElementSelector(const CNodeElementSelector & src);

  CNodeElementSelector(const json_t * json);

  virtual ~CNodeElementSelector();

  virtual CSetContent * copy() const override;

  virtual void fromJSON(const json_t * json) override;

  virtual bool computeProtected() override;

private:
  bool all();
  bool propertySelection();
  bool propertyIn();
  bool propertyNotIn();
  bool withIncomingEdge();
  bool inDBTable();
  bool withDBFieldSelection();
  bool withDBFieldWithin();
  bool withDBFieldNotWithin();

  CNodeProperty mNodeProperty;
  CValue * mpValue;
  CValueList * mpValueList;
  CSetContent * mpSelector;
  std::string mDBTable;
  std::string mDBField;
  CObservable * mpObservable;
  CFieldValue * mpDBFieldValue;
  CFieldValueList * mpDBFieldValueList;
  CValueInterface::pComparison mpComparison;
  std::string mSQLComparison;
  bool mLocalScope;
  bool (CNodeElementSelector::*mpCompute)();
};

#endif /* SRC_SETS_CNODEELEMENTSELECTOR_H_ */
