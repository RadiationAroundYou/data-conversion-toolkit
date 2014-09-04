#!/usr/bin/env python

"""

==============================================================================
                        CERN@school Simple Frame Viewer
==============================================================================

This script can be used to read and display Timepix data stored in the
ROOT format. See the Allpix README.md for further instructions.

This code owes a huge debt of thanks to:

Eli Bendersky (eliben@gmail.com)
http://eli.thegreenplace.net/

with respect to the PyQt GUI implementation.

"""

# Import the code needed to manage files.
import sys, os, inspect, glob

# Import the argument parser functionality.
import argparse

# Import the required ROOT libraries - hurrah for PyROOT!
from ROOT import TFile, gSystem

# Load in the (skeleton) Frame class - a bare-minimum class that
# provides the ROOT file format interface.
gSystem.Load('Frame_C')

# Import the Graphical User Interface (GUI) libraries.
from PyQt4.QtCore import *
from PyQt4.QtGui import *

# Import the plotting libraries (and their interfaces to the GUI libraries).
import matplotlib

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar

# ...for the figures.
from matplotlib.figure import Figure

# ...for the colour scaling.
from matplotlib import colorbar, colors

# ...for the plotting.
import matplotlib.pyplot as plt

# ...for nice text.
from matplotlib import rc

# Uncomment to use LaTeX for the plot text.
rc('font',**{'family':'serif','serif':['Computer Modern']})
rc('text', usetex=True)

# Import the data structures package.
from datastructs import *

# Import the validation package.
from mfvalidator import *

class AppForm(QMainWindow):
    """ The main GUI window - Qt-powered.

    """
    def __init__(self, args, parent=None):
        """ Constructor. """
        QMainWindow.__init__(self, parent)

        # Set the window title.
        self.setWindowTitle('CERN@school: ROOT Frame Viewer')

        ## The current frame number.
        self.fn = 0

        ### Dictionary for the detector data files.
        #self.datafile  = {}

        # Populate the data file dictionary.
        self.datafile = Data(args.inputFilePath)

        ## The maximum frame number.
        self.fmax = self.datafile.nframes - 1

        ## The pixel dictionary {frame number:pixels}
        self.data = [ {} ]

        ## Dictionary of the maximum count values for the frames.
        self.cmaxdict = {}

        self.metadata = [ {} ]

        # Get the first frame figure.

        # Create the mpl Figure and FigCanvas objects.
        # 5x4 inches, 100 dots-per-inch
        #
        self.dpi = 72

        ## The colour map for the pixel counts.
        self.cmap = plt.cm.hot # "Hot" used.

        ## The current maximum count value.
        self.cmax = 1

        # The figure and array of axes for the frame display.
        self.fig, self.axarr = plt.subplots(1, 1, figsize=(8.0,8.0), dpi=self.dpi)

        # Remove the axis labels.
        self.axarr.xaxis.set_visible(False)
        self.axarr.yaxis.set_visible(False)
        self.axarr.set_xlim([0,256])
        self.axarr.set_ylim([0,256])

        self.create_menu()
        self.create_main_frame()

        self.framenum_textbox.setText('%d' % self.fn)

        self.set_frame()

    def save_plot(self):
        file_choices = "PNG (*.png)|*.png"

        path = unicode(QFileDialog.getSaveFileName(self,
                        'Save file', '',
                        file_choices))
        if path:
            self.canvas.print_figure(path, dpi=self.dpi)
            self.statusBar().showMessage('Saved to %s' % path, 2000)

    def on_about(self):
        msg = """ A Simple ROOT-based Frame Display:

         * Browse frames using the navigation buttons below.
         * Set the maximum count value (i.e. white in the colour scale)
           with the Max. C text box.
        """
        QMessageBox.about(self, "About the ROOT Frame Display.", msg.strip())

#    def on_pick(self, event):
#        # The event received here is of the type
#        # matplotlib.backend_bases.PickEvent
#        #
#        # It carries lots of information, of which we're using
#        # only a small amount here.
#        #
#        box_points = event.artist.get_bbox().get_points()
#        msg = "You've clicked on a bar with coords:\n %s" % box_points
#
#        QMessageBox.information(self, "Click!", msg)

    def on_draw(self):
        """ Redraws the figure.

        """

        # Plot the frames.
        self.plot_frames()

        # Update the metadata.
        self.update_metadata()

        # Redraw the canvas.
        self.canvas.draw()

    def create_main_frame(self):
        """ Create the main frame of the GUI display.
        """

        ## The main frame.
        self.main_frame = QWidget()

        ## The canvas for the frame display figure.
        self.canvas = FigureCanvas(self.fig)

        # Set the canvas parent as the main frame.
        self.canvas.setParent(self.main_frame)

        # Other GUI controls
        #--------------------

        # Frame navigation
        #------------------
        self.frame_label = QLabel("Frame:")

        self.first_button = QPushButton("&First")
        self.connect(self.first_button, SIGNAL('clicked()'), self.first_frame)

        self.prev_button = QPushButton("&Previous")
        self.connect(self.prev_button, SIGNAL('clicked()'), self.prev_frame)

        self.framenum_textbox = QLineEdit()
        self.framenum_textbox.setMinimumWidth(50)
        self.connect(self.framenum_textbox, SIGNAL('editingFinished ()'), self.set_frame_from_textbox)

        self.next_button = QPushButton("&Next")
        self.connect(self.next_button, SIGNAL('clicked()'), self.next_frame)

        self.last_button = QPushButton("&Last")
        self.connect(self.last_button, SIGNAL('clicked()'), self.last_frame)

        # Display
        #---------
        self.cmax_label = QLabel("Max. C:")

        self.cmax_textbox = QLineEdit()
        self.cmax_textbox.setText('%d' % self.cmax)
        self.cmax_textbox.setMinimumWidth(50)
        self.cmax_textbox.setMaximumWidth(50)
        self.connect(self.cmax_textbox, SIGNAL('editingFinished ()'), self.set_cmax)

        # Define the layout with box sizers
        #-----------------------------------

        ## Layout box for the frame navigation panel.
        hbox = QHBoxLayout()

        # Loop over the widgets.
        for w in [ \
            self.frame_label, \
            self.first_button, \
            self.prev_button, \
            self.framenum_textbox, \
            self.next_button, \
            self.last_button, \
            self.cmax_label, \
            self.cmax_textbox
            ]:
            hbox.addWidget(w)
            hbox.setAlignment(w, Qt.AlignVCenter)

        ## Layout box for the frame display and control panel.
        vbox = QVBoxLayout()

        # Add the canvas and the control panel.
        vbox.addWidget(self.canvas)
        vbox.addLayout(hbox)

        #self.main_frame.setLayout(vbox)
        #self.main_frame.setFixedHeight(630)
        #self.main_frame.setFixedWidth(630)
        #self.setCentralWidget(self.main_frame)

        # The box for the frame status.
        statBox = QGridLayout()
        statBox.setSpacing(5)

        self.gridlabels = {}
        self.gridvalues = {}

        # #widgetlist1
        for i, key in enumerate([ \
            "#Payload", \
            "Hit pixels", \
            "Total counts", \
            "Data type", \
            #
            "#Payoad info.", \
            "Dataset ID", \
            "Frame ID", \
            "Frame size", \
            "Original format", \
            "Occupancy", \
            #
            "#Geospatial info.", \
            "Location", \
            "Altitude", \
            "Lab rotation", \
            "Lab Omega", \
            #
            "#Temporal info.", \
            "Start time", \
            "Start time (str)", \
            "Acquisition time", \
            "End time (str)", \
            #
            #"spacer1", \
            "#Acquisition", \
            "Acq. mode", \
            "%Counters", \
            "%HW timer mode", \
            "%Auto erase int.", \
            "%Int. counter", \
            "%Last trigger time", \
            "%Coinc. mode", \
            "%Coinc. delay", \
            "%Coinc. live time", \
            "Pixelman ver", \
            ]):
            if key[0]=="#":
                self.gridlabels[key] = QLabel("<b>%s</b>" % (key[1:]))
            elif "?" in key:
                self.gridlabels[key] = QLabel("%s" % (key))
            elif "%" in key:
                self.gridlabels[key] = QLabel("<font color='#999999'>%s</font>" % (key[1:]))
            elif "spacer" in key:
                self.gridlabels[key] = QLabel("")
            else:
                self.gridlabels[key] = QLabel("%s:" % (key))
            self.gridlabels[key].setFixedWidth(100)
            statBox.addWidget(self.gridlabels[key], i, 0)

            self.gridvalues[key] = QLabel("")
            self.gridvalues[key].setFixedWidth(220)
            #self.gridvalues[key].setReadOnly(True)
            statBox.addWidget(self.gridvalues[key], i, 1)

        # #widgetlist2
        for i, key in enumerate([ \
            "#Detector info.", \
            "Chipboard ID", \
            "Custom name", \
            "Firmware", \
            "Interface", \
            "Medipix type", \
            "Applied filters", \
            "Position", \
            "Rotation", \
            #
            #"spacer1", \
            "#Settings", \
            "Polarity", \
            "Bias voltage", \
            "IKrum", \
            "Disc.", \
            "Preamp", \
            "BuffAnalogA", \
            "BuffAnalogB", \
            "Hist", \
            "THL", \
            "THLCoarse", \
            "Vcas", \
            "FBK", \
            "GND", \
            "THS", \
            "BiasLVDS", \
            "RefLVDS", \
            "Medipix clock", \
            "Timepix clock", \
            "BS preamp?", \
            #
            "#Source info.", \
            "Source ID", \
            ]):
            if key[0]=="#":
                self.gridlabels[key] = QLabel("<b>%s</b>" % (key[1:]))
            elif "?" in key:
                self.gridlabels[key] = QLabel("%s" % (key))
            elif "spacer" in key:
                self.gridlabels[key] = QLabel("")
            else:
                self.gridlabels[key] = QLabel("%s:" % (key))
            self.gridlabels[key].setFixedWidth(100)
            statBox.addWidget(self.gridlabels[key], i, 2)

            self.gridvalues[key] = QLabel("")
            self.gridvalues[key].setFixedWidth(180)
            #self.gridvalues[key].setReadOnly(True)
            statBox.addWidget(self.gridvalues[key], i, 3)

        hbox2 = QHBoxLayout()
        hbox2.addLayout(statBox)
        hbox2.addLayout(vbox)

        self.main_frame.setLayout(hbox2)
        self.main_frame.setFixedHeight(630)
        self.main_frame.setFixedWidth(1230)
        self.setCentralWidget(self.main_frame)

    ## Create the menu.
    #
    # (Taken directly from Eli's example code - thanks!)
    def create_menu(self):
        self.file_menu = self.menuBar().addMenu("&File")

        load_file_action = self.create_action("&Save plot",
            shortcut="Ctrl+S", slot=self.save_plot,
            tip="Save the plot")
        quit_action = self.create_action("&Quit", slot=self.close,
            shortcut="Ctrl+Q", tip="Close the application")

        self.add_actions(self.file_menu,
            (load_file_action, None, quit_action))

        self.help_menu = self.menuBar().addMenu("&Help")
        about_action = self.create_action("&About",
            shortcut='F1', slot=self.on_about,
            tip='About the demo')

        self.add_actions(self.help_menu, (about_action,))

    ## Add the menu actions.
    #
    # @param [in] The target.
    # @param [in] The actions.
    def add_actions(self, target, actions):
        for action in actions:
            if action is None:
                target.addSeparator()
            else:
                target.addAction(action)

    ## Create the menu actions.
    #
    def create_action(  self, text, slot=None, shortcut=None,
                        icon=None, tip=None, checkable=False,
                        signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

    ## Set the frame number.
    #
    def set_frame(self):

        # Change the frame number in the text box to the current frame.
        self.framenum_textbox.setText('%d' % self.fn)

        # Check if the dictionary has the data.
        if self.fn not in self.data[0]:
            self.load_frame()

        # Set the maximum C count value to that of the frame's.
        self.cmax_textbox.setText('%d' % self.cmaxdict[self.fn])
        self.cmax = self.cmaxdict[self.fn]
        self.set_cmax

        # Update the canvas with the new frame.
        self.on_draw()

    def first_frame(self):
        """ Go to the first frame. """
        self.fn = 0
        self.set_frame()

    def prev_frame(self):
        """ Go to the previous frame. """
        if self.fn > 0:
            self.fn -= 1
        self.set_frame()

    def set_frame_from_textbox(self):
        """ Go to the frame in the textbox. """
        self.fn = int(self.framenum_textbox.text())
        self.set_frame()

    def next_frame(self):
        """ Go to the next frame. """
        if self.fn < self.fmax:
            self.fn += 1
        self.set_frame()

    def last_frame(self):
        """ Go to the last frame. """
        self.fn = self.fmax
        self.set_frame()

    def set_cmax(self):
        """ Set the maximum count value to display (for the colour map). """
        self.cmax = int(self.cmax_textbox.text())
        self.on_draw()

    def plot_frames(self):
        """ Plot the frame contents on the frame display.

        """

        self.plot_frame(self.axarr, self.data[0][self.fn], 0)

    ## Plot the individual frame data for a given detector.
    # @param self The object pointer.
    # @param [in] ax The axis upon which to display the frame.
    # @param [in] pxs The pixel dictionary {X:C}.
    # @param [in] tpxnum The Timepix detector number.
    def plot_frame(self, ax, pxs, tpxnum):

        # Clear the display.
        ax.clear()

        # Set the background colour.
        ax.set_axis_bgcolor('#82bcff')

        #ax.set_title("TPX%d" % (tpxnum))

        # Loop over the pixels in the pixel dictionary.
        for X, C in pxs.iteritems():
            ## The pixel x value.
            x = X%256;

            ## The pixel y value.
            y = X/256
            #print("* DEBUG: Pixel found at (%3d, %3d) with C = %d" % (x,y,C))
            if C > 0:
                color = float(C) / float(self.cmax)
                #print("* DEBUG: C = %d - setting color to %f" % (C, color))

                # Plot the pixel.
                ax.add_patch(plt.Rectangle((x,y),1,1, edgecolor=self.cmap(color), facecolor=self.cmap(color)))
            else:
                ax.add_patch(plt.Rectangle((x,y),1,1, edgecolor='#33aacc', facecolor='#33aacc'))

    ## Load the current frame into memory.
    #
    def load_frame(self):

        # Loop over the detectors (TPX0 - TPX4).
        cmax = 1
        for i, pxsdict in enumerate(self.data):

            # If the frame number is not found in frame-pixels dictionary,
            # retrieve the data from that and add it to the detector's
            # dictionary.
            if self.fn not in pxsdict:

                # Retrieve the pixel data for the detector/frame
                #pxsdict[self.fn] = self.datafiles[i].getPixels(self.fn)
                pxsdict[self.fn], detcmax = self.datafile.getPixels(self.fn)

                # Adjust the maximum C, if required.
                if detcmax > cmax:
                    cmax = detcmax

        # Assign the maximum value from all five detectors for this frame.
        if self.fn not in self.cmaxdict:
            self.cmaxdict[self.fn] = cmax

        for i, metadatadict in enumerate(self.metadata):
            if self.fn not in metadatadict:
                metadatadict[self.fn] = self.datafile.getMetadata(self.fn)

    def update_metadata(self):
        self.gridvalues["Hit pixels"     ].setText(self.metadata[0][self.fn].getNumHits())
        self.gridvalues["Total counts"   ].setText(self.metadata[0][self.fn].getTotalCounts())
        self.gridvalues["Data type"      ].setText(self.metadata[0][self.fn].getDataType())
        #
        self.gridvalues["Dataset ID"     ].setText(self.metadata[0][self.fn].getDatasetID())
        self.gridvalues["Frame ID"       ].setText(self.metadata[0][self.fn].getFrameID())
        self.gridvalues["Frame size"     ].setText(self.metadata[0][self.fn].getFrameSize())
        self.gridvalues["Original format"].setText(self.metadata[0][self.fn].getFormat())
        self.gridvalues["Occupancy"      ].setText(self.metadata[0][self.fn].getOccupancy())
        #
        # Acquisition information
        #-------------------------
        self.gridvalues["Acq. mode"        ].setText(self.metadata[0][self.fn].getAcqMode())
        na = "<font color='#999999'>n/a</font>"
        self.gridvalues["%Counters"         ].setText(na)
        self.gridvalues["%HW timer mode"    ].setText(na)
        self.gridvalues["%Auto erase int."  ].setText(na)
        self.gridvalues["%Int. counter"     ].setText(na)
        self.gridvalues["%Last trigger time"].setText(na)
        self.gridvalues["%Coinc. mode"      ].setText(na)
        self.gridvalues["%Coinc. delay"     ].setText(na)
        self.gridvalues["%Coinc. live time" ].setText(na)
        self.gridvalues["Pixelman ver"     ].setText(self.metadata[0][self.fn].getPixelmanVer())
        #
        # Geospatial information
        #------------------------
        self.gridvalues["Location"    ].setText(self.metadata[0][self.fn].getLocation())
        self.gridvalues["Altitude"    ].setText(self.metadata[0][self.fn].getAltitude())
        self.gridvalues["Lab rotation"].setText(self.metadata[0][self.fn].getLabRotation())
        self.gridvalues["Lab Omega"   ].setText(self.metadata[0][self.fn].getLabOmega())
        #
        # Temporal information
        #----------------------
        self.gridvalues["Start time"      ].setText(self.metadata[0][self.fn].getStartTime())
        self.gridvalues["Start time (str)"].setText(self.metadata[0][self.fn].getStartTimeStr())
        self.gridvalues["Acquisition time"].setText(self.metadata[0][self.fn].getAcqTime())
        self.gridvalues["End time (str)"  ].setText(self.metadata[0][self.fn].getEndTimeStr())
        #
        # Detector information
        #----------------------
        self.gridvalues["Chipboard ID"   ].setText(self.metadata[0][self.fn].getChipboardID())
        self.gridvalues["Custom name"    ].setText(self.metadata[0][self.fn].getCustomName())
        self.gridvalues["Firmware"       ].setText(self.metadata[0][self.fn].getFirmware())
        self.gridvalues["Interface"      ].setText(self.metadata[0][self.fn].getInterfaceType())
        self.gridvalues["Medipix type"   ].setText(self.metadata[0][self.fn].getMpxType())
        self.gridvalues["Applied filters"].setText(self.metadata[0][self.fn].getAppFilters())
        self.gridvalues["Position"       ].setText(self.metadata[0][self.fn].getDetPosition())
        self.gridvalues["Rotation"       ].setText(self.metadata[0][self.fn].getDetRotation())
        #
        # Detector settings
        #-------------------
        self.gridvalues["Polarity"     ].setText(self.metadata[0][self.fn].getPolarity())
        self.gridvalues["Bias voltage" ].setText(self.metadata[0][self.fn].getBiasVoltage())
        self.gridvalues["IKrum"        ].setText(self.metadata[0][self.fn].getIKrum())
        self.gridvalues["Disc."        ].setText(self.metadata[0][self.fn].getDisc())
        self.gridvalues["Preamp"       ].setText(self.metadata[0][self.fn].getPreamp())
        self.gridvalues["BuffAnalogA"  ].setText(self.metadata[0][self.fn].getBuffAnalogA())
        self.gridvalues["BuffAnalogB"  ].setText(self.metadata[0][self.fn].getBuffAnalogB())
        self.gridvalues["Hist"         ].setText(self.metadata[0][self.fn].getHist())
        self.gridvalues["THL"          ].setText(self.metadata[0][self.fn].getTHL())
        self.gridvalues["THLCoarse"    ].setText(self.metadata[0][self.fn].getTHLCoarse())
        self.gridvalues["Vcas"         ].setText(self.metadata[0][self.fn].getVcas())
        self.gridvalues["FBK"          ].setText(self.metadata[0][self.fn].getFBK())
        self.gridvalues["GND"          ].setText(self.metadata[0][self.fn].getGND())
        self.gridvalues["THS"          ].setText(self.metadata[0][self.fn].getTHS())
        self.gridvalues["BiasLVDS"     ].setText(self.metadata[0][self.fn].getBiasLVDS())
        self.gridvalues["RefLVDS"      ].setText(self.metadata[0][self.fn].getRefLVDS())
        self.gridvalues["Medipix clock"].setText(self.metadata[0][self.fn].getMpxClock())
        self.gridvalues["Timepix clock"].setText(self.metadata[0][self.fn].getTpxClock())
        self.gridvalues["BS preamp?"   ].setText(self.metadata[0][self.fn].getBSEnabled())
        #
        # Source information
        #--------------------
        self.gridvalues["Source ID"].setText(self.metadata[0][self.fn].getSourceID())

def main(args):
    """ The main program.

    """

    app = QApplication(sys.argv)

    form = AppForm(args)

    form.show()

    app.exec_()

#
# The main event!
#
if __name__ == "__main__":

    print("================================================")
    print("     CERN@school ROOT Frame Viewer (Python)     ")
    print("================================================")

    # Get the datafile paths from the command line.
    parser = argparse.ArgumentParser()
    parser.add_argument("inputFilePath", help="The path of the Timepix data file."  )
    args = parser.parse_args()

    # Call the main program.
    main(args)
