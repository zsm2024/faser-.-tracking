#!/bin/env python
#
# simpleMonitoringTest.py
#
# Example of simple monitoring job using hte MonitorValueManager
# Need JSON file simpleMonitoringTest.json and a TDR-style
# ntuple in the run directory
#
# E. Torrence, Aug. 2019
#
# Use python3-style print statements
from __future__ import print_function

from Monitoring.MonitorValueManager import MonitorValueManager

import ROOT

# Start by instantiating mvm
mvm = MonitorValueManager()

# Load the configuration
mvm.createValuesFromJSON('simpleMonitoringTest.json')
mvm.initValues()
# mvm.listValues()  # Check what we have
mvm.dumpValues()  # Check what we have

# Open ntuple
tf = ROOT.TFile('ntuple.root')
tt = tf.Get('faser')

# Loop over events
for event in tt:
    mvm.fillValues(event)

tf.Close()

# Write to file
mvm.writeValues('simpleMonitoringTest.root')

# Plot values
mvm.plotValuesToPDF('simpleMonitoringTest.pdf')
