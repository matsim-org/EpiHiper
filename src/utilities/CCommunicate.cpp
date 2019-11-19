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

#include <algorithm>
#include <unistd.h>
#include <cassert>

#include "utilities/CCommunicate.h"
#include "utilities/CStreamBuffer.h"

// static
int CCommunicate::MPIRank(-1);

// static
int CCommunicate::MPINextRank(-1);

// static
int CCommunicate::MPIPreviousRank(-1);

// static
int CCommunicate::MPIProcesses(-1);

// static
int CCommunicate::ReceiveSize(0);

// static
char * CCommunicate::ReceiveBuffer(NULL);

// static
MPI_Win CCommunicate::MPIWin;

// static
size_t CCommunicate::RMAIndex(0);

// static
size_t CCommunicate::MPIWinSize(0);

// static
double * CCommunicate::RMABuffer(NULL);


// static
void CCommunicate::resizeReceiveBuffer(int size)
{
  if (size <= ReceiveSize) return;

  if (size < 0)
    {
      FatalError(ErrorCode::AllocationError, "");
    }

  if (ReceiveBuffer != NULL)
    {
      delete [] ReceiveBuffer;
      ReceiveSize = 0;
    }

  // Assure we have a valid buffer
  ReceiveSize = std::max(1024, size);

  try {
      ReceiveBuffer = new char[ReceiveSize];
  }

  catch (...) {
      FatalError(ErrorCode::AllocationError, "");
  }
}

// static
void CCommunicate::init(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &MPIRank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPIProcesses);

  MPINextRank = (MPIProcesses + MPIRank + 1) % MPIProcesses;
  MPIPreviousRank = (MPIProcesses + MPIRank - 1) % MPIProcesses;
}

// static
int CCommunicate::abortMessage(ErrorCode err, const std::string & msg, const char * file, int line)
{
  std::cerr << "Rank: " << MPIRank << ", " << file << "(" << line << "): " << msg << std::endl;

  return abort(err);
}

// static
int CCommunicate::abort(ErrorCode errorcode)
{
  return MPI_Abort(MPI_COMM_WORLD, (int) errorcode);
}

// static
int CCommunicate::finalize(void)
{
  if (MPIWinSize)
    {
      MPI_Win_free(&MPIWin);
      delete [] RMABuffer;
      MPIWinSize = 0;
    }

  return MPI_Finalize();
}

// static
int CCommunicate::send(const void *buf,
                      int count,
                      MPI_Datatype datatype,
                      int dest,
                      int tag)
{
  return MPI_Send(buf, count, datatype, dest, tag, MPI_COMM_WORLD);
}

// static
int CCommunicate::receive(void *buf,
                         int count,
                         MPI_Datatype  datatype,
                         int source,
                         int tag,
                         MPI_Status *status)
{
  return MPI_Recv(buf, count, datatype, source, tag, MPI_COMM_WORLD, status);
}

// static
int CCommunicate::broadcast(void *buffer,
                           int count,
                           MPI_Datatype datatype,
                           int root)
{
  return MPI_Bcast(buffer, count, datatype, root, MPI_COMM_WORLD);
}

// static
int CCommunicate::broadcast(const void *buffer,
                           int count,
                           CCommunicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;

  int Count = -1;

  for (int sender = 0; sender < MPIProcesses && Result == ErrorCode::Success; ++sender)
    {
      if (sender != MPIRank)
        {
          broadcast(&Count, 1, MPI_INT, sender);

          if (Count > 0)
            {
              resizeReceiveBuffer(Count);
              // std::cout << Rank << ", " << sender << ": Communicate::broadcast (receive: " << Count << ")"<< std::endl;
              broadcast(ReceiveBuffer, Count, MPI_CHAR, sender);
            }

          CStreamBuffer Buffer(ReceiveBuffer, Count);
          std::istream is(&Buffer);

          Result = (*pReceive)(is, sender);
        }
      else
        {
          Count = count;
          broadcast(&Count, 1, MPI_INT, sender);

          if (Count > 0)
            {
              resizeReceiveBuffer(Count);
              memcpy(ReceiveBuffer, buffer, Count * sizeof(char));
              // std::cout << Rank << ", " << sender << ": Communicate::broadcast (send: " << Count << ")" << std::endl;
              broadcast(ReceiveBuffer, Count, MPI_CHAR, sender);
            }
        }
    }

  return (int) Result;
}

// static
int CCommunicate::sequential(int firstRank, CCommunicate::SequentialProcessInterface * pSequential)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;
  int signal = 0;

  if (MPIRank == firstRank)
    {
      (*pSequential)();

      signal = 1;
      send(&signal, 1, MPI_INT, MPINextRank, firstRank);
      receive(&signal, 1, MPI_INT, MPIPreviousRank, (firstRank - 1) % MPIProcesses, &status);
    }
  else
    {
      receive(&signal, 1, MPI_INT, MPIPreviousRank, MPIPreviousRank,  &status);

      (*pSequential)();

      send(&signal, 1, MPI_INT, MPINextRank, MPIRank);
    }

  return (int) Result;
}

// static
int CCommunicate::allocateRMA()
{
  MPIWinSize = RMAIndex;

  if (MPIWinSize > 0)
    {
      RMABuffer = new double[MPIWinSize];
      return MPI_Win_create(&RMABuffer, MPIWinSize, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &MPIWin);
    }

  return (int) ErrorCode::Success;
}

// static
double CCommunicate::getRMA(const int & index)
{
  if (index >= MPIWinSize)
    return std::numeric_limits< double >::quiet_NaN();

  double Value;

  MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush_local(0, MPIWin);

  return Value;
}

// static
double CCommunicate::updateRMA(const int & index, CCommunicate::Operator pOperator, const double & value)
{
  if (index >= MPIWinSize)
    return std::numeric_limits< double >::quiet_NaN();

  double Value;

  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, MPIWin);
  MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush(0, MPIWin);

  (*pOperator)(Value, value);

  MPI_Put(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush(0, MPIWin);
  MPI_Win_unlock(0, MPIWin);

  return Value;
}

// static
size_t CCommunicate::getRMAIndex()
{
  size_t Index = RMAIndex;
  ++RMAIndex;

  return Index;
}


CCommunicate::~CCommunicate()
{}

CCommunicate::Receive::Receive(CCommunicate::Receive::Type method)
  : CCommunicate::ReceiveInterface()
  , mMethod(method)
{}

// virtual
CCommunicate::Receive::~Receive() {}

// virtual
CCommunicate::ErrorCode CCommunicate::Receive::operator()(std::istream & is, int sender)
{
  // execute member function
  return (*mMethod)(is, sender);
}

CCommunicate::SequentialProcess::SequentialProcess(CCommunicate::SequentialProcess::Type method)
  : CCommunicate::SequentialProcessInterface()
  , mMethod(method)
{}

// virtual
CCommunicate::SequentialProcess::~SequentialProcess() {}

// virtual
CCommunicate::ErrorCode CCommunicate::SequentialProcess::operator()()
{
  // execute member function
  return (*mMethod)();
}


