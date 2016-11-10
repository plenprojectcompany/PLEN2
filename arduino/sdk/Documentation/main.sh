#!/bin/bash

# author    : Kazuyuki TAKASE
# copyright : PLEN Project Company Inc, and all authors.
# license   : The MIT License


cd ../../
REPOSITORY_ROOT=`pwd`


# Set up documents directory.
# =============================================================================
rm -rf docs
mkdir docs
cd ./docs
touch .nojekyll


# Run the documentation program.
# =============================================================================
cd "${REPOSITORY_ROOT}/firmware"
doxygen
