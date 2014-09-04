#!/usr/bin/env python

#===================================
# CERN@school Particle Rate Plotter
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
    print("==============================================")
    print("  CERN@school Particle Rate Plotter (Python)  ")
    print("==============================================")

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

    ## List for the number of single pixel clusters in each frame.
    n_g1_l  = []

    ## List for the number of bipixel clusters in each frames.
    n_g2_l  = []

    ## List for the number of tripixel gamma candidates in each frame.
    n_g3_l  = []

    ## List for the number of tetrapixel gamma candidates in each frame.
    n_g4_l  = []

    ## List for the total number of gamma candidates in each frames.
    n_g_l   = []

    ## List for the number of tripixel beta candidates in each frame.
    n_b3_l  = []

    ## List for the number of tetrapixel beta candidates in each frames.
    n_b4_l  = []

    ## List for the total number of small beta candidates in each frame.
    n_bs_l  = []

    ## List for the number of large clusters in each frames.
    n_lc_l  = []

    ## List for the number of non-gamma candidate clusters in each frame.
    n_nongam_l = []

    # Get the pixel data from the ROOT file, looping over the frames.

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

        # Calculate the total number of gamma candidates.
        n_g  = n_g1 + n_g2 + n_g3 + n_g4

        # Calculate the total number of small beta candidates.
        n_bs = n_b3 + n_b4

        # Calculate the total number of non-gamma candidates.
        n_nongam = n_bs + n_lc

        # A check on the total number of clusters found.
        totcheck = n_g + n_bs + n_lc

        # Add to the cluster lists.
        n_g1_l.append(n_g1)
        n_g2_l.append(n_g2)
        n_g3_l.append(n_g3)
        n_g4_l.append(n_g4)
        #
        n_g_l.append(n_g)
        #
        n_b3_l.append(n_b3)
        n_b4_l.append(n_b4)
        n_bs_l.append(n_bs)
        #
        n_lc_l.append(n_lc)
        #
        n_nongam_l.append(n_nongam)

    # Close the data file.
    f.Close()

    ## Output string for the rates.JSON file.
    of = "{\n"
    of += "    \"units\": \"$\\\\textrm{min.}^{-1}$\",\n"

    # Plot the various particle rate estimates.

    nclplot = RateHistogram(100, "ncl", Delta_t, outputpath, dbg)
    of += "    \"ncl\": [%f, %f],\n" % (nclplot.fill(n_cl_l))

    ng1plot = RateHistogram(101, "ng1", Delta_t, outputpath, dbg)
    of += "    \"ng1\": [%f, %f],\n" % (ng1plot.fill(n_g1_l))

    ng2plot = RateHistogram(102, "ng2", Delta_t, outputpath, dbg)
    of += "    \"ng2\": [%f, %f],\n" % (ng2plot.fill(n_g2_l))

    ng3plot = RateHistogram(103, "ng3", Delta_t, outputpath, dbg)
    of += "    \"ng3\": [%f, %f],\n" % (ng3plot.fill(n_g3_l))

    ng4plot = RateHistogram(104, "ng4", Delta_t, outputpath, dbg)
    of += "    \"ng4\": [%f, %f],\n" % (ng4plot.fill(n_g4_l))

    ngplot = RateHistogram(110, "ng", Delta_t, outputpath, dbg)
    of += "    \"ng\" : [%f, %f],\n" % (ngplot.fill(n_g_l))

    nb3plot = RateHistogram(113, "nb3", Delta_t, outputpath, dbg)
    of += "    \"nb3\": [%f, %f],\n" % (nb3plot.fill(n_b3_l))

    nb4plot = RateHistogram(114, "nb4", Delta_t, outputpath, dbg)
    of += "    \"nb4\": [%f, %f],\n" % (nb4plot.fill(n_b4_l))

    nbsplot = RateHistogram(117, "nbs", Delta_t, outputpath, dbg)
    of += "    \"nbs\": [%f, %f],\n" % (nbsplot.fill(n_bs_l))

    nlcplot = RateHistogram(120, "nlc", Delta_t, outputpath, dbg)
    of += "    \"nlc\": [%f, %f],\n" % (nlcplot.fill(n_lc_l))

    nngplot = RateHistogram(130, "nng", Delta_t, outputpath, dbg)
    of += "    \"nng\": [%f, %f]\n" % (nngplot.fill(n_nongam_l))

    of += "}"

    print("* Particle candidate rates (JSON):")
    print of
    print("*")
    print("* See '%s' for detailed plots." % (outputpath+"/index.html"))
    print("*")

    ## JSON file for the rate results.
    jf = open(outputpath+"/rates.json","w")

    # Write out the JSON file.
    jf.write(of)

    # Close the JSON file.
    jf.close()

    ## Web page for particle rate display.
    hf = open(outputpath+"/index.html", "w")

    hf.write("<!DOCTYPE html>\n")
    hf.write("<html>\n")
    hf.write("  <body>\n")
    hf.write("    <h2>Particle rate plots for '%s'</h2>\n" % (datapath))
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ncl.png"))
    hf.write("      <figcaption><em>Fig. 1: Estimated rate for all clusters.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <h3>Basic groupings</h3>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ng.png"))
    hf.write("      <figcaption><em>Fig. 2: Estimated rate for all gamma candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("nbs.png"))
    hf.write("      <figcaption><em>Fig. 3: Estimated rate for small beta candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("nlc.png"))
    hf.write("      <figcaption><em>Fig. 4: Estimated rate for large clusters.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <h3>Gamma candidates</h3>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ng1.png"))
    hf.write("      <figcaption><em>Fig. 5a: Estimated rate for monopixel clusters.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ng2.png"))
    hf.write("      <figcaption><em>Fig. 5b: Estimated rate for bipixel clusters.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ng3.png"))
    hf.write("      <figcaption><em>Fig. 5c: Estimated rate for tripixel gamma candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("ng4.png"))
    hf.write("      <figcaption><em>Fig. 5d: Estimated rate for tetrapixel gamma candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <h3>Small beta candidates</h3>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("nb3.png"))
    hf.write("      <figcaption><em>Fig. 6a: Estimated rate for tripixel beta candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("nb4.png"))
    hf.write("      <figcaption><em>Fig. 6b: Estimated rate for tetrapixel beta candidates.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <h3>Non-gamma candidates</h3>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % ("nng.png"))
    hf.write("      <figcaption><em>Fig. 7: Estimated rate for non-gamma candidate clusters.</em></figcaption>\n")
    hf.write("    </figure>\n")
    hf.write("    <h3>JSON of the rate data</h3>\n")
    hf.write("    <textarea rows=\"14\" cols=\"80\">\n")
    hf.write(of)
    hf.write("    </textarea>\n")
    hf.write("  </body>\n")
    hf.write("</html>")

    # Close the HTML output file.
    hf.close()
