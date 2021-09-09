#!/usr/bin/env bash

# BEGIN: Copyright 
# Copyright (C) 2021 Rector and Visitors of the University of Virginia 
# All rights reserved 
# END: Copyright 

# BEGIN: License 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#   http://www.apache.org/licenses/LICENSE-2.0 
# END: License 

jobId=${SLURM_JOB_ID:-222222}
jobName=${SLURM_JOB_NAME:-testAddNoise}
CONFIG_FILE=$1
CONFIG_FILE=${CONFIG_FILE:-"/input/runParameters"}
statusFile=`cat $CONFIG_FILE | jq -r .status`
statusFile=${statusFile:-"/job/sciduct.status.json"}

[ -e ${statusFile} ] || \
    /epihiper/bin/epiHiperStatus -i "${jobId}" -n "${jobName}" -s running -p 0 ${statusFile}

/epihiper/bin/epiHiperStatus -d "Adding Noise" ${statusFile}
retval=0

pushd /input
if [ -e addNoise.sh ]; then
  chmod a+x addNoise.sh
  ./addNoise.sh
  let retval=$?
fi
popd

if [ ${retval} != 0 ]; then
  /epihiper/bin/epiHiperStatus -s failed ${statusFile}
  exit ${retval}
fi

/epihiper/bin/epiHiperStatus -s completed -d "Finished" -p 100 ${statusFile}
exit 0