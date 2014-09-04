#!/usr/bin/env python

"""

==============================================================================
                         CERN@school - Get the Run ID
==============================================================================

A short script for getting the run ID from the DSC files.

"""

# Import the code needed to manage files.
import sys, os, inspect, glob

# Import the argument parser functionality.
import argparse

# Import the time functionality.
import time

#
# The main event!
#
if __name__ == "__main__":

    dbg = False

    # Get the datafile path from the command line.
    parser = argparse.ArgumentParser()
    parser.add_argument("inputPath", help="The path of the Pixelman data files."  )

    ## The command line arguments.
    args = parser.parse_args()

    ## The path to the input data.
    inputPath = args.inputPath

    # Update the user.
    if dbg:
        print("==============================================")
        print("       CERN@school Get Run ID (Python)        ")
        print("==============================================")
        print("*")
        print("* Datapath is: '%s'" % (inputPath))

    # Get the first DSC file in the input data path.
    f = open(sorted(glob.glob(inputPath + "/*.txt.dsc"))[0], 'r')

    ## String for the chip ID (obtained from the DSC file).
    chipid = ""

    # Read the DSC file and scan it for the relevant lines.
    dsclines = f.readlines()
    for i, line in enumerate(dsclines):
       # If the line contains the metadata identifier "ChipboardID",
       # get the chip ID from two lines down.
       if "ChipboardID" in line:
           chipid = dsclines[i+2].strip()
       #
       # If the line contains the start time identifier, obtain the
       # time. Note that the decimal part needs to be removed to
       # get the epoch time in seconds.
       if '\"Start time\" (\"Acquisition start time\"):' in line:
           epochtime = int(dsclines[i+2].split('.')[0])

    ## The time as extracted from the DSC file.
    #
    # FIXME: I may need to think about whether it is best to use UTC
    # with gmtime() rather than the local time to get a match.
    # Hmm.
    #mytime = time.gmtime(epochtime)
    mytime = time.localtime(epochtime)

    ## The run ID to return, formed from the chip ID and the
    ## start second of the run.
    runid = chipid+"_"+time.strftime("%Y-%m-%d-%H%M%S", mytime)

    # Print the results for the user.
    print runid

    # Close the DSC file.
    f.close()
