#!/usr/bin/env python

"""

==============================================================================
                             CERN@school Mask Maker
==============================================================================

A small script for producing a pixel filter mask [X Y C] format for a given
data file, based on observing a pixel hit more frequently than one would
expect.

FIXME: Plenty!

"""

# Import the code needed to manage files.
import sys, os, inspect, glob

# Import the argument parser functionality.
import argparse

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
# The main event!
#
if __name__ == "__main__":

    print("==============================================")
    print("     CERN@school ROOT Mask Maker (Python)     ")
    print("==============================================")

    dbg = True

    # Get the datafile paths from the command line.
    parser = argparse.ArgumentParser()
    parser.add_argument("inputFilePath", help="The path of the ROOT Timepix data file.")
    parser.add_argument("filterFilename", help="The name of the filter mask file.")
    parser.add_argument("outputPath", help="The path for the output files."         )
    parser.add_argument("outputBaseName", help="The base name for the output files.")
    parser.add_argument("maskThreshold", help="The threshold for applying the mask (n_hits).")
    args = parser.parse_args()

    # Process the input arguments.

    ## The full path of the input Mafalda ROOT file to be analysed.
    datapath = args.inputFilePath

    ## The desired filename for the filter file.
    filterfilename = args.filterFilename

    ## The full path for the output.
    outputpath = args.outputPath

    ## The basename for the output files.
    outputnamebase = args.outputBaseName

    ## The threshold (number of hits) for applying a mask to a given pixel.
    #
    # FIXME - error handling for bad input.
    threshold = int(args.maskThreshold)


    # Get the output file names from the input arguments.

    ## The path + filename of the analysis summary HTML page.
    htmlfilename = outputpath+"/"+outputnamebase+".html"

    ## The path + filename of the ROOT analysis file.
    rootfilename = outputpath+"/"+outputnamebase+".root"

    ## The path + filename of the hit frequency map.
    maskmapfilename = outputnamebase+"_map.png"

    ## The path of the hit frequency map.
    maskmapfilepath = outputpath + "/" + maskmapfilename

    ## The filename of the pixel hit frequency histogram.
    hitfreqplotfilename = outputnamebase+"_hitfreq.png"

    ## The path of the hit frequency histogram.
    hitfreqplotfilepath = outputpath + "/" + hitfreqplotfilename

    ## The actual ROOT analysis file (for manual inspection).
    hf = TFile(rootfilename, "RECREATE")

    # Create the 2D histogram for the (integrated) pixel hits.
    hg = TH2D("hits","",256,0.,256,256,0.,256.0)

    ## The ROOT file itself.
    f = TFile(datapath)

    ## The TTree containing the data.
    chain = f.Get('MPXTree')

    ## The number of frames in the file.
    nframes = chain.GetEntriesFast()

    # Update the user.
    if dbg:
        print
        print("* Input path is: '%s'" % (datapath))
        print("* The output is being written to:")
        print("*--> Filter file            : %s" % (filterfilename))
        print("*--> Summary HTML page      : %s" % (htmlfilename))
        print("*--> Hit frequency map      : %s" % (maskmapfilename))
        print("*--> Hit frequency histogram: %s" % (hitfreqplotfilename))
        print("*--> Analysis ROOT file     : %s" % (rootfilename))
        print("*")
        print("* Number of frames found = %d" % (nframes))

    ## A dictionary of the hit pixels in all frames of the dataset.
    hitpixels = {}

    # Loop over the frames in the file.
    for fn in range(nframes):

        # Load the TTree.
        ientry = chain.LoadTree(fn)

        # Copy the entry into memory.
        nb = chain.GetEntry(fn)

        ## The hit pixels in the frame.
        pxs = chain.FramesData.GetFrameXC()

        # Loop over the pixels and extract the coordinates.
        for px in pxs:
            X = px.first
            x = X%256
            y = X/256
            hg.Fill(x,y)

            # Add the hit pixels to the integral map.
            if X in hitpixels.keys():
                hitpixels[X] += 1
            else:
                hitpixels[X] = 1

    # Close the data file.
    f.Close()

    # Go back to the output histogram file.
    hf.cd()

    ## A histogram of the number of times a given pixel is hit.
    #
    # To be used to establish (by eye) the threshold for a "noisy" pixel.
    dist = TH1D("numhits","",100,0.,100.)

    ## The output file for the filter.
    maskfile = open(outputpath + "/" + filterfilename,"w")

    print("* Masking pixels that have been `hit' >= %d times." % (threshold))

    maskdict = {}

    # Loop over the hit pixels.
    for key, val in hitpixels.iteritems():

        # Plot the distribution of the pixel hit frequency.
        dist.Fill(val)

        # Extract the x and y coordinates.
        x = key%256; y = key/256

        # If a pixel is hit more than a given number of times, add it to
        # the mask.
        if val >= threshold:
            maskdict[key] = val
            maskfile.write("%d\t%d\t1\n" % (x, y))
            #print("%d\t%d\t1" % (x, y))

    # Close the mask file.
    maskfile.close()

    # Save to the histogram file and close.
    hf.Write()
    hf.Close()

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

    # Remove the axis labels.
    mapaxarr.xaxis.set_visible(False)
    mapaxarr.yaxis.set_visible(False)
    mapaxarr.set_xlim([0,256])
    mapaxarr.set_ylim([0,256])

    # Clear the display.
    mapaxarr.clear()

    # Set the background colour.
    mapaxarr.set_axis_bgcolor('#82bcff')

    # Add a grid to the plot.
    mapaxarr.grid(1)

    # Set the plot title.
    mapaxarr.set_title("Filter map: \\texttt{%s} - %d pixels masked." % (outputnamebase.replace("_","\_"), len(maskdict)))
    print("* Masking %d pixels." % (len(maskdict)))

    # Loop over the pixels in the pixel dictionary.
    for X, C in hitpixels.iteritems():
        ## The pixel x value.
        x = X%256;

        ## The pixel y value.
        y = X/256
        #print("* DEBUG: Pixel found at (%3d, %3d) with C = %d" % (x,y,C))
        if C >= threshold:
            color = float(C) / float(cmax)
            #print("* DEBUG: C = %d - setting color to %f" % (C, color))

            # Plot the pixel.
            mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor=cmap(color), facecolor=cmap(color)))
        elif C > 0:
            mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor='#449bff', facecolor='#449bff'))
        else: # negative values - masked.
            mapaxarr.add_patch(plt.Rectangle((x,y),1,1, edgecolor='#33aacc', facecolor='#33aacc'))

    mapfig.savefig(maskmapfilepath)

    # Fig. 1: Hits Per Pixel frequency histogram.
    hppplot = plt.figure(101, figsize=(5.0, 3.0), dpi=150, facecolor='w', edgecolor='w')
    hppplot.subplots_adjust(bottom=0.17, left=0.15)
    hppplotax = hppplot.add_subplot(111)
    # y axis
    plt.ylabel('Number of pixels')
    # x axis
    plt.xlabel('$N_h$')
    #
    plt.grid(1)
    #
    plt.uselogy = True
    numhits = hitpixels.values()

    # Round this up to the nearest 10.
    hpp_max_x    = 10 * (np.floor(max(numhits)/10.) + 1)
    #print("* x max                          : %4d" % (hpp_max_x))

    # Create the histogram.
    n, bins, patches = plt.hist(numhits, range(0,int(hpp_max_x),1), histtype='stepfilled')
    plt.setp(patches, 'facecolor', 'g', 'alpha', 0.75, 'linewidth', 0.0)

    # Find the maximum y value after the N_h bin
    ymax =  10 * (np.floor(sorted(n)[-2]/10.) + 1)

    # Set the y_max of the plot.
    hppplotax.set_ylim([0.,ymax])

    # Set the plot title, to mention the pixel count in bin 1 that
    # has been excluded.
    hitsinbin1 = n[1]
    hppplotax.set_title("\emph{Note}: %d pixels in bin $N_{h} = 1$" % (hitsinbin1))

    #
    # Save the figure.
    hppplot.savefig(hitfreqplotfilepath)

    # Write the summary HTML file.

    hf = open(htmlfilename, "w")

    hf.write("<!DOCTYPE html>\n")
    hf.write("<html>\n")
    hf.write("  <body>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % (maskmapfilename))
    hf.write("      <figcaption><em>Fig. 1: Hit freqency map for</em> ")
    hf.write("<code>%s</code>.</figcaption>\n" % (outputnamebase))
    hf.write("    </figure>\n")
    hf.write("    <figure>\n")
    hf.write("      <img src=\"%s\" />\n" % (hitfreqplotfilename))
    hf.write("      <figcaption><em>Fig. 2: Hit freqency histogram for</em> ")
    hf.write("<code>%s</code>.</figcaption>\n" % (outputnamebase))
    hf.write("    </figure>\n")
    hf.write("  </body>\n")
    hf.write("</html>")

    hf.close()

