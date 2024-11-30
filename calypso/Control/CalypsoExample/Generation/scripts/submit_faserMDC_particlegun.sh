#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faserMDC_particlegun.sh config_file segment [release_directory] [working_directory] 
# 
# Options:
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
#
# config_file - full file name (with path)
# segment - segment number (file segment)
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
#
# Afterwards, the output file will be copied to the directory specified in working_directory
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
# Parse options
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

      *) break;; # Not an option, don't shift
  esac
done
#
# Parse command-line options
config_path=${1}
segment=${2}
release_directory=${3}
working_directory=${4}
#
# Set defaults if arguments aren't provided
if [ -z "$config_path" ]; then
  echo "No config_path specified!"
  echo "Usage: submit_faserMDC_particlegun.sh config_file segment [release dir] [output dir]"
  exit 1
fi
# Check if relative path (only works run interactively)
if ! [[ ${config_path::1} == "/" ]]; then 
    echo "config_path should be absolute!"
    config_path=`pwd`/${1}
    echo "Using: $config_path"
fi
#
if [ -z "$segment" ]; then
  segment=0
fi
#
if [ -z "$release_directory" ]; then
  release_directory=`pwd`
fi
#
if [ -z "$working_directory" ]; then
  working_directory=`pwd`
fi
# 
# Apply padding to segment number
printf -v seg_str "%05d" $segment
#
starting_directory=`pwd`
#
# Now extract the file stem
#
# First, get the filename
config_file=$(basename "$config_path")
# 
# Now split based on '.' to get stem
defaultIFS=$IFS
IFS='.'
read config_file_stem ext <<< "$config_file"
#
# Try to find the run number
IFS='-'
# Read the split words into an array based on delimeter
read faser short run_number <<< "$config_file_stem"
#
# Set the IFS delimeter back or else echo doesn't work...
IFS=$defaultIFS
#
# Check if we found a number, use full config name if not
output_directory="$working_directory/${run_number}"
re='^[0-9]+$'
if ! [[ $run_number =~ $re ]] ; then
    # Not a number...
    output_directory="$working_directory/${config_file_stem}"
fi
#
# Make output directory if needed
mkdir -p "$output_directory"
#
# This magic redirects everything in this script to our log file
logfile="${config_file_stem}-${seg_str}.gen.log"
exec >& "$output_directory/${logfile}"
echo `date` - $HOSTNAME
echo "File: $config_file"
echo "Segment: $seg_str"
echo "Release: $release_directory"
echo "Output: $output_directory"
echo "Starting: $starting_directory"
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
#
# Try to find a release tag
cd calypso
gentag=`git describe`
if [[ "$gentag" == "gen/g"???? ]]; then
  tag=`echo "$gentag" | cut -c 5-10`
  echo "Found gen tag: $tag"
fi
if [[ "$gentag" == "sim/s"???? ]]; then
  tag=`echo "$gentag" | cut -c 5-10`
  echo "Found sim tag: $tag"
fi
if [[ "$gentag" == "digi/d"???? ]]; then
  tag=`echo "$gentag" | cut -c 6-11`
  echo "Found digi tag: $tag"
fi
if [[ "$gentag" == "reco/r"???? ]]; then
  tag=`echo "$gentag" | cut -c 6-11`
  echo "Found reco tag: $tag"
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
if [[ -e "${config_file_stem}-${seg_str}" ]]; then
    echo "Directory ${config_file_stem}-${seg_str} already exists"
else
    mkdir "${config_file_stem}-${seg_str}"
fi
cd "${config_file_stem}-${seg_str}"
#
# Run job
if [[ -z "$tag" ]]; then
    faserMDC_particlegun.py  "--conf=$config_path" "--segment=$seg_str"
else
    faserMDC_particlegun.py  "--conf=$config_path" "--segment=$seg_str" "--tag=$tag"
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
    eos mkdir -p $outdest 
    eos cp *-HITS.root ${outdest}/ || true
fi
#
# Also copy log file
if ! [ -z "$logdest" ]
then
    cd ..
    ls -l
    echo "copy $logfile to $logdest"
    eos mkdir -p $logdest 
    eos cp $logfile $logdest/$logfile
elif ! [ -z "$outdest" ]
then 
    cd ..
    ls -l
    echo "copy $logfile to $outdest"
    eos mkdir -p $outdest
    eos cp $logfile $outdest/$logfile
fi
