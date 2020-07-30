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

#ifndef UTILITIES_CLOGGER_H
#define UTILITIES_CLOGGER_H

#include <sstream>
#include <stack>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include "utilities/CContext.h"

struct LoggerData
{
  std::shared_ptr< spdlog::logger > pLogger;
  std::shared_ptr< spdlog::sinks::sink > pConsole;
  std::shared_ptr< spdlog::sinks::sink > pFile;
  std::string task;
  std::stack< spdlog::level::level_enum > levels;
};

class CLogger
{
private:
  template < int level >
  class CStream : public std::ostringstream
  {
  public:
    CStream();
    CStream(const std::string & msg);
    virtual ~CStream();
    void flush();

  private:
    static void flush(const std::string & msg);
  };

public : 
  typedef spdlog::level::level_enum LogLevel;

  struct LoggerData
  {
    std::shared_ptr< spdlog::logger > pLogger;
    std::shared_ptr< spdlog::sinks::sink > pConsole;
    std::shared_ptr< spdlog::sinks::sink > pFile;
    std::string task;
    std::stack< spdlog::level::level_enum > levels;
  };

  static void init();

  static void release();

  static void setLevel(LogLevel level);

  static void pushLevel(LogLevel level);

  static void popLevel();

  static void setTask(int rank, int processes);

  static void setLogDir(const std::string dir);
  
  static std::string sanitize(std::string dirty);

  static bool hasErrors();

  static void setSingle(bool single);

  typedef CStream< spdlog::level::trace > trace;
  typedef CStream< spdlog::level::debug > debug;
  typedef CStream< spdlog::level::info > info;
  typedef CStream< spdlog::level::warn > warn;
  typedef CStream< spdlog::level::err > error;
  typedef CStream< spdlog::level::critical > critical;
  typedef CStream< spdlog::level::off > off;

private:
  static void initData(LoggerData & loggerData);
  static void releaseData(LoggerData & loggerData);
  static void setLevel();
  static bool haveErrors;
  static int single;
  static CContext< LoggerData > Context;
};

template < int level >
CLogger::CStream< level >::CStream()
  : std::ostringstream()
{
  if (Context.Active().levels.top() > level)
    setstate(std::ios_base::badbit);
}

template < int level >
CLogger::CStream< level >::CStream(const std::string & msg)
  : std::ostringstream()
{
  if (Context.Active().levels.top() <= level)
    flush(msg);

  setstate(std::ios_base::badbit);
}

// virtual
template < int level >
CLogger::CStream< level >::~CStream()
{
  flush();
}

template < int level >
void CLogger::CStream< level >::flush()
{
  if (bad()
      || tellp() == 0)
    return;

  flush(str());
  str("");
}

// static
template < int level >
void CLogger::CStream< level >::flush(const std::string & msg)
{
  LoggerData * pIt = NULL;
  LoggerData * pEnd = NULL;

  if ((single != -1
       && Context.localIndex(&Context.Active()) == single)
      || __GET_NUM_THREADS == 1)
    {
      pIt = Context.beginThread();
      pEnd = Context.endThread();
    }
  else
    {
      pIt = &Context.Active();
      pEnd = pIt + 1;
    }

  for (; pIt != pEnd; ++pIt)
    switch (static_cast< LogLevel >(level))
      {
      case spdlog::level::trace:
        pIt->pLogger->trace(pIt->task + " " + msg);
        break;
      case spdlog::level::debug:
        pIt->pLogger->debug(pIt->task + " " + msg);
        break;
      case spdlog::level::info:
        pIt->pLogger->info(pIt->task + " " + msg);
        break;
      case spdlog::level::warn:
        pIt->pLogger->warn(pIt->task + " " + msg);
        break;
      case spdlog::level::err:
  #pragma omp atomic
        haveErrors |= true;
        pIt->pLogger->error(pIt->task + " " + msg);
        break;
      case spdlog::level::critical:
  #pragma omp atomic
        haveErrors |= true;
        pIt->pLogger->critical(pIt->task + " " + msg);
        break;
      case spdlog::level::off:
        break;
      }
}

#endif // UTILITIES_CLOGGER_H