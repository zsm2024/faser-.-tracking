#!/usr/bin/env python

# This script combines the strip occupancy histograms from NoisyStripFinderHist.root files then it generates a pdf with sumarry plots and an XML file with a list of noisy strips

# Set up (Py)ROOT.
import ROOT
import xml.etree.ElementTree as ET

ROOT.gStyle.SetOptStat(0) #take away option box in histograms
ROOT.gStyle.SetOptTitle(1)
#ROOT.gROOT.SetStyle("ATLAS")

def GetKeyNames( self ): # gets a list of object names in a root file
    return [key.GetName() for key in f.GetListOfKeys()]
ROOT.TFile.GetKeyNames = GetKeyNames

numEvents = 0
nfiles = 0
HistDict = {}

ROOT.TH1.AddDirectory(0) # This is necessary in order to have the histogram data after closing the file

for inputfile in ["./NoisyStripFinderHist.root"]:
    print("opening root file ",inputfile)
    f = ROOT.TFile.Open(inputfile, "r")
    numEvents += f.Get("numEvents").GetVal()
    if nfiles == 0:
        trigger = f.Get("trigger").GetVal()
    for rootkey in f.GetKeyNames():
        if rootkey == 'numEvents' or rootkey == 'trigger':
            continue # skip over the root objects TParameters that store the trigger and number of events data
        if rootkey in HistDict: # if sensor histogram has already been stored, then add to it 
            HistDict[rootkey].Add(f.Get(rootkey),1.0)
        else: # if sensor histogram has not already been stored, then store this histogram
            HistDict[rootkey] = f.Get(rootkey).Clone()

    nfiles += 1

    # Good to cleanup
    f.Close()

print("Total # of root files analyzed = ", nfiles)
print("Trigger mask = ", trigger)
print("Total number of events = ", numEvents)

# Now make some plots
filename = "NoisyStripFinderHist_Analysis.pdf"

c = ROOT.TCanvas()
c.Print(filename+'[')

for dictkey in HistDict:
    c = ROOT.TCanvas()
    c.Divide(1,2)
    c.cd(1)
    tempHist = HistDict[dictkey].Clone()
    tempHist.Draw("")
    c.cd(2)
    HistDict[dictkey].Scale(1.0/float(numEvents)) # Normalize histrograms by total event number in order to get occupancies
    HistDict[dictkey].GetYaxis().SetTitle("Strip Occupancy")
    HistDict[dictkey].Draw("")
#    ROOT.gPad.SetLogy()
    c.Print(filename)

c.Print(filename+']') # Must close file at the end

# Now make an XML file to store noisy strip data
root = ET.Element('NoisyStripData')
for dictkey in HistDict:
    sensorhash = ET.SubElement(root, "Sensor_Hash")
    sensorhash.text = dictkey
    bini = 1
    while bini <= 768:
        if HistDict[dictkey].GetBinContent(bini) >= 0.01:
            strip = ET.SubElement(sensorhash, "Strip")
            strip.text = str(bini - 1) # strip number is offset by histogram bin number by 1 because of underflow bin
            occupancy = ET.SubElement(strip, "Occupancy")
            occupancy.text = str(HistDict[dictkey].GetBinContent(bini))
        bini += 1           
 
tree = ET.ElementTree(root)
tree.write("NoisyPixels.xml")

