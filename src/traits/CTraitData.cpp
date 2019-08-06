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

#include <cstring>
#include <assert.h>
#include <iostream>

#include "traits/CTrait.h"
#include "traits/CTraitData.h"

// static
bool CTraitData::hasValue(const CTraitData::base & data, const CTraitData::value & value)
{
  return (data & value.first) == value.second;
}

// static
void CTraitData::setValue(CTraitData::base & data, const CTraitData::value & value)
{
  base F(value.first);

  data &= ~F;
  data |= value.second;
}

// static
CTraitData::value CTraitData::getValue(const CTraitData::base & data, const CTraitData::base & feature)
{
  return value(feature, data & feature);
}

CTraitData::CTraitData(CTraitData::base val)
  : std::bitset< 32 >((unsigned long long) val)
{}

CTraitData::CTraitData(const CTraitData & src)
  : std::bitset< 32 >(src)
{}

CTraitData::CTraitData(const CTrait & trait)
  : std::bitset< 32 >(0)
{
  assert(trait.size() == 4);
}

CTraitData::~CTraitData()
{}

void CTraitData::setBits(const size_t & begin, const size_t & beyond)
{

  if (begin >= beyond || beyond >= 32) return;

  for (size_t i = begin; i < beyond; ++i)
    {
      std::bitset< 32 >::set(i);
    }
}

size_t CTraitData::firstBit() const
{
  size_t i = 0;

  for (; i < 32; ++i)
    if (std::bitset< 32 >::operator[](i)) break;

  return i;
}
