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

def parse_arguments():

    description="Script to merge DBs from individual runs into one DB\n"
    parser = argparse.ArgumentParser(description,
                                     formatter_class=argparse.RawTextHelpFormatter)


    parser.add_argument("runs", nargs='+', help="Specify FASER runs or range")

    parser.add_argument("-v", "--verbose", action="store_true", help="Debugging output")
    parser.add_argument("-a", "--append", action="store_true", help="Append (rather than overwrite) existing file")
    parser.add_argument("-o", "--output", default="noisy_strips.db", help="Specify output DB name")
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


# Command-line execution
if __name__ == "__main__":

    # Parse the command-line arguments
    args = parse_arguments()

    run_list = parseRunList(args.runs)
    run_list.sort()

    first = True

    for runnum in run_list:

        if args.verbose: print(f"\nRun {runnum}")
        runstr = f'{runnum:06d}'
        infile = Path(f'{runstr}/noisy_{runstr}.db')
        if not infile.is_file():
            print(f"{runstr}/noisy_{runstr}.db doesn't exist!")
            continue

        command = ['AtlCoolCopy']
        command.append(f'sqlite://;schema={runstr}/noisy_{runstr}.db;dbname=CONDBR3')
        command.append(f'sqlite://;schema={args.output};dbname=CONDBR3')
        if first:
            first = False
            target = Path(args.output)
            if not target.is_file():
                print(f"Creating file {args.output}")
                command.append("-create")
            elif args.append:
                print(f"Appending to existing file {args.output}")
            else:
                print(f"Deleting existing file {args.output}")
                target.unlink()
                command.append("-create")

        command.extend(["-alliov", "-nrls", f"{runnum}", "0"])

        if args.verbose: print(command)
        rc = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, 
                                universal_newlines=True)

        if args.verbose: print(rc.stdout)

