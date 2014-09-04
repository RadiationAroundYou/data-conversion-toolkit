#!/bin/bash

#
# This script creates the shared library used by PyROOT to read the Timepix
# data. It should be run before attempting to use the viewer.
#

echo .L Frame.C+ | root -l
