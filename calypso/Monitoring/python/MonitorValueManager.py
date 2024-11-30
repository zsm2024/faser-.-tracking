#!/bin/env python
#
# MonitorValueManager.py
#
# Defines the MonitorValueManager class that coordinates actions
# for a large set of instantiated MonitorValue objects
#
# Classes
#   MonitorValueManager
#
#  E.Torrence, Aug. 2019 
#
# Use python3-style print statements
from __future__ import print_function

import ROOT
import json

from Monitoring.MonitorValueBase import monitorValueFromJSON

# Need to import all defined MV types so we can find them below
from Monitoring.Monitor1DValue import Monitor1DValue


class MonitorValueManager:
    """Class to instantiate and manage a large set of MonitorValue objects"""

    def __init__(self):
        """Constructor"""

        # Flag to print out some monitoring information
        self.debug = True

        # dict containing all MonitorValues, tags are used for keys
        self.mvalDict = dict()

        # Key list, used to keep order straight
        self.tagList = []

    def addValue(self, val):
        """Add an instantiated MonitorValue to the dictionary"""

        tag = val.conf.tag
        if tag in self.mvalDict.keys():
            print("Value with tag", tag,"already exists!")
            print(self.mvalDict[tag].conf)
            return

        self.mvalDict[tag] = val
        self.tagList.append(tag)
        
    def initValues(self):
        """Call the init method on all stored MVs"""
        # Loop through all MonitorValues defined and call the init() function
        for val in self.mvalDict.itervalues():
            if (self.debug):
                print("Initializing value", val.conf.tag)

            val.init()

    def fillValues(self, event):
        """Call the fill method on all stored MVs"""
        for val in self.mvalDict.itervalues():
            val.fill(event)

    def writeValues(self, filename):
        """Write out value histograms to file"""

        # Simplest implementation for now
        # Add some directory structure later
        f = ROOT.TFile(filename, "RECREATE")

        for val in self.mvalDict.itervalues():
            val.writeRoot(f)

        # Cleanup
        f.Close()

    def plotValuesToPDF(self, filename):
        """Call the plot methods with output going to a PDF file"""
        
        # Lets see if this works
        c = ROOT.TCanvas()
        c.Print(filename+"[")

        for tag in self.tagList:
            val = self.mvalDict[tag]
            val.plot(c)
            c.Print(filename)

        c.Print(filename+"]")

    def createValuesFromJSON(self, filename):
        """ Create values from JSON file of name filename"""

        if self.debug: print("Opening JSON file", filename)
        # Open file
        try:
            f = open(filename)
        except Exception as e:
            print("Error opening file", filename)
            print(e)
            return

        # Parse JSON
        if self.debug: print("Parsing JSON")
        jsonList = json.load(f)

        # Close the file
        f.close()

        # Keep track of what we are doing
        count = 0
        if self.debug: print("Creating objects from JSON")
        for item in jsonList:
            try:
                newval = monitorValueFromJSON(item)
            except Exception as e:
                # On error, print error and continue
                print("MonitorValueManager.createValuesFromJSON - Error creating object", item, "!")
                print(e)
                continue

            self.addValue(newval)
            count += 1

        # Print out some results
        if (self.debug):
            print("Created", count, "new values")
            print("Full list of MonitorValue tags:")
            for key in sorted(self.mvalDict.keys()):
                print(key)

    def listValues(self):
        """Print out type and tags for all defined values"""


        print ("Listing of MonitorValues")

        if len(self.mvalDict) == 0:
            print("Value list is empty!")
            return


        print("Tag   ValueType")
        print("---   ---------")
        for tag in self.tagList:
            print(tag, self.mvalDict[tag].conf.type)

    def dumpValues(self):
        """Print out verbose information for all defined values"""

        if len(self.mvalDict) == 0:
            print("Value list is empty!")
            return

        for tag in self.tagList:         # Sort to get alphabetical listing
            print()
            print(tag, self.mvalDict[tag].conf.type)
            self.mvalDict[tag].dump()

    def writeJSON(self, stream):
        """Write out JSON for all configured objects to specified stream"""

        jsonList = []
        for tag in self.tagList:
            val = self.mvalDict[tag]
            jsonList.append(val.conf)

        json.dump(jsonList, stream, default=lambda o: o.__dict__, 
                  sort_keys=True, indent=2)

# Testing code
if __name__ == '__main__':

    from Monitoring.Monitor1DValue import Monitor1DValue

    mv1 = Monitor1DValue('test1')
    mv1.conf.setBinning(30, -3., 3.)

    mv2 = Monitor1DValue('test2')
    mv2.conf.setBinning(60, -3., 3.)

    mvm = MonitorValueManager()

    mvm.addValue(mv1)  # Add already defined mv
    mvm.addValue(mv2)
    mvm.initValues()   # Initialize all values
    mvm.listValues()   # Brief printout of all values
    mvm.dumpValues()


    import sys
    mvm.writeJSON(sys.stdout)
    print()

    mvm.createValuesFromJSON('test.json')

    mvm.listValues()

