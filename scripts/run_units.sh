#!/bin/bash
set -e
cd ./build/Release/tests
CPU_CORES=$(getconf _NPROCESSORS_ONLN)
ctest -j $CPU_CORES
