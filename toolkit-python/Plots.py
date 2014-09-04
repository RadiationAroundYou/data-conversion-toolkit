#!/usr/bin/env python

# Import the plotting libraries.
import pylab as plt

# Load the LaTeX text plot libraries.
from matplotlib import rc

# Uncomment to use LaTeX for the plot text.
rc('font',**{'family':'serif','serif':['Computer Modern']})
rc('text', usetex=True)

# Import the math libraries.
import numpy as np
from scipy.misc import factorial
from scipy.stats import poisson
from scipy import optimize

class RateHistogram():
    """Class for creating particle rate histograms.
    """

    ## Constructor
    # @param [in,out] self The object pointer.
    # @param [in] num The figure number.
    # @param [in] name The name of the histogram.
    # @param [in] The output path for the histogram image file.
    # @param [in] dbg Run in debug mode?

    def __init__(self, num, name, Delta_t, outputpath, dbg=False):

        ## The figure number.
        self.num = num

        ## The histogram name.
        self.name = name

        ## The run acquisition time.
        self.Delta_t = Delta_t

        ## The output path.
        self.outputpath = outputpath

        ## Run in debug mode?
        self.dbg = dbg

        ## Bin width.
        self.bin_w = 1

        ## The figure.
        self.plot = plt.figure(num, figsize=(5.0, 3.0), dpi=150, facecolor='w', edgecolor='w')

        # Adjust the subplot axes positions.
        self.plot.subplots_adjust(bottom=0.17, left=0.15)

        ## The plot.
        self.ax = self.plot.add_subplot(111)

        # Set the axis labels.
        self.ax.set_xlabel("Clusters per frame")
        self.ax.set_ylabel("Number of frames")

        # Add a grid.
        self.ax.grid(1)

        if self.dbg:
            print("* DEBUG: Histogram number is : %d"   % (self.num ))
            print("* DEBUG: Histogram name is   : '%s'" % (self.name))

    ## Method for filling the plot.
    #
    # @param [in] cpf List containing the clusters per frame for each frame.
    def fill(self, cpf):

        ## Clusters per frame (list).
        self.cpf = cpf

        # Work out the bin details.
        self.max_x = max(self.cpf)

        # Round this up to the nearest 10.
        self.max_x_r = np.floor(float(self.max_x)/10.) * 10. + 10.

        # Create the bin edges array for the data:
        # * Bin width one, rounded-up max x value.
        self.bins = np.arange(0, self.max_x_r + self.bin_w, self.bin_w)

        # Plot the histogram.
        #self.n, self.bins, self.patches = plt.hist(self.cpf, bins=self.bins, histtype='stepfilled')
        self.n, self.bins = np.histogram(self.cpf, bins=self.bins)

        # Set the colour of the histogram patches (make configurable?).
        #plt.setp(self.patches, 'facecolor', '#44BB11', 'alpha', 0.75, 'linewidth', 0.0)

        if self.dbg:
            print("* DEBUG: Number of clusters in each frame:", self.cpf)
            print("* DEBUG: Max number of clusters per frame  : %6d" % (self.max_x))
            print("* DEBUG: x max                             : %6d" % (self.max_x_r))
            print("* DEBUG: x bin width                       : %6d" % (self.bin_w))
            print("* DEBUG: bins:", self.bins)
            print("* DEBUG:")

#        # Fit to Poisson?
#        if self.n[0] != 60:
#            fitfunc = lambda p, x: p[0]*pow(p[1],x)*pow(np.e,-p[1])/factorial(x)
#            p0 = [10.0, 5.] # Initial guess for the parameters
#            errfunc = lambda p, x, y: fitfunc(p, x) - y # Distance to the target function
#            p1, success = optimize.leastsq(errfunc, p0[:], args=(self.bins[:-1], self.n))
#            print p1, success
#            plt.bar(self.bins, fitfunc(p1, self.bins))

        M = sum(self.cpf)
        N = len(self.cpf)
        Lambda = float(M) / float(N)
        #mymean = float(sum(self.cpf)/float(N))
        sd_Lambda = np.sqrt(float(M)) / N
        #mysd = np.sqrt(mymean) / N
        if self.dbg:
            print("* mean (Lambda)         is %f" % (Lambda))
            print("* s.d. (\sigma_{Lambda} is %f" % (sd_Lambda))

        rv = poisson(Lambda)
        #x = np.arange(0,np.min(rv.dist.b,3)+1)
        h = plt.bar(self.bins,rv.pmf(self.bins)*float(sum(self.n)))

        #fitfunc = lambda p, x: p[0]*pow(p[1],x)*pow(np.e,-p[1])/factorial(x)
        #plt.bar(self.bins, fitfunc(, self.bins))
        #
        #plt.plot(self.bins[:-1] + 0.5, self.n, color="black")

        # Firstly, we'll need an array of the bin centres to put the points
        # at (rather than the bin edges). Note the slicing of the bin edges
        # array to get the N bins (as otherwise we'd have N+1 bin edges).
        self.bins_centres = 0.5*(self.bins[1:]+self.bins[:-1])

        if self.dbg:
            print self.bins_centres

#    # Get the total number of clusters...
#    hpc_dat_sum = sum(n_r)
#
#    # And multiply by the bin width. We'll need this for the normalisation.
#    hpc_dat_sum = hpc_dat_sum * hpc_dat_bin_width
#    #print("* Scale factor (for normalisation) = %f" % (hpc_dat_sum))
#
        # Some bins will be empty. To avoid plotting these, we can replace
        # the contents of that bin with "nan" (not a number).
        # matplotlib then cleverly skips over these, leaving the x axis
        # clean and shiny.
        self.n = [np.nan if x==0 else float(x) for x in self.n]
        if self.dbg:
            print("* n_r after zero-bin replacement:")
            print self.n

        # Calculate the errors on the number of clusters counted (Poisson).
        self.err = np.sqrt(self.n)
        if self.dbg:
            print("* The errors on each bin: ")
            print self.err

#    # Normalise the histogram entries and the errors.
#    n_r = n_r/hpc_dat_sum
#    err = err/hpc_dat_sum
#    #print("* The histogram values after normalisation:")
#    #print n_r
#    #print("* The errors after normalisation:")
#    #print err
#
        # Plot the real data points as an "errorbar" plot
        plt.errorbar(self.bins_centres, \
                     self.n, \
                     fmt='d', \
                     color='black', \
                     yerr=self.err, \
                     ecolor='black', \
                     capthick=2, \
                     elinewidth=1)

        ## The actual rate [s^{-1}].
        rate_ps    = Lambda    / self.Delta_t
        sd_rate_ps = sd_Lambda / self.Delta_t

        ## The rate [min.^{-1}]
        rate_pm    = rate_ps    * 60.0 # [s^{-1}] * [s/min]
        sd_rate_pm = sd_rate_ps * 60.0 # [s^{-1}] * [s/min]

        # Add a title.
        self.ax.set_title("\\texttt{%s}: $\\Lambda = (%.2f \pm %.2f) \\, \\textrm{min.}^{-1}$" % (self.name, rate_pm, sd_rate_pm))

        # Ensure that the lower y limit is 0.
        self.ax.set_ylim(bottom=0.0)

        # Save the figure.
        self.plot.savefig("%s/%s.png" % (self.outputpath, self.name))

        #return Lambda, sd_Lambda
        return rate_pm, sd_rate_pm
