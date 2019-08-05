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

#ifndef SRC_DISEASEMODEL_CFACTOROPERATION_H_
#define SRC_DISEASEMODEL_CFACTOROPERATION_H_

struct json_t;

class CFactorOperation
{
public:
  enum struct Type {
    assign,
    multiply,
    divide,
    __NONE
  };

  CFactorOperation();

  CFactorOperation(const CFactorOperation & src);

  virtual ~CFactorOperation();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

  void apply(double & value) const;

private:
  Type mType;
  double mValue;
  bool mValid;
};

#endif /* SRC_DISEASEMODEL_CFACTOROPERATION_H_ */