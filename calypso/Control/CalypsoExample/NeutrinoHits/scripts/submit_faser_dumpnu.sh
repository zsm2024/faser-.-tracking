#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faser_dumpnu.sh filepath [release_directory] [working_directory]
#
# Options:
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
# 
# filepath - full file name (with path)
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
#
# The release directory must already be set up 
# (so an unqualified asetup can set up the release properly)
#
# Script will use git describe to find the release tag.  
# If this matches sim/s???? or digi/d???? it will be passed to the job
#
#----------------------------------------
# Keep track of time
SECONDS=0
#
# Parse command-line options
while [ -n "$1" ]
do 
  case "$1" in
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
# Parse command-line options
file_path=${1}
release_directory=${2}
working_directory=${3}
#
# Set defaults if arguments aren't provided
if [ -z "$file_path" ]
then
  echo "No file specified!"
  echo "Usage: submit_faser_dumpnu.sh file [release dir] [output dir]"
  exit 1
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
starting_directory=`pwd`
#
# Now extract the run number and file stem
#
# First, get the filename
file_name=$(basename "$file_path")
# 
# Now split based on '.' to get stem
defaultIFS=$IFS
IFS='.'
read file_stem ext <<< "$file_name"
#
# Finally extract the run number
IFS='-'
# Read the split words into an array based on delimiter
read faser short run_number segment <<< "$file_stem"
#
# Set the IFS delimeter back or else echo doesn't work...
IFS=$defaultIFS
#
# Make output directory if needed
output_directory="$working_directory/$run_number"
mkdir -p "$output_directory"
#
# This magic redirects everything in this script to our log file
logfile="${file_stem}.ntp.log"
exec >& "${output_directory}/${logfile}"
echo `date` - $HOSTNAME
echo "File: $file_name"
echo "Release: $release_directory"
echo "Output: $output_directory"
echo "Starting: $starting_directory"
#
# Set up the release (do this automatically)?
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh 
#
# Always go back to the starting directory in case paths are relative
cd "$starting_directory"
cd "$release_directory"
#
# Do this by hand
asetup --input=calypso/asetup.faser Athena,22.0.49
source run/setup.sh
#
# Try to find a release tag
cd calypso
recotag=`git describe --tags`
if [[ "$recotag" == "reco/r"???? ]]; then
  tag=`echo "$recotag" | cut -c 6-11`
  echo "Found reco tag: $tag"
fi
if [[ "$recotag" == "digi/d"???? ]]; then
  tag=`echo "$recotag" | cut -c 6-11`
  echo "Found digi tag: $tag"
fi
if [[ "$recotag" == "sim/s"???? ]]; then
  tag=`echo "$recotag" | cut -c 5-10`
  echo "Found sim tag: $tag"
fi
#
if [[ -z "$tag" ]]; then
    tagstr=""
else
    tagstr="--tag $tag"
fi
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
if [[ -e "$file_stem" ]]; then
    echo "Directory $file_stem already exists"
else
    mkdir "$file_stem"
fi
cd "$file_stem"
#
# Run job
#
faser_dumpnu.py $tagstr "$file_path"
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
    echo "copy *-NTUP.root to $outdest"
    mkdir -p $outdest
    eos cp *-NTUP.root ${outdest}/ || true
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
