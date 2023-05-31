// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include "actions/CNodeAction.h"
#include "actions/CActionDefinition.h"

CNodeAction::CNodeAction(const CActionDefinition * pDefinition, const CNode * pTarget)
  : CAction()
  , mpDefinition(pDefinition)
  , mpTarget(pTarget)
{}

// virtual 
CNodeAction::~CNodeAction()
{}

// virtual 
size_t CNodeAction::getOrder() const
{
  return mpDefinition->getOrder();
}

// virtual 
bool CNodeAction::execute() const
{
  return mpDefinition->execute(const_cast< CNode * >(mpTarget));
}
