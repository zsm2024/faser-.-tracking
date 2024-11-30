#!/usr/bin/env python3
#
# Sept 2022, E. Torrence
#
# Script to find FASER runs taken in Physics
#
# Usage:
#  ./findRuns.py -h
#
import json
import argparse
import requests

from pathlib import Path

def parse_arguments():

    description="Script to find PHYSICS runs in a range\n"
    parser = argparse.ArgumentParser(description,
                                     formatter_class=argparse.RawTextHelpFormatter)


    parser.add_argument("runs", nargs='+', help="Specify FASER runs or range")

    parser.add_argument("-v", "--verbose", action="store_true", help="Debugging output")
    parser.add_argument("-o", "--output", default="findRuns.txt", help="Specify output file")
    parser.add_argument("-t", "--type", default="Physics", help="Run type to match")
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

    for string in runlist:
        tokens = string.split(',')

        for segment in tokens:

            if len(segment) == 0: continue

            if '-' in segment:  # Range of runs
                start, end = segment.split('-')
                start = int(start)
                end = int(end)
                run_list.extend(list(range(int(start), int(end)+1)))

            else:
                run_list.append(int(segment))

    return(run_list)

# Command-line execution
if __name__ == "__main__":

    # Parse the command-line arguments
    args = parse_arguments()

    run_list = parseRunList(args.runs)
    run_list.sort()

    with open(args.output, "w") as f:
        f.write(f"# findRuns.py")
        [f.write(f" {run}") for run in args.runs]
        f.write("\n")

    # faser-runinfo address
    url = "https://faser-runinfo.app.cern.ch/cgibin/"

    # Cycle through range
    for run in run_list:
        query = f"{url}/getRunInfo.py?runno={run}"
        response = requests.get(query)
        
        if not response.json():
            if args.verbose:
                print(f"Couldn't find run {run}")

            continue

        run_type = response.json()['type']
        if args.verbose:
            print(f"Run {run} has type {run_type}")
        if run_type != args.type: continue

        with open(args.output, "a") as f:
            f.write(f"{run}\n")

    # End of loop over runs
