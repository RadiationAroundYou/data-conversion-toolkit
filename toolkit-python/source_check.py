#!/usr/bin/env python

#============================================
# Source particle validation for CERN@school
#============================================
#

# Import the code needed to manage files.
import os, inspect, glob, argparse

# Import the required ROOT libraries - hurrah for PyROOT!
from ROOT import TFile, gSystem
from ROOT import TH1D, TH2D

# ...for the colour scaling.
from matplotlib import colorbar, colors

# ...for the plotting.
import matplotlib.pyplot as plt

# ...for nice text.
from matplotlib import rc

# Uncomment to use LaTeX for the plot text.
rc('font',**{'family':'serif','serif':['Computer Modern']})
rc('text', usetex=True)

import numpy as np

# Load in the (skeleton) Frame class - a bare-minimum class that
# provides the ROOT file format interface.
gSystem.Load('Frame_C')

#
# The main program.
#
if __name__=="__main__":
    print("*===========================================")
    print("   CERN@school source particle validation   ")
    print("*===========================================")

    # Get the validation file path from the command line.
    parser = argparse.ArgumentParser()
    parser.add_argument("inputPath", \
        help="Path to the validation ROOT file."  )
    parser.add_argument("outputPath", \
        help="Path to the output directory.")
    args = parser.parse_args()

    # The input path of the (Monte Carlo) data to check.
    datapath = args.inputPath

    ## The output path for the results.
    outputpath = args.outputPath

    dbg = True

    print("*")
    print("* The datapath is    : '%s'" % datapath)
    print("* The output path is : '%s'" % outputpath)
    print("*")
    print("* [PROCESSING...]")
    print("*")
    print("*---------------------------------------------")

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


    ## Half the detector width [mm].
    HDx  = 7.04

    ## Pixel density [pixels/mm].
    Dpx = 256./(2.0*HDx)

    ## Centre of the detector (x) in the lab frame [mm].
    c_x = -1.305

    ## Centre of the detector (y) in the lab frame [mm].
    c_y = 23.07

    # Loop over the frames in the file.
    for fn in range(nframes):

        # Load the TTree.
        ientry = chain.LoadTree(fn)

        # Copy the entry into memory.
        nb = chain.GetEntry(fn)

        if dbg:
            print("* Frame %5d" % (fn))

        # The source particle positions
        xs = chain.FramesData.GetSourcePos_xs()
        ys = chain.FramesData.GetSourcePos_ys()
        zs = chain.FramesData.GetSourcePos_zs()

        kinEs = chain.FramesData.GetSource_kinEs()

        hitpixels = {}

        xlist = []
        ylist = []
        zlist = []
        kinElist = []

        for i, x in enumerate(xs):
            #print("*--> Source particle %5d: X = (%8.3f,%8.3f,%8.3f)" % (i, xs[i], ys[i], zs[i]))
            xlist.append(xs[i])
            ylist.append(ys[i])
            zlist.append(zs[i])
            kinElist.append(kinEs[i])
            x_ = (xs[i] - (c_x + HDx)) * Dpx
            y_ = (ys[i] - (c_y + HDx)) * Dpx
            x_ = (xs[i] - c_x + HDx) * Dpx
            y_ = (ys[i] - c_y + HDx) * Dpx
            #print("*----> [%3d,%3d]" % (x_,y_))
            hitpixels[256*y_ + x_] = 10

        # Create the mpl Figure and FigCanvas objects.
        # 5x4 inches, 100 dots-per-inch
        #
        dpi = 72

        ## The colour map for the pixel counts.
        cmap = plt.cm.hot # "Hot" used.

        ## The current maximum count value.
        cmax = max(hitpixels.values())

        # The figure and array of axes for the frame display.
        mapfig, mapaxarr = plt.subplots(1, 1, figsize=(8.0,8.0), dpi=dpi)

        #print xlist
        #print ylist

        plt.scatter(xlist,ylist)

        # Remove the axis labels.
        #mapaxarr.xaxis.set_visible(False)
        #mapaxarr.yaxis.set_visible(False)
        #mapaxarr.set_xlim([0,256])
        #mapaxarr.set_ylim([0,256])
        mapaxarr.set_xlim([-8.345,5.735])
        mapaxarr.set_ylim([16.03,30.11])

        # Clear the display.
        #mapaxarr.clear()

        # Set the background colour.
        mapaxarr.set_axis_bgcolor('#82bcff')

        # Add a grid to the plot.
        mapaxarr.grid(1)

        # Set the plot title.
        mapaxarr.set_title("Frame %d" % (fn))

#        # Loop over the pixels in the pixel dictionary.
#        for X, C in hitpixels.iteritems():
#            ## The pixel x value.
#            x = X%256;
#
#            ## The pixel y value.
#            y = X/256
#            #print("* DEBUG: Pixel found at (%3d, %3d) with C = %d" % (x,y,C))
#            if C > 1:
#                color = float(C) / float(cmax)
#                #print("* DEBUG: C = %d - setting color to %f" % (C, color))
#
#                # Plot the pixel.
#                mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor=cmap(color), facecolor=cmap(color)))
#            elif C > 0:
#                mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor='#449bff', facecolor='#449bff'))
#            else: # negative values - masked.
#                mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor='#33aacc', facecolor='#33aacc'))


        mapfig.savefig(outputpath + "/%d.png" % (fn))

    f.Close()

    ## ROOT file to output the results to.
    of = TFile(outputpath + "/out.root", "RECREATE")

    print("* Number of source particles = %d" % (len(xlist)))

    hg_xz = TH2D("xz", "", 120, -60.0, 60.0, 120, -60.0, 60.0)
    hg_xy = TH2D("xy", "", 120, -60.0, 60.0, 120, -60.0, 60.0)
    hg_zy = TH2D("zy", "", 120, -60.0, 60.0, 120, -60.0, 60.0)

    hg_e_kinE = TH1D("e_kinE", "", 200, 0.0, 10.0)

    #countE = 0

    for i, x in enumerate(xlist):
        hg_xz.Fill(xlist[i], zlist[i])
        hg_xy.Fill(xlist[i], ylist[i])
        hg_zy.Fill(zlist[i], ylist[i])

        hg_e_kinE.Fill(kinElist[i])
        #if kinElist[i] > 0.1:
        #    countE += 1

    #print countE

    of.Write()
    of.Close()
