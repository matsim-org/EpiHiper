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

#ifndef SRC_ACTIONS_CACTIONENSEMBLE_H_
#define SRC_ACTIONS_CACTIONENSEMBLE_H_

#include <vector>

#include <actions/CActionDefinition.h>
struct json_t;

class CActionEnsemble
{
public:
  CActionEnsemble();

  CActionEnsemble(const CActionEnsemble & src);

  CActionEnsemble(const json_t * json);

  virtual ~CActionEnsemble();

  void fromJSON(const json_t * json);

  const bool & isValid() const;

private:
  std::vector< CActionDefinition > mOnce;
  std::vector< CActionDefinition > mForEach;
  bool mValid;
};

#endif /* SRC_ACTIONS_CACTIONENSEMBLE_H_ */
