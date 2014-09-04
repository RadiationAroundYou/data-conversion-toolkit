#!/usr/bin/env python

#==================================
# Cl2Mf-validation for CERN@school
#==================================
#
# See the README.md for more information.

# Import the code needed to manage files.
import os, inspect, glob, argparse

# Import the code needed to parse times.
import datetime

# Import the ROOT libraries.
from ROOT import TFile, TH1D

import rootplot.root2matplotlib as r2m

## Import the plotting libraries.
import numpy as np
import pylab as plt
import matplotlib
from matplotlib import rc

# Make our plots compatible with CERN@school LaTeX reports.
rc('font',**{'family':'serif','serif':['Computer Modern']})
rc('text', usetex=True)

# Get the path of the current directory
path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

#
# The main program.
#
if __name__=="__main__":
    print("*==================================================")
    print("   CERN@school Calibration -> Mafalda validation   ")
    print("*==================================================")

    # Get the validation file path from the command line.
    parser = argparse.ArgumentParser()
    parser.add_argument("valpath", \
        help="Path to the validation ROOT file."  )
    parser.add_argument("outpath", \
        help="Path to the output directory.")
    args = parser.parse_args()

    # Set the validation file path.
    valpath = args.valpath

    # Set the output path.
    outpath = args.outpath

    print("*")
    print("* valpath is '%s'" % valpath)
    print("* outpath us '%s'" % outpath)
    print("*")

    # Get the validation ROOT file.
    f = TFile(valpath, "READ")

    print("*======================================================")
    print("* TEST 01: Number of lines in the cluster log file     ")
    print("*======================================================")

    # Get the number of lines histogram.
    hg_nLines_ex = f.Get("nLines_ex")
    nLines_ex = hg_nLines_ex.Integral()

    hg_nHeaders_ex = f.Get("nHeaders_ex")
    nHeaders_ex = hg_nHeaders_ex.Integral()

    hg_nClusters_ex = f.Get("nClusters_ex")
    nClusters_ex = hg_nClusters_ex.Integral()

    hg_nBlanks_ex = f.Get("nBlanks_ex")
    nBlanks_ex = hg_nBlanks_ex.Integral()

    calcLines = nHeaders_ex + nClusters_ex + nBlanks_ex

    print("* Number of lines extracted            = %10d" % (nLines_ex))
    print("*")
    print("* Number of frame headers extracted    = %10d" % (nHeaders_ex) )
    print("* Number of cluster lines extracted    = %10d" % (nClusters_ex))
    print("* Number of blank lines extracted      = %10d" % (nBlanks_ex)  )
    print("*")
    print("* Validation check: total lines        = %10d" % (calcLines)   )
    print("*======================================================")
    if calcLines==nLines_ex:
        print("* TEST PASSED - total number of lines match.")
    else:
        print("* TEST FAILED - mismatch in the number of lines.")
    print("*======================================================")

    print("*")
    print("*")

    print("*======================================================")
    print("* TEST 02: Number of pixels in the cluster log file    ")
    print("*======================================================")

    hg_nPixelsPf_ex = f.Get("nPixelsPf_ex")
    nPixels_ex = 0
    for i in range(hg_nPixelsPf_ex.GetNbinsX()+1):
        nb_i = hg_nPixelsPf_ex.GetBinLowEdge(i)
        n_i = hg_nPixelsPf_ex.GetBinContent(i)
        nPixels_ex += nb_i * n_i

    # Count the total number of pixels, as well as the
    # minimum and maximum filled bins.
    hg_nPixelsPf_fc = f.Get("nPixelsPf_fc")
    nPixels_fc = 0
    min_x_p = 65536; max_x_p = 0
    n_p0 = 0
    for i in range(hg_nPixelsPf_fc.GetNbinsX()+1):
        x_i = hg_nPixelsPf_fc.GetBinLowEdge(i)
        n_i = hg_nPixelsPf_fc.GetBinContent(i)
        if n_i > 0:
            if x_i > max_x_p: max_x_p = x_i
            if x_i < min_x_p and x_i != 0: min_x_p = x_i
            if x_i == 0: n_p0 += 1
        nPixels_fc += x_i * n_i
    min_x_p = max(min_x_p - 5,    0)
    max_x_p = min(max_x_p + 5,65536)

    # Fig. 1: Pixels per frame frequency histogram
    ppfplot = plt.figure(101, figsize=(5.0, 3.0), dpi=150, facecolor='w', edgecolor='w')
    ppfplot.subplots_adjust(bottom=0.17, left=0.15)
    ppfplotax = ppfplot.add_subplot(111)
    # y axis
    plt.ylabel('Number of frames')
    # x axis
    plt.xlabel('$N_p$')
    #
    hist0 = r2m.Hist(hg_nPixelsPf_fc)
    hist0.hist(facecolor='b', edgecolor='none')
    plt.xlim(min_x_p,max_x_p+1)
    ppfplotax.grid(1)
    #ppfplot.show()
    ppfplot.savefig(outpath+'/nPixelsPf.pdf')

    print("* Number of pixels extracted    = %10d" % (nPixels_ex))
    print("* Number of pixels counted      = %10d" % (nPixels_fc))
    print("* Number of blank frames        = %10d" % (n_p0)      )
    print("*======================================================")
    if nPixels_ex==nPixels_fc:
        print("* TEST PASSED - total number of pixels match.")
    else:
        print("* TEST FAILED - mismatch in the number of pixels.")
    print("*======================================================")

    print("*")
    print("*")

    print("*======================================================")
    print("* TEST 03: Number of clusters in the cluster log file  ")
    print("*======================================================")

    # Get the clusters per frame (extracted) histogram.
    hg_nClustersPf_ex = f.Get("nClustersPf_ex")

    # Get the total number of clusters from the histogram.
    nClusters_ex = 0
    for i in range(hg_nClustersPf_ex.GetNbinsX()+1):
        nb_i = hg_nClustersPf_ex.GetBinLowEdge(i)
        n_i = hg_nClustersPf_ex.GetBinContent(i)
        nClusters_ex += nb_i * n_i

    # Get the clusters per frame (BlobFinder) histogram.
    hg_nClustersPf_bf = f.Get("nClustersPf_bf")

    # Get the total number of clusters from the histogram,
    # as well as the minimum and maximum filled bins.
    nClusters_bf = 0;
    min_x = 16384; max_x = 0
    n_c0 = 0
    for i in range(hg_nClustersPf_bf.GetNbinsX()+1):
        x_i = hg_nClustersPf_bf.GetBinLowEdge(i)
        n_i = hg_nClustersPf_bf.GetBinContent(i)
        if n_i > 0:
            if x_i > max_x: max_x = x_i
            if x_i < min_x and x_i != 0: min_x = x_i
            if x_i == 0: n_c0 += 1
        nClusters_bf += x_i * n_i
    min_x = max(min_x - 5,    0)
    max_x = min(max_x + 5,16384)

    # Fig. 2: Clusters per frame frequency histogram
    cpfplot = plt.figure(102, figsize=(5.0, 3.0), dpi=150, facecolor='w', edgecolor='w')
    cpfplot.subplots_adjust(bottom=0.17, left=0.15)
    cpfplotax = cpfplot.add_subplot(111)
    # y axis
    plt.ylabel('Number of frames')
    # x axis
    plt.xlabel('$N_c$')
    #
    hist1 = r2m.Hist(hg_nClustersPf_bf)
    hist1.hist(facecolor='r', edgecolor='none')
    plt.xlim(min_x,max_x+1)
    cpfplotax.grid(1)
    #cpfplot.show()
    cpfplot.savefig(outpath+'/nClustersPf.pdf')

    print("* Number of clusters extracted    = %10d" % (nClusters_ex))
    print("* Number of clusters counted      = %10d" % (nClusters_bf))
    print("* Number of blank frames          = %10d" % (n_c0)      )
    print("*======================================================")
    if nClusters_ex==nClusters_bf:
        print("* TEST PASSED - total number of clusters match.")
    else:
        print("* TEST FAILED - mismatch in the number of clusters.")
    print("*======================================================")

    print("*")
    print("*")

    print("*======================================================")
    print("* TEST 04: No. of pixels from the pixels per cluster   ")
    print("*======================================================")
    # Get the pixels per cluster (extracted) histogram.
    hg_ppc_ex = f.Get("ppc_ex")

    # Get the total number of clusters from the histogram.
    nPixels_ppc_ex = 0
    min_x = 11810; max_x = 0
    n_1 = hg_ppc_ex.GetBinContent(2)
    n_2 = hg_ppc_ex.GetBinContent(3)
    n_3 = hg_ppc_ex.GetBinContent(4)
    n_4 = hg_ppc_ex.GetBinContent(5)
    n_small = n_1
    n_large = 0
    for i in range(hg_ppc_ex.GetNbinsX()+1):
        x_i = hg_ppc_ex.GetBinLowEdge(i)
        n_i = hg_ppc_ex.GetBinContent(i)
        if n_i > 0:
            if x_i > max_x: max_x = x_i
            if x_i < min_x: min_x = x_i
        if x_i > 1:
            n_large += n_i
        nPixels_ppc_ex += x_i * n_i

    min_x = max(min_x - 5,    0)
    max_x = min(max_x + 5,11810)

    # Fig. 3: Pixels per cluster frequency histogram
    ppcplot = plt.figure(103, figsize=(5.0, 3.0), dpi=150, facecolor='w', edgecolor='w')
    ppcplot.subplots_adjust(bottom=0.17, left=0.15)
    ppcplotax = ppcplot.add_subplot(111)
    # y axis
    plt.ylabel('Number of clusters')
    # x axis
    plt.xlabel('Pixels per cluster')
    #
    hist2 = r2m.Hist(hg_ppc_ex)
    hist2.hist(facecolor='g', edgecolor='none')
    plt.xlim(min_x,max_x+1)
    ppcplotax.grid(1)
    #ppcplot.show()
    ppcplot.savefig(outpath+'/ppf.pdf')
    print("* Number of pixels extracted    = %10d" % (nPixels_ex)    )
    print("* Number of pixels counted      = %10d" % (nPixels_ppc_ex))
    print("*======================================================")
    if nPixels_ex==nPixels_ppc_ex:
        print("* TEST PASSED - total number of pixels match.")
    else:
        print("* TEST FAILED - mismatch in the number of pixels.")
    print("*======================================================")

    print("*")
    print("*")

    print("*======================================================")
    print("* TEST 05: No. of clusters from the pixels per cluster ")
    print("*======================================================")
    # Get the number of clusters (extracted) histogram.
    print("* Number of clusters extracted    = %10d" % (nClusters_ex))
    print("* Number of clusters (ppf)        = %10d" % (hg_ppc_ex.Integral()))
    print("*------------------------------------------------------")
    print("* Number of monopixel clusters    = %10d" % (n_1))
    print("* Number of bipixel clusters      = %10d" % (n_2))
    print("* Number of tripixel clusters     = %10d" % (n_3))
    print("* Number of tetrapixel clusters   = %10d" % (n_4))
    print("*------------------------------------------------------")
    print("* Number of 'OK' clusters    (=1) = %10d" % (n_small))
    print("* Number of 'Large' clusters (>1) = %10d" % (n_large))
    print("* 'OK' + 'Large'                  = %10d" % (n_small+n_large))
    print("*======================================================")
    if nClusters_ex==hg_ppc_ex.Integral():
        print("* TEST PASSED - total number of clusters match.")
    else:
        print("* TEST FAILED - mismatch in the number of clusters.")
    print("*======================================================")

    print("*")
    print("*")

    # Time information
    #------------------
    phg_time = f.Get("times")
    starttime = phg_time.GetBinContent(1)
    starttimes = datetime.datetime.fromtimestamp(starttime).strftime('%a %d %b %Y, %H:%M:%S')
    starttimefrac = ("%.6f" % (starttime-np.floor(starttime))).lstrip('0')
    endtime   = phg_time.GetBinContent(2)
    endtimes = datetime.datetime.fromtimestamp(endtime).strftime('%a %d %b %Y, %H:%M:%S')
    endtimefrac = ("%.6f" % (endtime-np.floor(endtime))).lstrip('0')
    time      = endtime - starttime
    print("*======================================================")
    print("* INFO: Start time = %s%s" % (starttimes, starttimefrac))
    print("* INFO: End time   = %s%s" % (endtimes, endtimefrac))
    print("* INFO: Total time = %10f [seconds]" % (time))
    print("* INFO: Total time = %10.2f [minutes]" % (time/60.))
