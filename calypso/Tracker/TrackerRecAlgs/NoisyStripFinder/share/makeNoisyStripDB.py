#!/usr/bin/env python3

import os
import sys
import ROOT
import argparse
import array
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

parser = argparse.ArgumentParser()
parser.add_argument("file", nargs="+", help="full path to input file")
parser.add_argument("-t", "--threshold", type=float, default=0.001, help="add strips with an occupancy larger this threshold to the database")
parser.add_argument("--force", "-f", action="store_true", help="Overwrite existing DB")
parser.add_argument("--permissive", action="store_true", help="Allow some input files to be missing")
parser.add_argument("--output", "-o", default="noisy_strips.db", help="Specify output DB")
parser.add_argument("--isMC", action="store_true", help="Write MC DB (default: real data)")
parser.add_argument("--use_blob", action="store_true", help="Write to DB using blob (default: save as string")

args = parser.parse_args()

def GetKeyNames(self):
    return [key.GetName() for key in f.GetListOfKeys()]

ROOT.TFile.GetKeyNames = GetKeyNames

numEvents = 0
nfiles = 0
HistDict = {}

ROOT.TH1.AddDirectory(0) # This is necessary in order to have the histogram data after closing the file

trigger = None
iovlo = cool.ValidityKeyMax
iovhi = cool.ValidityKeyMin

# Keys to skip
skipList = ["numEvents", "trigger", "IOVLoRun", "IOVLoLB", "IOVHiRun", "IOVHiLB"]

for inputfile in args.file:
    # Check that this exists
    if not os.path.exists(inputfile) :
        if args.permissive: continue
        print(f"File {inputfile} not found!")
        sys.exit(1)

    try:
        f = ROOT.TFile.Open(inputfile, "r")
    except Exception as e:
        print(e)
        if args.permissive: continue
        sys.exit(1)

    n = f.Get("numEvents").GetVal()
    print(f"Found {n} events in {inputfile}")
    if n == 0: continue
    numEvents += n
    lorun = f.Get("IOVLoRun").GetVal()
    hirun = f.Get("IOVHiRun").GetVal()
    lo = (lorun << 32)
    hi = ((hirun+1) << 32) - 1
    if lo < iovlo: iovlo = lo
    if hi > iovhi: iovhi = hi

    if trigger is None:
        trigger = f.Get("trigger").GetVal()
    else:
        t = f.Get("trigger").GetVal()
        if t != trigger:
            print(f"Trigger mismatch! {t} != {trigger} in {inputfile}")
            sys.exit(1)  # This shouldn't happen

    for rootkey in f.GetKeyNames():

        # skip over the root objects TParameters that store the trigger and number of events data
        if rootkey in skipList: continue 

        if rootkey in HistDict: # if sensor histogram has already been stored, then add to it
            HistDict[rootkey].Add(f.Get(rootkey),1.0)
        else: # if sensor histogram has not already been stored, then store this histogram
            HistDict[rootkey] = f.Get(rootkey).Clone()

    nfiles += 1
    f.Close()

print(f"Total {nfiles} analyzed with {numEvents} events")
print(f"Trigger mask = 0x{trigger:02x}")
print(f"IOV from {(iovlo >> 32)}/{(iovlo & 0xFFFFFFFF)} to {(iovhi >> 32)}/{(iovhi & 0xFFFFFFFF)}")

# Write DB

dbSvc = cool.DatabaseSvcFactory.databaseService()
dbname = "CONDBR3"   # Real data
if args.isMC:
    dbname="OFLP200" # MC

connectString = f'sqlite://;schema={args.output};dbname={dbname}'

print(f"Using connection string {connectString}")

if os.path.exists(args.output):
    if args.force:
        print(f"Deleting {args.output} due to --force")
        os.remove(args.output)
    else:
        print(f"File {args.output} exists, use --force to overwrite")
        sys.exit(1)

try:
    print('Creating database')
    dbSvc.dropDatabase(connectString)
    db = dbSvc.createDatabase(connectString)
except Exception as e:
    print(e)
    sys.exit(1)

noisyStripsSpec = cool.RecordSpecification()
noisyStripsSpec.extend('sensor', cool.StorageType.Int32)
noisyStripsSpec.extend('nstrips', cool.StorageType.Int32)
if args.use_blob:
    noisyStripsSpec.extend('strip', cool.StorageType.Blob64k) # Array of ints
    noisyStripsSpec.extend('occupancy', cool.StorageType.Blob64k) # Array of floats
else:
    noisyStripsSpec.extend('noisyList', cool.StorageType.String64k)

description = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'
noisyStripsFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, noisyStripsSpec)

print("Creating new folder")
noisyStripsFolder = db.createFolder('/SCT/DAQ/NoisyStrips', noisyStripsFolderSpec, description, True)

firstValid = iovlo
lastValid  = iovhi

numNoisyStrips = 0

for dictkey in HistDict:
    noisy = 0
    strip_array = array.array('i')
    occ_array = array.array('d')
    noisy_str = ''
    HistDict[dictkey].Scale(1.0/float(numEvents))
    for bini in range(768):
        if HistDict[dictkey].GetBinContent(bini+1) >= args.threshold:
            noisy += 1
            numNoisyStrips += 1
            strip_array.append(int(bini))
            occ_array.append(HistDict[dictkey].GetBinContent(bini+1))
            noisy_str += f' {int(bini):3}:{HistDict[dictkey].GetBinContent(bini+1):.5f}'

    # Done with loop over bins
    noisyStripsRecord = cool.Record(noisyStripsSpec)
    noisyStripsRecord['sensor'] = int(dictkey)
    noisyStripsRecord['nstrips'] = noisy
    if args.use_blob:
        strip_blob = noisyStripsRecord['strip']
        occ_blob = noisyStripsRecord['occupancy']
        if len(strip_array) > 0:
            strip_blob.resize(4*len(strip_array)) # Length in bytes
            occ_blob.resize(4*len(strip_array))
            strip_blob = strip_array.tobytes()
            occ_blob = occ_array.tobytes()

    else:
        noisyStripsRecord['noisyList'] = noisy_str

    noisyStripsFolder.storeObject(firstValid, lastValid, noisyStripsRecord, int(dictkey))

db.closeDatabase()

print('Database completed')
print(f"Added {numNoisyStrips} strips to database")
