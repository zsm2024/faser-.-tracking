#!/bin/env python
#
#  Monitor1DValue.py
#
#  Defines classes to implement monitored values
#
#  Classes
#    Monitor1DValue
#    Monitor1DValueConfig
#
#  E.Torrence, Aug. 2019
#
#
# Use Python3-style print statements
from __future__ import print_function

import sys
import ROOT

from Monitoring.MonitorValueBase import MonitorValueBase, MonitorConfigBase

#
# 1D Monitoring class and configuration
#
class Monitor1DValueConfig(MonitorConfigBase):
    """Configuration for Monitor1DValue"""

    # Everything in this configuration must be simple values so JSON works

    def __init__(self):
        """Constructor providing default values"""

        MonitorConfigBase.__init__(self)
        # Defines tag, type, and desc

        # Variable
        self.selector = None # Text string describing selector class

        self.xvar = None   # Text string describing value to fill

        # Scale factor
        self.xscale = 1

        # Histogram parameters
        self.nbins = 0
        self.xlo = 0.
        self.xhi = 0.
        self.xlabel = ""
        self.ylabel = ""

        # Plot under/overflows
        self.overflow = True

        # Draw as linear or log scale by default
        self.logy = False

    # Utility function to make it easier to define hist parameters
    def setBinning(self, nbins, xlo, xhi, xlabel="", ylabel=""):
        """Define histogram bins and range"""

        # Utility function to set histogram parameters in one go
        self.nbins = nbins
        self.xlo = xlo
        self.xhi = xhi
        self.xlabel = xlabel
        self.ylabel = ylabel

    
class Monitor1DValue (MonitorValueBase):
    """One dimensional monitored value class"""

    def __init__(self, tag=''):
        """Constructor, tag is a unique identification string"""

        # Instantiate the configuration object
        self.conf = Monitor1DValueConfig()

        # Set the tag if provided
        self.conf.tag = tag

    def init(self):
        """Initialize class and create histogram"""

        # First call the base class, which checks a few things
        MonitorValueBase.init(self)

        # Now create our histogram with the parameters in self.conf
        # Check if we want to store integers or floats
        if isinstance(self.conf.xlo, int) and isinstance(self.conf.xhi, int):
            self.hist = ROOT.TH1I(self.conf.tag+"_h", "", 
                                  self.conf.nbins, self.conf.xlo, self.conf.xhi)
        else: 
            self.hist = ROOT.TH1D(self.conf.tag+"_h", "", 
                                  self.conf.nbins, self.conf.xlo, self.conf.xhi)

        # Also set labels
        self.hist.GetXaxis().SetTitle(self.conf.xlabel)
        self.hist.GetYaxis().SetTitle(self.conf.ylabel)

    def fill(self, event):
        """Fill histogram from event object"""

        self.fillValue(self.selector.get1DValue(event, self.conf.xvar))

        #varstr = self.conf.xvar
        #theVar = getattr(event, varstr)
        #self.fillValue(theVar)

    def fillValue(self, val):
        """Fill histogram with one or more values"""

        #print ("Monitor1DValue.fillValue called with", val)

        # Check if this is a single value or an iterable sequence
        if hasattr(val, '__iter__'):
            for v in val:
                self.hist.Fill(v*self.conf.xscale)
        else:
            self.hist.Fill(val*self.conf.xscale)


    def plot(self, c):
        """Plot histogram to canvas c"""

        # Log or lin?
        if self.conf.logy:
            c.SetLogy(True)
        else:
            c.SetLogy(False)

        # Do something fancy to draw the overflow?
        if self.conf.overflow:
            # Must make a new histogram and fill under/overflow in first/last bins
            h = self.hist.Clone()
            nb = h.GetNbinsX()

            under = h.GetBinContent(0)
            h.SetBinContent(1, h.GetBinContent(1)+under)
            over = h.GetBinContent(nb+1)
            h.SetBinContent(nb, h.GetBinContent(nb)+over)

        else:
            h = self.hist

        # Draw with default parameters
        h.DrawCopy()

# Testing code
if __name__ == '__main__':

    mv1 = Monitor1DValue('test1')
    mv1.conf.setBinning(30, -3., 3.)
    mv1.init()

    mv2 = Monitor1DValue('test2')
    mv2.conf.setBinning(30, -3., 3.)
    mv2.init()

    # Fill a list
    import random
    values = [random.gauss(0., 1.) for x in range(100)]
    mv1.fill(values)

    # Try one more
    mv1.fill(random.gauss(0., 1.))

    # Print something out
    mv1.dump()

    # Write to file
    print()
    print("Test writing to ROOT file")
    tf = ROOT.TFile("test.root", "recreate")
    mv1.writeRoot(tf)
    mv2.writeRoot(tf)
    #writeMonitorValue(tf, mv1)
    #writeMonitorValue(tf, mv2)
    tf.Close()

    print()
    print("Test reading from ROOT file")
    tf = ROOT.TFile('test.root')

    print()
    print("Directory listing")
    tf.ls()

    from Monitoring.MonitorValueBase import readMonitorValueFromRoot

    print()
    print("Read objects back from ROOT file")    
    mvr1 = readMonitorValueFromRoot(tf, "test1")
    mvr1.dump()
    mvr2 = readMonitorValueFromRoot(tf, "test2")
    mvr2.dump()
    try:
        readMonitorValueFromRoot(tf, "test3")
    except Exception as e:
        print(e)

