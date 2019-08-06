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

/*
 * CNodeProperty.h
 *
 *  Created on: Aug 6, 2019
 *      Author: shoops
 */

#ifndef SRC_NETWORK_CNODEPROPERTY_H_
#define SRC_NETWORK_CNODEPROPERTY_H_

struct json_t;

class CNodeProperty
{
public:
  CNodeProperty();

  virtual ~CNodeProperty();

  void fronJSON(const json_t * json);
};

#endif /* SRC_NETWORK_CNODEPROPERTY_H_ */
