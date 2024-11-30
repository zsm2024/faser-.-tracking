#!/usr/bin/env python3
#
# Sept 2022, E. Torrence
#
# Script to run noisy strip finder on a given run
# This creates condor submitssion scripts and a DAG
# and submits those to run the jobs
#
# Usage:
#  submitNoisyStripJobs.py -h
#
import os
import sys
import time
import argparse
import subprocess

from pathlib import Path

def parse_arguments():

    description="Script to submit jobs to find noisy strips"
    parser = argparse.ArgumentParser(description,
                                     formatter_class=argparse.RawTextHelpFormatter)


    parser.add_argument("runs", nargs='+', help="Specify FASER runs")

    parser.add_argument("--per_job", type=int, default=25, help="Specify maximum files per job (default: 25)")
    parser.add_argument("--release", default='.', help="Specify path to release directory (default: .)")
    parser.add_argument("--nosubmit", action="store_true", help="Don't submit jobs")
    parser.add_argument("--nocleanup", action="store_true", help="Don't cleanup output directory on completion")
    parser.add_argument("--queue", default="longlunch", help="Specify queue (longlunch=2h (default), workday=8h)")
    parser.add_argument("--rawdir", default="/eos/experiment/faser/raw/2022", 
                        help="Specify raw data directory (default: /eos/experiment/faser/raw/2022)")
    parser.add_argument("-n", "--nsubmit", default=100, help="Specify total number of jobs to submit at once (default: 100)")

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

    # Check some things
    rel_dir = Path(args.release)
    package_dir = rel_dir / Path("calypso/Tracker/TrackerRecAlgs/NoisyStripFinder")

    # Script to allow python scripts to be run in condor in the FASER environment
    env_exec = package_dir / Path("share/runFaserScript.sh")

    if not env_exec.exists():
        print(f"Can't find executable in release directory {args.release}")
        sys.exit(1)

    print(f"Start processing {len(run_list)} runs")

    nsubmitted = 0

    for run in run_list:
        print(f"Working on run {run}")

        runstr = f"{run:06d}"

        # Get file list
        raw_dir = Path(f'{args.rawdir}/{runstr}')
        file_list = list(raw_dir.glob("Faser-Physics*.raw"))

        # Now we need to decide what to do
        nraw = len(file_list)
        njobs = (nraw-1) // args.per_job + 1
        if njobs == 1:
            print(f"{nraw} raw files found, submitting {njobs} job")
        else:
            print(f"{nraw} raw files found, submitting {njobs} jobs")

        if njobs == 0: continue

        # Create a directory for this
        jobdir = Path(runstr)
        if jobdir.exists():
            print(f"Directory {jobdir} exists, deleting...")
            import shutil
            shutil.rmtree(jobdir.resolve())

        jobdir.mkdir(exist_ok=True)
        submit_list = []

        # Start the DAG file
        dagfile = jobdir / Path(f"noise_{runstr}.dag")
        with open(dagfile, 'w') as d:
            d.write(f"# Auto-generated DAG submission script for {runstr}\n")

        for job in range(njobs):

            jobstr = f"{job:03d}"
            if njobs == 1:
                jobname = f"noise_{runstr}"
            else:
                jobname = f"noise_{runstr}_{jobstr}"
            subfile = jobdir / Path(jobname+".sub")

            ilo = job * args.per_job 
            ihi = ilo + args.per_job 
            job_files = file_list[ilo:ihi]

            #
            # Generate a job submission script
            print(f"Writing {subfile}")
            submit_list.append(jobname)
            with open(subfile, "w") as f:
                f.write(f"# Auto-generated submission script for {jobname}\n")
                # Set the queue workday = 8h, longlunch = 2h might be enough
                f.write(f'+JobFlavour = "{args.queue}"\n') 
                f.write(f"executable = {env_exec.resolve()}\n")
                f.write(f"output = {jobdir.resolve()}/{jobname}.out\n")
                f.write(f"error = {jobdir.resolve()}/{jobname}.err\n")
                f.write(f"log = {jobdir.resolve()}/{jobname}.log\n")
                # No newline as we need to add input files
                f.write(f"arguments = --rel {rel_dir.resolve()} NoisyStripFinderJob.py --out {jobname}.root ")
                [f.write(f" {filename}") for filename in job_files]
                f.write("\n")
                f.write("queue")

            # Also add this to our DAG
            with open(dagfile, 'a') as d:
                d.write(f"JOB {jobname} {subfile.name}\n")
                # Also check that the histogram isn't empty
                # This can fix some file read errors
                d.write(f"SCRIPT POST {jobname} {env_exec.resolve()} --rel {rel_dir.resolve()} checkNoisyStripHist.py {jobname}.root $RETURN\n")

        # Done writing individual jobs

        # Add the merge job to the DAG
        with open(dagfile, 'a') as d:
            d.write(f"JOB merge_{runstr} merge_{runstr}.sub\n")
            d.write("PARENT")
            for jobname in submit_list:
                d.write(f" {jobname}")
            d.write(f" CHILD merge_{runstr}\n")
            # Add a retry directive
            d.write(f"RETRY ALL_NODES 1\n")

        # Write the merge job submit script
        jobname = f"merge_{runstr}"
        subfile = jobdir / Path(jobname+".sub")
        with open(subfile, "w") as f:
            f.write(f"# Auto-generated submission script for {jobname}\n")
            f.write(f"output = {jobdir.resolve()}/{jobname}.out\n")
            f.write(f"error = {jobdir.resolve()}/{jobname}.err\n")
            f.write(f"log = {jobdir.resolve()}/{jobname}.log\n")

            #f.write('+JobFlavour = "workday"\n') # 8 hours, longlunch might be enough
            #f.write(f"executable = {hist_exec.resolve()}\n")
            f.write(f"executable = {env_exec.resolve()}\n")

            # No newline as we need to add input files
            # f.write(f"arguments = --rel {rel_dir.resolve()} --force -o noisy_{runstr}.db")
            f.write(f"arguments = --rel {rel_dir.resolve()} makeNoisyStripDB.py --force -o noisy_{runstr}.db")
            [f.write(f" {filename}.root") for filename in submit_list]
            f.write("\n")

            # Provide files to transfer
            f.write(f"transfer_input_files = {submit_list[0]}.root")
            [f.write(f",{filename}.root") for filename in submit_list[1:]]
            f.write("\n")
            f.write(f"should_transfer_files = IF_NEEDED\n")

            # Don't forget queue command
            f.write("queue")

        # Do we want a cleanup script?
        if not args.nocleanup:
            with open(dagfile, 'a') as d:
                d.write(f"SCRIPT POST merge_{runstr} cleanup.sh $RETURN\n")

            cleanup_file = jobdir / Path("cleanup.sh")
            with open(cleanup_file, 'w') as f:
                f.write("#!/bin/bash\n")
                f.write('if [[ $1 != "0" ]]; then\n')
                f.write(' exit $1\n')
                f.write('fi\n')
                # Removing files used by the DAG will make this hang...
                # f.write('rm noise_{runstr}.dag.* \n')
                # f.write('rm *.log\n')
                # f.write('rm *.err\n')
                f.write('rm eventLoopHeartBeat.txt\n')
                f.write('rm *.cc\n')
                for job in submit_list:
                    f.write(f'gzip {job}.out\n')
                #f.write('gzip merge*.out\n')
                f.write('exit 0\n')

            # And make it executable
            import stat
            cleanup_file.chmod(cleanup_file.stat().st_mode | stat.S_IEXEC)

        if not args.nosubmit:
            print(f"Submitting noise_{runstr}.dag")
            startdir = os.getcwd()
            os.chdir(jobdir)

            # lxplus python3 is 3.6.8, so use old subprocess.run arguments
            proc = subprocess.run(["/usr/bin/condor_submit_dag", f"noise_{runstr}.dag"],
                                  stdout=subprocess.PIPE,
                                  stderr=subprocess.PIPE,
                                  universal_newlines=True)

            if len(proc.stdout) > 0:
                print(proc.stdout)
            if len(proc.stderr) > 0:
                print(proc.stderr)

            # Chaange back to our starting point
            os.chdir(startdir)

            nsubmitted += 1

            # Don't flood the system, wait a few seconds here
            time.sleep(5)

            # Have we reached our limit?
            if nsubmitted >= int(args.nsubmit):
                print(f"Quitting after submitting {nsubmitted} jobs")
                print("Change this with --nsubmit option")
                break
        
