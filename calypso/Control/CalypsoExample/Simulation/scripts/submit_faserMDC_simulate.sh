#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faserMDC_simluate.sh [--shift] input_file output_file [release_directory] [working_directory] [skip] [nevts]
#
# Options:
# --shift - apply crossing angle (and FASER shift)
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
# 
# input_file - full file name (with path)
# output_file - full output file name
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
# skip - events in input file to skip
# nevts = events in input file to process
#
# The release directory must already be set up 
# (so an unqualified asetup can set up the release properly)
#
# Script will use git describe to find the release tag.  
# If this matches gen/g???? or sim/s???? it will be passed to the job
#
#----------------------------------------
# Keep track of time
SECONDS=0
#
# Parse command-line options
while [ -n "$1" ]
do 
  case "$1" in
      -s | --shift) 
	  echo "Applying crossing-angle shift" 
	  xangle=1 
	  shift;;  # This 'eats' the argument

      -l | --log)
	  logdest="$2";
	  shift;
	  shift;; # Must eat 2 options here

      -o | --out)
	  outdest="$2";
	  shift;
	  shift;;

      --) # End of options
	  shift; # Eat this
	  break;; # And stop parsing

      -*) 
	  echo "Unknown option $1"
	  shift;;

      *) break;;  # Not an option, don't shift
  esac
done

#
# Parse command-line arguments
infile=${1}
outfile=${2}
release_directory=${3}
working_directory=${4}
skip_events=${5}
nevts=${6}
#
# Set defaults if arguments aren't provided
if [ -z "$infile" ]
then
  echo "No input file specified!"
  echo "Usage: submit_faserMDC_simulate.sh input_file output_file [release dir] [output dir]"
  exit 1
fi
#
if [ -z "$outfile" ]
then
  outfile="FaserMC-Test-123456-00000-HITS.root"
  echo "No output file specified, using $outfile !"
fi
#
if [ -z "$release_directory" ]
then
  release_directory=`pwd`
fi
#
if [ -z "$working_directory" ]
then
  working_directory=`pwd`
fi
#
if [ -z "$skip_events" ]
then
  skip_events=0
fi
#
if [ -z "$nevts" ]
then
  nevts=-1
fi
#
starting_directory=`pwd`
#
# Now extract the file information
# Here we do this on the output file, as the input files can be non-standard
#
# First, get the filename
outfilename=$(basename "$outfile")
# 
# Now split based on '.' to get stem
defaultIFS=$IFS
IFS='.'
read file_stem ext <<< "$outfilename"
#
# Try to find the run number
IFS='-'
# Read the split words into an array based on delimeter
read faser short run_number seg <<< "$file_stem"
#
# Set the IFS delimeter back or else echo doesn't work...
IFS=$defaultIFS
#
# Check if we found a number, use full input file name if not
output_directory="$working_directory/${run_number}"
re='^[0-9]+$'
if ! [[ $run_number =~ $re ]] ; then
    # Not a number...
    output_directory="$working_directory/${file_stem}"
fi
#
# Make output directory if needed
mkdir -p "$output_directory"
#
# This magic redirects everything in this script to our log file
logfile=${file_stem}.sim.log
exec >& "$output_directory/$logfile"
echo `date` - $HOSTNAME
echo "Input File: $infile"
echo "Output File: $outfilename"
echo "Release: $release_directory"
echo "Output: $output_directory"
echo "Starting: $starting_directory"
echo "Skip: $skip_events"
echo "Nevts: $nevts"
#
# Set up the release (do this automatically)?
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh 
#
# Try automatic
# Always go back to the starting directory in case paths are relative
cd "$starting_directory"
cd "$release_directory"
# This doesn't seem to work, as we need the --input argument
#asetup 
#source build/x8*/setup.sh
#
# Do this by hand
asetup --input=calypso/asetup.faser Athena,22.0.49
source build/x86*/setup.sh
#
# Move to the run directory
cd "$starting_directory"
cd "$output_directory"
#
# Remove any previous directory if it exists
#if [[ -e "$file_stem" ]]; then
#    echo "Remove previous directory $file_stem"
#    rm -rf "$file_stem"
#fi
#
# Make run directory
if [[ -e "${file_stem}" ]]; then
    echo "Directory ${file_stem} already exists"
else
    mkdir "${file_stem}"
fi
cd "${file_stem}"
#
# Run job
#if [[ -z "$tag" ]]; then
#fi
if [[ -z "$xangle" ]]; then
    faserMDC_simulate.py  --skip "$skip_events" -n "$nevts" "$infile" "$outfile"
else
    faserMDC_simulate.py  --yangle -0.000150 --yshift 12.0 --skip "$skip_events" -n "$nevts" "$infile" "$outfile"
fi
#
# Print out ending time
date
echo "Job finished after $SECONDS seconds"
#
# Copy output to EOS if desired
export EOS_MGM_URL=root://eospublic.cern.ch
#
if ! [ -z "$outdest" ]
then
    ls -l
    echo "copy *-HITS.root to $outdest"
    mkdir -p $outdest
    eos cp *-HITS.root ${outdest}/ || true
fi
#
# Also copy log file
if ! [ -z "$logdest" ]
then
    cd ..
    ls -l
    echo "copy $logfile to $logdest"
    mkdir -p $logdest
    eos cp $logfile $logdest/$logfile
elif ! [ -z "$outdest" ]
then 
    cd ..
    ls -l
    echo "copy $logfile to $outdest"
    mkdir -p $outdest
    eos cp $logfile $outdest/$logfile
fi

