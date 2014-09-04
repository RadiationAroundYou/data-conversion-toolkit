#!/usr/bin/env python

"""@package mfvalidator

==============================================================================
                  CERN@school Mafalda format validation code
==============================================================================

See the data-conversion-toolkit README.md for further instructions.

"""

# Import the data structures package.
from datastructs import *

# Import NumPy.
import numpy as np

# Import the regex module.
import re

# Import time.
import time

class MfValidator:
    """Class for validating Mafalda-format Timepix data.
    """

    ## Constructor
    # @param [in,out] self The object pointer.
    # @param [in] fn The frame number.
    # @param [in] dbg Run in debug mode?
    def __init__(self, fn, dbg = False):

        ## The frame number.
        self.fn = fn

        ## Run in debug mode?
        self.dbg = dbg

        # regex match for the chipboard ID
        self.r = re.compile(r'[A-Z]\d{2,2}-[A-Z]\d{4,4}')

        # regex match for the start time string
        #Wed Apr 02 15:03:15.838143 2014
        self.r_starttime = re.compile(r'[A-Z][a-z][a-z] [A-Z][a-z][a-z] \d{2,2} \d{2,2}:\d{2,2}:\d{2,2}.\d{6,6} \d{4,4}')

        if self.dbg:
            print("*   MfValidator   ")
            print("*=================")
            print("* MfValidator frame number: %5d" % (self.fn))

        self.numhitsfrompixelmap = -1

    def setHitsAndCountsFromPixelsAndMask(self, pixelsmap, maskmap):

        ## The total number of hits, including those from masked pixels.
        self.numhitsfrompixelmap   = len(pixelsmap)

        ## The number of masked pixels.
        self.nummaskedpixels = len(maskmap)

        ## The number of hit pixels that have been filtered.
        self.numfilteredhits   = 0

        ## The number of hit pixels that have not been filtered.
        self.numunfilteredhits = 0

        ## The total charge from the pixel map.
        self.totalcountsfrompixelmap = 0

        ## The total charge from unfiltered pixels.
        self.totalcountsfromunfiltered = 0

        ## The total charge from filtered pixels.
        self.totalcountsfromfiltered = 0

        ## Loop over the pixel map.
        for key, val in pixelsmap.iteritems():
            self.totalcountsfrompixelmap += val
            if key in maskmap.keys():
                self.numfilteredhits += 1
                self.totalcountsfromfiltered += val
            else:
                self.numunfilteredhits += 1
                self.totalcountsfromunfiltered += val

        self.totalcountsfrommetadata = 0

#    def setNumCountsFromPixelMap(self, numcounts):
#        self.numhitsfrompixelmap = numcounts
#        if self.dbg:
#            print("* Number of counts from the pixel map         : %5d" % (self.numhitsfrompixelmap))

    def setNumHitsFromMetadata(self, numhits):
        self.numhitsfrommetadata = numhits
        if self.dbg:
            print("* Number of hits     from the metadata          : %5d" % (self.numhitsfrommetadata))

    def isNumCountsOK(self):
        mapmetamatch = self.numhitsfrompixelmap == self.numhitsfrommetadata
        #result = result and self.numhitsfrompixelmap == self.occupancy
        #result = result and self.numhitsfrommetadata == self.occupancy
        #return result
        return mapmetamatch

    def isNumMaskedPixelsOK(self):
        nummaskedval = self.nummaskedpixels >= 0 and self.nummaskedpixels <= 65536
        return nummaskedval

    def getNumHits(self):

        if self.isNumCountsOK():
            c = "green"
        else:
            c = "red"
        if self.isNumMaskedPixelsOK():
            cmask = "green"
        else:
            cmask = "red"
        s  = ""
        s += "<font color='%s'>%d</font>" % (cmask, self.numunfilteredhits)
        s += " (= "
        s += "<font color='%s'>%d</font>" % (c, self.numhitsfrompixelmap)
        s += " - "
        s += "<font color='%s'>%d</font>" % (cmask, self.numfilteredhits)
        s += " filtered; "
        s += "<font color='%s'>%d</font>" % (cmask, self.nummaskedpixels)
        s += " masked)"
        return s

#    def setTotalCountsFromPixelMap(self, tc):
#        self.totalcountsfrompixelmap = tc
#        if self.dbg:
#            print("* Total counts from the pixel map             : %15d" % (self.totalcountsfrompixelmap))

    def setTotalCountsFromMetadata(self, tc):
        self.totalcountsfrommetadata = tc
        if self.dbg:
            print("* Total counts from the metadata              : %15d" % (self.totalcountsfrommetadata))

    def getTotalCounts(self):
        #if self.totalcountsfrompixelmap == self.totalcountsfrommetadata:
        #    s = "<font color='green'>%d</font>" % (self.totalcountsfrompixelmap)
        #else:
        #    s = "<font color='red'>%d (%d)</font>" % (self.totalcountsfrompixelmap, self.totalcountsfrommetadata)
        c = "green"
        s = ""
        s += "<font color='%s'>%d</font>" % (c, self.totalcountsfromunfiltered)
        s += " (= "
        s += "<font color='%s'>%d</font>" % (c, self.totalcountsfrompixelmap)
        s += " - "
        s += "<font color='%s'>%d</font>" % (c, self.totalcountsfromfiltered)
        s += " filtered)"
        return s

    def setIsMCData(self, ismc):
        self.ismcdata = ismc
        if self.dbg:
            if self.ismcdata:
                print("* This is Monte Carlo data.")
            else:
                print("* This is real data.")

    def getDataType(self):
        s = ""
        if self.ismcdata:
            s += "Monte Carlo (simulated) data"
        else:
            s += "Real data"
        return s


    # Payload information
    #---------------------

    def getDatasetID(self):
        s = self.datasetid
        return s

    def setDatasetID(self, datasetid):
        self.datasetid = datasetid
        if self.dbg:
            print("* The dataset ID is '%s'." % (self.datasetid))

    def getFrameID(self):
        s = self.frameid
        return s

    def setFrameID(self, frameid):
        self.frameid = frameid
        if self.dbg:
            print("* The frame ID is '%s'." % (self.frameid))

    # Frame size
    def frameSizeOK(self):
        return self.framewidth == 256 and self.frameheight == 256

    def getFrameSize(self):
        if self.frameSizeOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%3d</font> [px] x <font color='%s'>%3d</font> [px]" % (c, self.framewidth, c, self.frameheight)
        return s
    def setFrameWidth(self, w):
        self.framewidth = w
    def setFrameHeight(self, h):
        self.frameheight = h

    def formatOK(self):
        # TODO other format codes...
        return self.formatcode in [18,8210,4114,17,8209,4113,16384]

    def getFormat(self):
        if self.formatOK():
            c = "green"
        else:
            c = "red"

        # Decode the format code into a string...
        s = "<font color = '%s'>" % (c)
        if self.formatcode == 4114:
            s += "ASCII [X, Y, C]"
        elif self.formatcode == 8210:
            s += "ASCII [X, C]"
        elif self.formatcode == 18:
            s += "ASCII matrix"
        elif self.formatcode == 17:
            s += "Binary matrix"
        elif self.formatcode == 8209:
            s += "Binary [X, C]"
        elif self.formatcode == 4113:
            s += "Binary [X, Y, C]"
        elif self.formatcode == 16384:
            s += "(Not from Pixelman)"
        else:
            s += "Unrecognised format"
        s += "</font>"
        return s

    def setFormat(self, formatcode):
        self.formatcode = formatcode

        if self.dbg:
            print("* The frame format code is %d" % (self.formatcode))

    def occupancyOK(self):
        # Check if the occupancy figures match.
        occFromPc = int(self.occupancyPc * self.framewidth * self.frameheight / 100.0)
        return self.occupancy == occFromPc

    def setOccupancy(self, occ):
        self.occupancy = occ
        if self.dbg:
            print("* The occupancy is %5d" % (self.occupancy))

    def setOccupancyPc(self, occPc):
        self.occupancyPc = occPc
        if self.dbg:
            print("* The occupancy (%%) is %f" % (self.occupancyPc))

    def getOccupancy(self):
        if self.isNumCountsOK() and self.occupancyOK():
            c = "green"
        else:
            c = "red"
        s = "<font color = '%s'>%5d (%8.4f %%)</font>" % (c, self.occupancy, self.occupancyPc)
        return s

    # Acquisition information
    #-------------------------
    #
    # Acqusition mode.
    def isAcqModeOK(self):
        isint   = isinstance(self.acqmode, int)
        isvalid = self.acqmode in [1,2,3,4]
        return isint and isvalid
    def setAcqMode(self, acqmode):
        self.acqmode = acqmode
        if   self.acqmode == 1:
            self.acqmodestr = "Manual start, timer stop"
        elif self.acqmode == 2:
            self.acqmodestr = "Manual start, SW stop"
        elif self.acqmode == 3:
            self.acqmodestr = "SW start, time stop"
        elif self.acqmode == 4:
            self.acqmodestr = "SW start, SW stop"
        else:
            self.acqmodestr = "UNKNOWN ACQ. MODE"
        if self.dbg:
            print("* Acquisition mode is %s (%d)" % (self.acqmodestr, self.acqmode))
    def getAcqMode(self):
        s = ""
        if self.isAcqModeOK():
            s += "<font color='green'>%s</font>" % (self.acqmodestr)
        else:
            s += "<font color='red'>%s</font>" % (str(self.acqmode))
        return s
    #
    # Pixelman version
    def isPixelmanVerOK(self):
        # TODO What do we need to check for this?
        return self.pixelmanver != ""
    def setPixelmanVer(self, ver):
        self.pixelmanver = str(ver).rstrip()
        if self.dbg:
            print("* Pixelman version is: '%s'" % (self.pixelmanver))
    def getPixelmanVer(self):
        s = ""
        if self.isPixelmanVerOK():
            s += "<font color='green'>%s</font>" % (self.pixelmanver)
        else:
            s += "<font color='red'>INVALID PIXELMAN VERSION</font>"
        return s
    #
    # Geospatial information
    #------------------------
    #
    def locationOK(self):
        result = self.latitude  >= -090.0 and self.latitude  <=  90.0 \
             and self.longitude >= -180.0 and self.longitude <= 180.0
        return result

    def setLatitude(self, lat):
        self.latitude = lat
        if self.dbg:
            print("* The latitude is %f degrees" % (self.latitude))

    def setLongitude(self, lon):
        self.longitude = lon
        if self.dbg:
            print("* The longitude is %f degrees" % (self.longitude))

    def getLocation(self):
        if self.locationOK():
            c = "green"
        else:
            c = "red"
        s = u"(<font color='%s'>%9.6f</font>\u00b0, <font color='%s'>%9.6f</font>\u00b0)" % (c, self.latitude, c, self.longitude)
        return s

    def setAltitude(self, alt):
        self.altitude = alt
        if self.dbg:
            print("* The altitude is %f [m]" % (self.altitude))
    #
    def getAltitude(self):
        if True:
            c = "green"
        s = "<font color='%s'>%9.4f</font> [m]" % (c, self.altitude)
        return s
    #
    # Lab rotation
    def isLabRotationOK(self):
        rollok  = self.roll  > -180.0 and self.roll  <  180.0
        pitchok = self.pitch > -180.0 and self.pitch <  180.0
        yawok   = self.yaw   > -180.0 and self.yaw   <  180.0
        return rollok and pitchok and yawok
    #
    def setRoll(self, roll):
        self.roll = roll
        if self.dbg:
            print("* Roll is %6.2f [deg.]" % (self.roll))
    def setPitch(self, pitch):
        self.pitch = pitch
        if self.dbg:
            print("* Pitch is %6.2f [deg.]" % (self.pitch))
    def setYaw(self, yaw):
        self.yaw = yaw
        if self.dbg:
            print("* Yaw is %6.2f [deg.]" % (self.yaw))
    #
    def getLabRotation(self):
        if self.isLabRotationOK():
            c = "green"
        else:
            c = "red"
        s  = u"("
        s += u"<font color='%s'>%6.2f</font>, " % (c, self.roll)
        s += u"<font color='%s'>%6.2f</font>, " % (c, self.pitch)
        s += u"<font color='%s'>%6.2f</font>"   % (c, self.yaw)
        s += u") [\u00b0]"
        return s

    def isLabOmegaOK(self):
        omegaxok = self.omegax > -180.0 and self.omegax <  180.0
        omegayok = self.omegay > -180.0 and self.omegay <  180.0
        omegazok = self.omegaz > -180.0 and self.omegaz <  180.0
        return omegaxok and omegayok and omegazok
    def setOmegaX(self, omegax):
        self.omegax = omegax
        if self.dbg:
            print("* Lab Omega_x = %6.2f" % (self.omegax))
    def setOmegaY(self, omegay):
        self.omegay = omegay
        if self.dbg:
            print("* Lab Omega_y = %6.2f" % (self.omegay))
    def setOmegaZ(self, omegaz):
        self.omegaz = omegaz
        if self.dbg:
            print("* Lab Omega_z = %6.2f" % (self.omegaz))
    def getLabOmega(self):
        if self.isLabOmegaOK():
            c = "green"
        else:
            c = "red"
        s  = u"("
        s += u"<font color='%s'>%6.2f</font>, " % (c, self.omegax)
        s += u"<font color='%s'>%6.2f</font>, " % (c, self.omegay)
        s += u"<font color='%s'>%6.2f</font>"   % (c, self.omegaz)
        s += u") [\u00b0 s<sup>-1</sup>]"
        return s

    # Temporal information
    #----------------------

    def isStartTimeOK(self):
        floatok  = isinstance(self.starttime, float)
        stringok = self.r_starttime.match(self.starttimestr) is not None
        # There is a rounding error in Pixelman, so only go to the fifth
        # decimal place.
        #matchok  = ("%.5f" % (self.starttimecheck))[:-2] == ("%.5f" % (self.starttime))[:-2]
        return floatok and stringok #and matchok
    def setStartTime(self, time):
        self.starttime = time
        if self.dbg:
            print("* Start time is %f [s]" % (self.starttime))
    def getStartTime(self):
        if self.isStartTimeOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%f</font> [s]" % (c, self.starttime)
        return s
    #
    def setStartTimeStr(self, timestr):
        self.starttimestr = str(timestr).rstrip()

        year   = self.starttimestr[-4:]
        month  = self.starttimestr[4:7]
        day    = self.starttimestr[8:10]
        mytime = self.starttimestr[11:19]
        subsec = float(self.starttimestr[-12:-5])
        cvtsstr = mytime + " " + day + " " + month + " " + year
        cvts = time.strptime(cvtsstr, "%H:%M:%S %d %b %Y")
        self.starttimecheck = time.mktime(cvts) + subsec

        if self.dbg:
            print("* Start time (str) = '%s'" % (self.starttimestr))
            print("* Time from Pixelman: '%s'" % (self.starttimestr))
            print("* Year extracted : '%s'" % (year))
            print("* Month extracted: '%s'" % (month))
            print("* Date extracted: '%s'" % (day))
            print("* Time extracted: '%s'" % (mytime))
            print("* Sub-second extracted: %f" % (subsec))
            print("* Converted time string '%s'" % (cvtsstr))
            print("* Converted time: %s" % (time.asctime(cvts)))
            print("* Converted time: %f" % (self.starttimecheck))

    def getStartTimeStr(self):
        if self.isStartTimeOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%s</font>" % (c, self.starttimestr)
        return s
    def getEndTimeStr(self):
        if self.isStartTimeOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%s</font>" % (c, self.endtimestr)
        return s
    #
    def isAcqTimeOK(self):
        return isinstance(self.acqtime, float)

    def setAcqTime(self, acqtime):

        ## The acquisition time.
        self.acqtime = acqtime

        ## The end time.
        self.endtime = self.starttimecheck + self.acqtime

        lf, rt = time.strftime("%a %b %d %H:%M:%S. %Y", time.localtime(self.endtime)).split('.')

        self.endtimestr = lf + ("%7.6f" % (self.endtime))[-7:] + rt

        #Wed Apr 02 15:03:15.838143 2014

        if self.dbg:
            print("* Acquisition time                  : %f [s]" % self.acqtime)
            print("* End time                          : %f [s]" % self.endtime)
            print("* End time (string)                 : '%s'"   % self.endtimestr)
    def getAcqTime(self):
        if self.isAcqTimeOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%.6f</font> [s]" % (c, self.acqtime)
        return s

    # Detector information
    #----------------------

    # Chipboard ID
    #
    def chipboardIDisOK(self):
        if self.r.match(self.chipid) is not None:
            return True
        else:
            return False
    #
    def setChipboardID(self, chipid):
        self.chipid = str(chipid).rstrip()
        if self.dbg:
            print("* The chipboard ID is               : '%s'" % (self.chipid))
    #
    def getChipboardID(self):
        if self.chipboardIDisOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%s</font>" % (c, self.chipid)
        return s

    # Custom name
    def isCustomNameOK(self):
        # The detector should have a custom name
        return self.customname != ""

    def setCustomName(self, name):
        self.customname = name
        if self.dbg:
            print("* The detector's custom name is     : '%s'" % (self.customname))

    def getCustomName(self):
        if self.isCustomNameOK():
            s = "<font color='green'>%s</font>" % (self.customname)
        else:
            s = "<font color='red'>INVALID NAME!</font>"
        return s

    # Firmware
    def isFirmwareOK(self):
        return self.firmware != ""
    def setFirmware(self, fw):
        self.firmware = str(fw).rstrip()
        if self.dbg:
            print("* Firmware                          : '%s'" % (self.firmware))
    def getFirmware(self):
        s = ""
        if self.isFirmwareOK():
            s += "<font color='green'>%s</font>" % (self.firmware)
        else:
            s += "<font color='red'>INVALID FIRMWARE</font>"
        return s

    # Interface type
    def isInterfaceTypeOK(self):
        return self.interface != ""
    def setInterfaceType(self, ift):
        self.interface = str(ift).rstrip()
        if self.dbg:
            print("* Interface type                    : '%s'" % (self.interface))
    def getInterfaceType(self):
        s = ""
        if self.isInterfaceTypeOK():
            s += "<font color='green'>%s</font>" % (self.interface)
        else:
            s += "<font color='red'>INVALID INTERFACE TYPE</font>"
        return s

    # Medipix type
    def isMpxTypeOK(self):
        return self.mpxtype in [1,2,3]
    def setMpxType(self, mpxtype):
        self.mpxtype = mpxtype
        if self.dbg:
            print("* Medipix type is                   : %d" % (self.mpxtype))
    def getMpxType(self):
        if self.isMpxTypeOK(): c = "green"
        else: c = "red"
        s = "<font color='%s'>" % (c)
        if self.mpxtype == 1:
            s += "Medipix 2.1"
        elif self.mpxtype == 2:
            s += "Medipix MXR"
        elif self.mpxtype == 3:
            s += "Timepix"
        else:
            s += "INVALID MEDIPIX TYPE"
        s += "</font>"
        return s

    # Applied filters
    def isAppFiltersOK(self):
        # TODO: check for the existence of the applied filter(s)...
        return True
    def setAppFilters(self, appf):
        self.appfilters = appf
        if self.dbg:
            print("* Applied filters                   : '%s'" % (self.appfilters))
    def getAppFilters(self):
        if self.isAppFiltersOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>" % (c)
        if len(self.appfilters) > 0:
            s += str(self.appfilters)
        else:
            s += "None"
        s += "</font>"
        return s
    #
    def isPositionOK(self):
        detxok = isinstance(self.detx, float)
        detyok = isinstance(self.dety, float)
        detzok = isinstance(self.detz, float)
        return detxok and detyok and detzok
    def setDetX(self, detx):
        self.detx = detx
        if self.dbg:
            print("* Detector x                        : %8.2f [mm]" % (self.detx))
    def setDetY(self, dety):
        self.dety = dety
        if self.dbg:
            print("* Detector y                        : %8.2f [mm]" % (self.dety))
    def setDetZ(self, detz):
        self.detz = detz
        if self.dbg:
            print("* Detector z                        : %8.2f [mm]" % (self.detz))
    def getDetPosition(self):
        if self.isPositionOK():
            c = "green"
        else:
            c = "red"
        s  = "("
        s += "<font color='%s'>%.1f</font>, " % (c, self.detx)
        s += "<font color='%s'>%.1f</font>, " % (c, self.dety)
        s += "<font color='%s'>%.1f</font>"   % (c, self.detz)
        s += ") [mm]"
        return s
    #
    def isRotationOK(self):
        euleraok = isinstance(self.eulera, float)
        eulerbok = isinstance(self.eulerb, float)
        eulercok = isinstance(self.eulerc, float)
        return euleraok and eulerbok and eulercok
    def setEulerA(self, eulera):
        self.eulera = eulera
        if self.dbg:
            print("* Detector Euler angle alpha        : %8.2f [deg.]" % (self.eulera))
    def setEulerB(self, eulerb):
        self.eulerb = eulerb
        if self.dbg:
            print("* Detector Euler angle beta         : %8.2f [deg.]" % (self.eulerb))
    def setEulerC(self, eulerc):
        self.eulerc = eulerc
        if self.dbg:
            print("* Detector Euler angle gamma        : %8.2f [deg.]" % (self.eulerc))
    def getDetRotation(self):
        if self.isRotationOK():
            c = "green"
        else:
            c = "red"
        s  = "("
        s += "<font color='%s'>%.1f</font>, " % (c, self.eulera)
        s += "<font color='%s'>%.1f</font>, " % (c, self.eulerb)
        s += "<font color='%s'>%.1f</font>"   % (c, self.eulerc)
        s += u") [\u00b0]"
        return s

    # Detector settings
    #-------------------
    def isPolarityOK(self):
        return self.polarity in [0,1]
    def setPolarity(self, pol):
        self.polarity = pol
        if self.dbg:
            print("* Polarity                          : %d" % (self.polarity))
    def getPolarity(self):
        if self.isPolarityOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>" % (c)
        if   self.polarity == 0:
            s += "Negative"
        elif self.polarity == 1:
            s += "Positive"
        else:
            s += "INVALID POLARITY VALUE"
        s += "</font>"
        return s
    #
    def isBiasVoltageOK(self):
        return isinstance(self.hv, float)
    def setBiasVoltage(self, hv):
        self.hv = hv
        if self.dbg:
            print("* Bias voltage                      : %f [V]" % (self.hv))
    def getBiasVoltage(self):
        if self.isBiasVoltageOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%4.1f</font> [V]" % (c, self.hv)
        return s
    #
    def isIKrumOK(self):
        return self.ikrum > 0 and self.ikrum <= 20
    def isDiscOK(self):
        return True
    def isPreampOK(self):
        return True
    def isBuffAnalogAOK(self):
        return True
    def isBuffAnalogBOK(self):
        return True
    def isHistOK(self):
        return True
    def isTHLOK(self):
        return True
    def isTHLCoarseOK(self):
        return True
    def isVcasOK(self):
        return True
    def isFBKOK(self):
        return True
    def isGNDOK(self):
        return True
    def isTHSOK(self):
        return True
    def isBiasLVDSOK(self):
        return True
    def isRefLVDSOK(self):
        return True
    def setDACs(self, dacs):
        self.dacs = dacs
        #
        self.ikrum       = self.dacs[ 0]
        self.disc        = self.dacs[ 1]
        self.preamp      = self.dacs[ 2]
        self.buffanaloga = self.dacs[ 3]
        self.buffanalogb = self.dacs[ 4]
        self.hist        = self.dacs[ 5]
        self.thl         = self.dacs[ 6]
        self.thlcoarse   = self.dacs[ 7]
        self.vcas        = self.dacs[ 8]
        self.fbk         = self.dacs[ 9]
        self.gnd         = self.dacs[10]
        self.ths         = self.dacs[11]
        self.biaslvds    = self.dacs[12]
        self.reflvds     = self.dacs[13]
        #
        if self.dbg:
            print("* DACs:")
            print("*---> IKrum         = %d" % (self.ikrum))
            print("*---> Disc.         = %d" % (self.disc))
            print("*---> Preamp        = %d" % (self.preamp))
            print("*---> BuffAnalogA   = %d" % (self.buffanaloga))
            print("*---> BuffAnalogB   = %d" % (self.buffanalogb))
            print("*---> Hist          = %d" % (self.hist))
            print("*---> THL           = %d" % (self.thl))
            print("*---> THLCoarse     = %d" % (self.thlcoarse))
            print("*---> Vcas          = %d" % (self.vcas))
            print("*---> FBK           = %d" % (self.fbk))
            print("*---> GND           = %d" % (self.gnd))
            print("*---> THS           = %d" % (self.ths))
            print("*---> BiasLVDS      = %d" % (self.biaslvds))
            print("*---> RefLVDS       = %d" % (self.reflvds))
    #
    def getIKrum(self):
        if self.isIKrumOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.ikrum)
        return s
    def getDisc(self):
        if self.isDiscOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.disc)
        return s
    def getPreamp(self):
        if self.isPreampOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.preamp)
        return s
    def getBuffAnalogA(self):
        if self.isBuffAnalogAOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.buffanaloga)
        return s
    def getBuffAnalogB(self):
        if self.isBuffAnalogBOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.buffanalogb)
        return s
    def getHist(self):
        if self.isHistOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.hist)
        return s
    def getTHL(self):
        if self.isTHLOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.thl)
        return s
    def getTHLCoarse(self):
        if self.isTHLCoarseOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.thlcoarse)
        return s
    def getVcas(self):
        if self.isVcasOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.vcas)
        return s
    def getFBK(self):
        if self.isFBKOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.fbk)
        return s
    def getGND(self):
        if self.isGNDOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.gnd)
        return s
    def getTHS(self):
        if self.isTHSOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.ths)
        return s
    def getBiasLVDS(self):
        if self.isBiasLVDSOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.biaslvds)
        return s
    def getRefLVDS(self):
        if self.isRefLVDSOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%d</font>" % (c, self.reflvds)
        return s
    #
    # Medipix clock
    def isMpxClockOK(self):
        return isinstance(self.mpxclock, float)
    def setMpxClock(self, mpxclock):
        self.mpxclock = mpxclock
        if self.dbg:
            print("* Medipix clock                     : %.2f [MHz]" % (self.mpxclock))
    def getMpxClock(self):
        if self.isMpxClockOK():
            c = "green"
        else:
            c = "red"
        s = "<font color='%s'>%.2f</font> [MHz]" % (c, self.mpxclock)
        return s
    #
    # Timepix clock
    def isTpxClockOK(self):
        #isbyte = isinstance(self.tpxclock, bytes)
        #valid = int(self.tpxclock) in [0,1,2,3]
        #return isbyte and valid
        isfloat = isinstance(self.tpxclock, float)
        valid = self.tpxclock > 0.0
        return isfloat and valid
    #
    def setTpxClock(self, tpxclock):
        self.tpxclock = tpxclock

        ## 0-3 bytes corresponds to 10MHz, 20MHz, 40MHz, 80MHz respectively.
        #self.tpxclockval = 2**(int(tpxclock.encode('hex'), 16)) * 10.0

        if self.dbg:
            #hexval = "0x%02X" % (ord(self.tpxclock))
            #print("* Timepix clock                     : %.2f [MHz] (%s)" % (self.tpxclockval, hexval))
            print("* Timepix clock                     : %.2f [MHz] " % (self.tpxclockval))
    #
    def getTpxClock(self):
        if self.isMpxClockOK():
            c = "green"
        else:
            c = "red"
        #s = "<font color='%s'>%.2f</font> [MHz]" % (c, self.tpxclockval)
        s = "<font color='%s'>%.2f</font> [MHz]" % (c, self.tpxclock)
        return s
    #
    # BS preamp enabled?
    def isBSPreampOK(self):
        return isinstance(self.bspreamp, bool)
    def setBSPreamp(self, bspreamp):
        self.bspreamp = bspreamp
        if self.dbg:
            if self.bspreamp:
                res = "Yes"
            else:
                res = "No"
            print("* Is the BS preamp enabled?                  : %s" % (res))
    def getBSEnabled(self):
        if self.isBSPreampOK():
            c = "green"
        else:
            c = "red"
        if self.bspreamp:
            res = "Yes"
        else:
            res = "No"
        s = "<font color='%s'>%s</font>" % (c, res)
        return s
    #
    # Source information
    #--------------------

    # Source ID
    #
    def isSourceIDOK(self):
        return self.sourceid != ""
    def setSourceID(self,sourceid):
        self.sourceid = sourceid
        if self.dbg:
            print("* Source ID is                      : %s" % (self.sourceid))
    def getSourceID(self):
        s = ""
        if self.isSourceIDOK():
            s = "<font color='green'>%s</font>" % (self.sourceid)
        else:
            s = "<font color='red'>INVALID SOURCE ID</font>"
        return s
