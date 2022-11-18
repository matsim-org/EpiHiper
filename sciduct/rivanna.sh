#!/usr/bin/env bash

# BEGIN: Copyright 
# MIT License 
#  
# Copyright (C) 2020 - 2022 Rector and Visitors of the University of Virginia 
#  
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions: 
#  
# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software. 
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE 
# END: Copyright 

SINGULARITY=${SINGULARITY:-"$(which singularity)"}
DEF=rivanna.def
IMAGE=epihiper.rivanna.sif
ID=$(id -u)

[ -e $IMAGE ] && rm $IMAGE

# Local cache.rivanna
[ -e cache.rivanna ] || mkdir -p cache.rivanna
    
cd cache.rivanna

# EpiHiper
[ -e EpiHiper ] || \
    scp rivanna1.hpc.virginia.edu:/project/biocomplexity/nssac/EpiHiper/build/src/EpiHiper .

# Intel Runtime Libraries
[ -e l_comp_lib_2018.5.274_comp.cpp_redist.tgz ] || \
    wget https://software.intel.com/sites/default/files/managed/b0/e9/l_comp_lib_2018.5.274_comp.cpp_redist.tgz

# Intel MPI
[ -e l_mpi_2018.5.288.tgz ] || \
    wget http://registrationcenter-download.intel.com/akdlm/irc_nas/tec/15614/l_mpi_2018.5.288.tgz
    
# IntelOPA
[ -e IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz ] || \
    wget https://downloadmirror.intel.com/28866/eng/IntelOPA-Basic.RHEL76-x86_64.10.9.3.1.1.tgz
    
cd ..

sudo "${SINGULARITY}" build $IMAGE $DEF | tee rivanna.log

[ -e cache.rivanna ] && sudo chown -R $ID cache.rivanna
[ -e $IMAGE ] && sudo chown -R $ID $IMAGE
