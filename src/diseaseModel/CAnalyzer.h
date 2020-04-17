// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#ifndef SRC_DISEASEMODEL_CANALYZER_H_
#define SRC_DISEASEMODEL_CANALYZER_H_

#include <string>

#include "diseaseModel/CModel.h"
#include "utilities/CLogger.h"

struct json_t;

class CAnalyzer
{
public:
  struct StateStatistics
  {
    CModel::state_t healthState;
    size_t total;
    size_t * histogramIn;
    size_t * histogramOut;
  };

  static void Init();

  static void Load(const std::string & jsonFile);

  static void Run();

  static void Release();

  static const size_t & Seed();

  static const std::string & StatusFile();
  
  virtual ~CAnalyzer();

private:
  CAnalyzer() = delete;

  CAnalyzer(const std::string & file);

  void run();

  static CAnalyzer * INSTANCE;

  size_t mSeed;
  size_t mMaxTick;
  size_t mSampleSize;
  StateStatistics * mData;
  CModel * mpModel;
  std::string mOutput;
  std::string mStatus;
  CLogger::LogLevel mLogLevel;
};

#endif // SRC_DISEASEMODEL_CANALYZER_H_