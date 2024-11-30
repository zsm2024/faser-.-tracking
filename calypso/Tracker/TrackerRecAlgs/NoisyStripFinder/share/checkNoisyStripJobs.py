#!/usr/bin/env python3
#
# Sept 2022, E. Torrence
#
# Script to check NoisyStrips jobs for problems
#
# Usage:
#  ./checkNoisyJobs.py -h
#
import sys
import argparse
from pathlib import Path

import ROOT

def parse_arguments():

    description="Script to check noisy strip finding jobs\n"
    parser = argparse.ArgumentParser(description,
                                     formatter_class=argparse.RawTextHelpFormatter)


    parser.add_argument("runs", nargs='+', help="Specify FASER runs or range")

    parser.add_argument("-v", "--verbose", action="store_true", help="Debugging output")
    parser.add_argument("--write_missing", action="store_true", help="Write out missing runs to file")

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

class JobChecker:

    def __init__(self, run=None):
        self.select_run(run)

        self.verbose = True
        self.check_all = False

        self.check_function_list = []
        self.check_function_list.append(self.check_directory)
        self.check_function_list.append(self.check_histograms)
        self.check_function_list.append(self.check_dbfile)

    def select_run(self, run):
        self.run_number = run
        if run is None: return
        self.run_string = f'{run:06d}'

    def check_run(self, run=None):
        # Return true on error

        if run is not None:
            self.select_run(run)

        if run is None:
            print("No run specified!")
            return True

        for func in self.check_function_list:
            if func(): return True

        return False

    def check_directory(self):

        directory_path = Path(self.run_string)
        if not directory_path.exists():
            print(f"* Directory {self.run_string} not found!")
        elif self.verbose:
            print(f" => Directory {self.run_string} found")
        return( not directory_path.exists() )

    def check_dbfile(self):

        dbfile_path = Path(f'{self.run_string}/noisy_{self.run_string}.db')
        if not dbfile_path.exists():
            print(f"* Database file {dbfile_path} not found!")
        elif self.verbose:
            print(f" => Database file {dbfile_path} found")
        return( not dbfile_path.exists() )

    def check_histograms(self):

        # First, find the submit files
        directory_path = Path(self.run_string)

        submit_list = directory_path.glob('noise*.sub')

        missing = False

        for filepath in submit_list:

            # Ignore DAG
            if '.dag.' in str(filepath): continue

            filestem = filepath.stem
            hist_file = directory_path / Path(f'{filestem}.root')
            if hist_file.exists():
                if self.verbose: print(f" => Found histogram file {hist_file}")

                # Check number of events?
                try:
                    f = ROOT.TFile.Open(str(hist_file), 'r')
                except Exception as e:
                    print(e)
                    missing = True
                else:
                    n = f.Get("numEvents").GetVal()
                    if self.verbose:
                        print(f"{hist_file} found with {n} entries")
                    if n == 0:
                        print(f"{hist_file} found with {n} entries")
                        missing = True

                continue

            # Histogram doesn't exist
            missing = True
            print(f"* Histogram file {hist_file} missing! ", end="")

            # See if we can figure out why
            logfile_path = directory_path / Path(f'{filestem}.log')
            if not logfile_path.exists():
                print("=> log file not found") 
                continue

            import subprocess
            if subprocess.call(['/bin/grep', "Killed", f"{logfile_path}"], 
                               stdout=subprocess.DEVNULL):
                # True means no match
                pass

            else:
                # False means match
                # See if we can find the time
                rc = subprocess.run(['/bin/grep', 'Job finished after', f"{logfile_path}"],
                                    stdout=subprocess.PIPE,
                                    universal_newlines=True)

                if rc.returncode:
                    # Can't find running time
                    print("=> job appears to have been killed")
                else:
                    timestr = rc.stdout.replace('Job finished after ', '')
                    print(f"=> job appears to have been killed after {timestr}")
                continue

            # Can't figure out why
            print('=> unknown problem')

        return missing

# Command-line execution
if __name__ == "__main__":

    # Parse the command-line arguments
    args = parse_arguments()

    run_list = parseRunList(args.runs)
    run_list.sort()

    good_runs = []
    missing_runs = []

    jc = JobChecker()
    jc.verbose = args.verbose

    for runnum in run_list:

        if args.verbose: print(f"\nRun {runnum}")

        if jc.check_run(runnum):
            missing_runs.append(runnum)
        else:
            good_runs.append(runnum)

    print(f"Found {len(good_runs)} good runs and {len(missing_runs)} missing runs")
    if args.write_missing:
        missing_file="missing_runs.txt"
        with open(missing_file, "w") as f:
            [f.write(f"{run}\n") for run in missing_runs]
        print(f"Wrote {len(missing_runs)} missing runs to {missing_file}")
