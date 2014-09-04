#!/usr/bin/env python

"""@package datastructs

==============================================================================
                     CERN@school data structures package
==============================================================================

See the data-conversion-toolkit README.md for further instructions.

"""

# Import the required ROOT libraries - hurrah for PyROOT!
from ROOT import TFile, gSystem

# Import the validator package.
from mfvalidator import *

class Data:
    """ Container class for the LUCID data files (ROOT format).

    """
    def __init__(self, path):
        """ Constructor. """

        ## The path to the data file.
        self.path = path

        ## The ROOT file itself.
        self.file = TFile(self.path)

        ## The TTree containing the data.
        self.chain = self.file.Get('MPXTree')

        ## The number of frames in the file.
        self.nframes = self.chain.GetEntriesFast()

    def getPixels(self, fn):
        """ Extracts the pixels from the ROOT file. """

        # Load the TTree.
        ientry = self.chain.LoadTree(fn)

        # FIXME Maybe handle any "badness" here...

        # Copy the entry into memory.
        nb = self.chain.GetEntry(fn)

        # FramesData is a C++ class stored in the ROOT file. We use
        # a skeleton implementation (found in Frame.C, built with the
        # setup.sh script) as an interface to the data.
        dataXCmap = self.chain.FramesData.GetFrameXC()

        ## A dictionary for the pixel information {X:C}.
        pxs = {}

        maskedPixels = self.chain.FramesData.GetLVL1()
        mask = {}
        for i in maskedPixels:
             if i.second == 1:
                 mask[i.first] = i.second
                 pxs[i.first] = -1
        #print masked

        #print("* DEBUG: found %d masked pixels!" % (maskedPixels.size()))

        ## The maximum pixel counts found in the frame.
        cmax = 1

        # Loop over the pixel data and extract the pixel information.
        for i in dataXCmap:
            #print("* (%d) = %d" % (i.first, i.second))

            # It's a C++ std::map<int,int> - suprisingly easy to access!
            if i.first not in mask.keys():
                pxs[i.first] = i.second

                # Do we have a new maximum count value?
                if i.second > cmax:
                    cmax = i.second
            else:
                # This is a bit hacky, but masked pixels could be stored
                # as -1 for now, and plotted according to that...
                pxs[i.first] = -1

        # Return the pixels and the maximum pixel count.
        return pxs, cmax

    # Metadata

    def getMetadata(self, fn):
        """ Extracts the metadata from the file."""

        # Load the TTree.
        ientry = self.chain.LoadTree(fn)

        # FIXME Maybe handle any "badness" here...

        # Copy the entry into memory.
        nb = self.chain.GetEntry(fn)

        # Create the metadata wrapper/validator instance.
        md = MfValidator(fn) #, True) # Set to true for debugging...

        # Get the hit pixel map.
        dataXC = self.chain.FramesData.GetFrameXC()
        dataXCmap = {}
        for XC in dataXC:
            dataXCmap[XC.first] = XC.second

        # Get the pixel mask map.
        mask = self.chain.FramesData.GetLVL1()
        maskmap = {}
        for m in mask:
            maskmap[m.first] = m.second

        # Set the hits and counts from the hit pixel map and the masked pixels.
        md.setHitsAndCountsFromPixelsAndMask(dataXCmap, maskmap)

        #md.setNumCountsFromPixelMap(len(dataXCmap))

        # Get the number of pixels hit from the metadata.
        md.setNumHitsFromMetadata(self.chain.FramesData.GetEntriesPad())

        #tc = 0
        #for i in dataXCmap:
        #    # It's a C++ std::map<int,int> - suprisingly easy to access!
        #    tc += i.second
        #md.setTotalChargeFromPixelMap(tc)

        ## Get the total charge from the metadata.
        md.setTotalCountsFromMetadata(self.chain.FramesData.GetChargeInPad())

        # Get the data type from the metadata.
        md.setIsMCData(self.chain.FramesData.IsMCData())

        # Payload information
        #---------------------

        # Get the dataset ID
        md.setDatasetID(str(self.chain.FramesData.GetDataSet()))

        # Get the frame ID
        md.setFrameID(str(self.chain.FramesData.GetFrameId()))

        # Get the frame dimensions.
        md.setFrameWidth( self.chain.FramesData.GetFrameWidth() )
        md.setFrameHeight(self.chain.FramesData.GetFrameHeight())

        # Get the original frame format.
        md.setFormat(self.chain.FramesData.GetPayloadFormat())

        # Get the occupancy.
        md.setOccupancy(self.chain.FramesData.GetOccupancy())
        md.setOccupancyPc(self.chain.FramesData.GetOccupancyPc())

        # Acquisition information
        #-------------------------
        md.setAcqMode(self.chain.FramesData.GetAcqMode())
        md.setPixelmanVer(self.chain.FramesData.GetPixelmanVersion())

        # Geospatial information
        #------------------------
        md.setLatitude( self.chain.FramesData.GetLatitude() )
        md.setLongitude(self.chain.FramesData.GetLongitude())
        #
        md.setAltitude(self.chain.FramesData.GetAltitude())
        #
        md.setRoll(self.chain.FramesData.GetRoll())
        md.setPitch(self.chain.FramesData.GetPitch())
        md.setYaw(self.chain.FramesData.GetYaw())
        md.setOmegaX(self.chain.FramesData.GetOmega_x())
        md.setOmegaY(self.chain.FramesData.GetOmega_y())
        md.setOmegaZ(self.chain.FramesData.GetOmega_z())

        # Temporal information
        #----------------------
        md.setStartTime(self.chain.FramesData.GetStartTime())
        md.setStartTimeStr(self.chain.FramesData.GetStartTimeS())
        md.setAcqTime(self.chain.FramesData.GetAcqTime())

        # Detector information
        #----------------------
        md.setChipboardID(str(self.chain.FramesData.GetChipboardID()))
        md.setCustomName(str(self.chain.FramesData.GetCustomName()))
        md.setInterfaceType(str(self.chain.FramesData.GetInterface()))
        md.setMpxType(self.chain.FramesData.GetMpxType())
        md.setAppFilters(self.chain.FramesData.GetAppFilters())
        md.setDetX(self.chain.FramesData.GetDet_x())
        md.setDetY(self.chain.FramesData.GetDet_y())
        md.setDetZ(self.chain.FramesData.GetDet_z())
        md.setEulerA(self.chain.FramesData.GetEulerA())
        md.setEulerB(self.chain.FramesData.GetEulerB())
        md.setEulerC(self.chain.FramesData.GetEulerC())

        # Detector settings
        #-------------------
        md.setPolarity(self.chain.FramesData.GetPolarity())
        md.setBiasVoltage(self.chain.FramesData.GetHV())
        md.setDACs(self.chain.FramesData.GetDACs())
        md.setMpxClock(self.chain.FramesData.GetMpxClock())
        md.setTpxClock(self.chain.FramesData.GetTpxClock())
        md.setBSPreamp(self.chain.FramesData.GetBsActive())

        # Source information
        #--------------------
        md.setSourceID(str(self.chain.FramesData.GetSourceId()))
        md.setFirmware(str(self.chain.FramesData.GetFirmware()))

        # Return the metadata.
        return md
