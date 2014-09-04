#!/usr/bin/env python

#=================================================
# data-conversion-toolkit - build and make script
#=================================================

import os, inspect
import sys
import argparse
import fileinput
import subprocess
import shutil

def replace_text(filename, stringtomatch, stringtoreplace):
    for line in fileinput.input(filename, inplace = 1):
        print line.replace(stringtomatch, stringtoreplace),

if __name__=="__main__":

    # Get the path of the current directory
    path = os.path.dirname(
        os.path.abspath(inspect.getfile(inspect.currentframe())))

    print
    print(" * Current working directory is %s" % (path))
    print(" *")

    # Generate the ROOT dictionary
    #
    # Run the rootcint command
    rootcintcmd = [
        "rootcint", \
        "-f", \
        "toolkitDict.cc", \
        "-c", \
        "../toolkit/include/Frames.h", \
        "../toolkit/include/WriteToNtuple.h", \
        "../toolkit/include/LinkDef.h"
        ]
    subprocess.call(rootcintcmd)
    # This generates toolkitDict.h and toolkitDict.cc in the current
    # directory.
    #
    # Remove the "../toolkit/include/" from the include statements so that
    # make can find the right files.
    replace_text("toolkitDict.h", "../toolkit/include/", "")
    print(" * Removed '../toolkit/include/' from the dictionary header file.")
    print
    replace_text("toolkitDict.cc", "../toolkit/include/", "")
    print(" * Removed '../toolkit/include/' from the dictionary implementation file.")
    print
    #
    # Move toolkitDict.h to ../toolkit/include...
    dictheadername = path + "/toolkitDict.h"
    dictheaderdest = path + "/../toolkit/include/toolkitDict.h"
    shutil.move(dictheadername,dictheaderdest)
    #
    # ...and toolkitDict.cc to ../toolkit/src/.
    dictimpname = path + "/toolkitDict.cc"
    dictimpdest = path + "/../toolkit/src/toolkitDict.cc"
    shutil.move(dictimpname, dictimpdest)

    # Build the toolkit
    modulesarg = "-DCMAKE_MODULE_PATH="+path+"/Modules/"
    buildcmd = [
        "cmake", \
        modulesarg, \
        "../toolkit"
        ]
    subprocess.call(buildcmd)
    makecmd = ["make"]
    subprocess.call(makecmd)
