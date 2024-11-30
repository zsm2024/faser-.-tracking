#!/usr/bin/env python3
#
# Sept 2022, E. Torrence
#
# Script to merge individual run DBs into a master DB
#
# Usage:
#  ./mergeNoisyDBRuns.py -h
#
import sys
import argparse
import subprocess

from pathlib import Path

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

def parse_arguments():

    description="Script to merge DBs from individual runs into one DB\n"
    parser = argparse.ArgumentParser(description,
                                     formatter_class=argparse.RawTextHelpFormatter)


    parser.add_argument("runs", nargs='+', help="Specify FASER runs or range")
    parser.add_argument("--threshold", default="0.01", help="Threshold to calls trip noisy (0.01)")
    parser.add_argument("-v", "--verbose", action="store_true", help="Debugging output")
    return parser.parse_args()

# Take a string and turn it into a list of integers
# Can specify single values, ranges, or comma separated lists of both
def parseRunList(runlist):

    run_list = []

    # Check if this is a file with run numbers
    if len(runlist) == 1:
        path = Path(runlist[0])
        if path.exists() and path.is_file():
            print(f"Reading runs from {path}")
            # Try reading each line as a run number
            with path.open() as f: 
                for line in f.readlines():
                    line = line.strip()
                    if len(line) == 0: continue
                    if line[0] in ['#', '!']: continue
                    if not line.isnumeric():
                        print(f"Error parsing {line}")
                        continue
                    run_list.append(int(line))
            # Done reading file
            return(run_list)
        elif '-' in runlist[0]:
            pass
        elif ',' in runlist[0]:
            pass
        elif not runlist[0].isnumeric():
            print(f"File {path} doesn't exist!")
            return run_list

    for string in runlist:
        tokens = string.split(',')

        for segment in tokens:

            if len(segment) == 0: continue

            if '-' in segment:  # Range of runs
                start, end = segment.split('-')
                if not start.isnumeric():
                    print(f"Found invalid run {start}")
                    continue
                if not end.isnumeric():
                    print(f"Found invalid run {end}")
                    continue
                start = int(start)
                end = int(end)
                run_list.extend(list(range(int(start), int(end)+1)))

            else:
                if not segment.isnumeric():
                    print(f"Found invalid run {segment}")
                    continue
                run_list.append(int(segment))

    return(run_list)

class NoisyRunAnalyzer:

    def __init__(self, verbose=False, threshold=0.01):
        self.verbose = verbose

        self.run_dict = {}
        self.noise_threshold = threshold

    def addRun(self, runnum):

        if self.verbose: print(f"\nRun {runnum}")

        runstr = f'{runnum:06d}'
        infile = Path(f'{runstr}/noisy_{runstr}.db')
        if not infile.is_file():
            print(f"{runstr}/noisy_{runstr}.db doesn't exist!")
            return

        db_string = f'sqlite://;schema={runstr}/noisy_{runstr}.db;dbname=CONDBR3'
        try:
            self.db = indirectOpen(db_string, readOnly=True, oracle=False, debug=False)
        except Exception as e:
            print(e)
            return

        # Now read all channels
        folder_string = "/SCT/DAQ/NoisyStrips"
        try:
            self.folder = self.db.getFolder(folder_string)
        except Exception as e:
            print(e)
            return

        if self.folder is None:
            print(f"Can't access folder {folder_string} in {db_string}")
            return

        channels = cool.ChannelSelection.all()
        iov_lo = (runnum<<32)
        iov_hi = ((runnum+1)<<32) - 1
        tag = ''

        try:
            itr = self.folder.browseObjects(iov_lo, iov_hi, channels, tag)
        except Exception as e:
            print(e)
            return

        rd = self.run_dict.get(runnum, None)
        if rd is None:
            self.run_dict[runnum] = {}
            
        # Now iterate through objects (should only be one IOV, but multiple channels)
        while itr.goToNext():
            obj = itr.currentRef()
            if self.verbose: print(obj.payload())

            sensor = obj.payload()['sensor']
            nstrips = obj.payload()['nstrips']

            # Skip if this sensor has no noisy strips
            if nstrips == 0: continue

            sensor_dict = self.run_dict[runnum].get(sensor, None)
            if sensor_dict is None:
                self.run_dict[runnum][sensor] = {}

            # Now need to parse the noisy strip string
            noisyList = obj.payload()['noisyList']

            for item in noisyList.strip().split():

                strip = int(item.split(':')[0])
                occupancy = float(item.split(':')[1])

                if occupancy < self.noise_threshold: continue

                strip_dict = self.run_dict[runnum][sensor].get(strip, None)
                if strip_dict is None:
                    self.run_dict[runnum][sensor][strip] = {}

                self.run_dict[runnum][sensor][strip] = occupancy

            # Done looping over strips

        # Done looping over objects
        if self.verbose: print(self.run_dict)

        # Done, close the database
        self.db.closeDatabase()

    def printRunSummary(self):

        for run in self.run_dict:

            #print(f"Run {run}: {len(self.run_dict[run])} sensors with noisy strips")

            noisy_by_layer = [0] * 12
            noisy_strips_by_layer = [0] * 12

            for sensor in self.run_dict[run]:
                layer = sensor // 16
                noisy_by_layer[layer] += 1
                noisy_strips_by_layer[layer] += len(self.run_dict[run][sensor])

            #print(f"Sensors by layer: ", end='')
            #[ print(f' {n:3d}', end='') for n in noisy_by_layer]
            #print()

            print(f"Run {run} strips > {100*self.noise_threshold:3.1f}% by layer: ", end='')
            [ print(f' {n:3d}', end='') for n in noisy_strips_by_layer]
            print()


# Command-line execution
if __name__ == "__main__":

    # Parse the command-line arguments
    args = parse_arguments()

    run_list = parseRunList(args.runs)
    run_list.sort()

    nra = NoisyRunAnalyzer(verbose=args.verbose, threshold=float(args.threshold))

    for runnum in run_list:
        nra.addRun(runnum)

    nra.printRunSummary()

