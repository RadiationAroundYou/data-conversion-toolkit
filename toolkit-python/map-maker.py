#!/usr/bin/env python

#===================================
# CERN@school Map Maker
#===================================
#
# See the README.md for more information.

# Import the code needed to manage files.
import os, inspect, glob, argparse

# Import the plotting libraries.
import pylab as plt

# Import the plotting and math libraries.
from matplotlib import rc
import numpy as np

# Uncomment to use LaTeX for the plot text.
rc('font',**{'family':'serif','serif':['Computer Modern']})
rc('text', usetex=True)

# Import the clustering code.
from clustering import *

# Import the webpage writing code.
#from pagemaker  import *

# Import the (custom) plotting code.
from Plots import *

# Import the required ROOT libraries - hurrah for PyROOT!
from ROOT import TFile, gSystem
from ROOT import TH1D, TH2D

# Import the JSON library.
import json

# Load in the (skeleton) Frame class - a bare-minimum class that
# provides the ROOT file format interface.
gSystem.Load('Frame_C')

## Get the path of the current directory
#path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

#
# The main program.
#
if __name__=="__main__":
    print("==================================")
    print("  CERN@school Map Maker (Python)  ")
    print("==================================")

    # Get the datafile path from the command line
    parser = argparse.ArgumentParser()
    parser.add_argument("inputPath",  help="Path to the file containing your data.")
    parser.add_argument("outputPath", help="The path for the output files.")
    args = parser.parse_args()

    ## The path to the data file.
    datapath = args.inputPath

    ## The output path.
    outputpath = args.outputPath

    dbg = False

    print("*")
    print("* The datapath is    : '%s'" % datapath)
    print("* The output path is : '%s'" % outputpath)
    print("*")
    print("* [PROCESSING...]")
    print("*")
    print("*---------------------------------------------")

    ## List for the total number of clusters in each frame.
    n_cl_l  = []

    # Start the CSV file.
    mf = open(outputpath + "/map.csv", "w")
    mm = ""
    mm += "Latitude,Longitude,Altitude,NumClusters\n"

    ## The ROOT file itself.
    f = TFile(datapath)

    ## The TTree containing the data.
    chain = f.Get('MPXTree')

    ## The number of frames in the file.
    nframes = chain.GetEntriesFast()

    # Update the user.
    if dbg:
        print("*")
        print("* Number of frames found = %d" % (nframes))

    ## A dictionary of the hit pixels in all frames of the dataset.
    hitpixels = {}

    Delta_t = 0.0

    # Loop over the frames in the file.
    for fn in range(nframes):

        # Load the TTree.
        ientry = chain.LoadTree(fn)

        # Copy the entry into memory.
        nb = chain.GetEntry(fn)

        # Get the geospatial information.
        lat = chain.FramesData.GetLatitude()
        lon = chain.FramesData.GetLongitude()
        alt = chain.FramesData.GetAltitude()

        ## The hit pixels in the frame (C++ std::map).
        pxs = chain.FramesData.GetFrameXC()

        # A dictionary for the pixel information [X:C].
        pixels = {}

        # Get the masked pixels from the file.
        maskedPixels = chain.FramesData.GetLVL1()

        # Get the run acquisition time.
        Delta_t = chain.FramesData.GetAcqTime()

        ## A dictionary for the pixel mask (as the ROOT object is a C++ map).
        mask = {}

        # Loop over the mask map and create the dictionary.
        for i in maskedPixels:
             if i.second == 1:
                 mask[i.first] = i.second

        # Loop over the pixels and extract the coordinates.
        for px in pxs:
            # If the pixel isn't masked, add it to the
            if px.first not in mask.keys():
                pixels[px.first] = px.second
                #X = px.first
                #x = X%256
                #y = X/256

        # Create a "BlobFinder" to cluster the pixels we've just extracted.
        # See clustering.py for more about how this is done.
        blob_finder = BlobFinder(pixels, 256, 256)

        # Add the total number of clusters in the frame to the list.
        n_cl_l.append(len(blob_finder.blob_list))

        # Number of clusters (by type).
        n_cl     = 0 # Total number of clusters.
        n_g      = 0 # Total gamma (g1 + g2 + g3 + g4).
        n_g1     = 0 # Monopixel gamma.
        n_g2     = 0 # Bipixel gamma.
        n_g3     = 0 # Tripixel gamma.
        n_g4     = 0 # Tetrapixel gamma.
        n_bs     = 0 # Small beta (b3 + b4)
        n_b3     = 0 # Beta (h = 3, r > 0.75)
        n_b4     = 0 # Beta (h = 4, r > 0.71)
        n_lc     = 0 # Large clusters (h > 4)
        n_nongam = 0 # Non-gamma candidates

        # Loop over the blobs found in the blob finder and record their
        # properties for plotting.
        for b in blob_finder.blob_list:
            # Monopixel gamma.
            if   b.get_size() == 1:
                n_g1 += 1
            # Bipixel gamma.
            elif b.get_size() == 2:
                n_g2 += 1
            # Tripixel...
            elif b.get_size() == 3:
                # Tripixel gamma.
                if b.r_u < 0.75:
                    n_g3 += 1
                # Tripixel beta.
                else:
                    n_b3 += 1
            # Tetrapixel...
            elif b.get_size() == 4:
                # Tetrapixel gamma.
                if b.r_u < 0.71:
                    n_g4 += 1
                # Tetrapixel beta
                else:
                    n_b4 += 1
            # Large cluster (h > 4).
            else:
                n_lc += 1

        mm += "%.6f,%.6f,%.6f,%d\n" % (lat,lon,alt, len(blob_finder.blob_list))

        # Calculate the total number of gamma candidates.
        n_g  = n_g1 + n_g2 + n_g3 + n_g4

        # Calculate the total number of small beta candidates.
        n_bs = n_b3 + n_b4

        # Calculate the total number of non-gamma candidates.
        n_nongam = n_bs + n_lc

        # A check on the total number of clusters found.
        totcheck = n_g + n_bs + n_lc


    mf.write(mm)
    mf.close()
