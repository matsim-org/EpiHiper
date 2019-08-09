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

#ifndef SRC_MATH_CVALUELIST_H_
#define SRC_MATH_CVALUELIST_H_

#include <set>

#include "math/CValue.h"

class CValueList : protected std::set< CValue >
{
public:
  typedef CValueInterface::Type Type;
  typedef std::set< CValue >::const_iterator const_iterator;

  CValueList(const Type & type = Type::number);

  CValueList(const CValueList & src);

  CValueList(const json_t * json);

  CValueList(std::istream & is);

  bool append(const CValue & value);

  bool contains(const CValueInterface & value) const;

  virtual ~CValueList();

  const_iterator begin() const;

  const_iterator end() const;

  size_t size() const;

  const bool & isValid() const;

  virtual void fromJSON(const json_t * json);

  void toBinary(std::ostream & os) const;

  void fromBinary(std::istream & is);

protected:
  Type mType;
  bool mValid;
};

#endif /* SRC_MATH_CVALUELIST_H_ */