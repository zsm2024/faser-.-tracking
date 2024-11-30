#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faser_particlegun.sh [script_options --] config_file segment [release_directory] [working_directory] [--other_options]
# 
# Script Options:
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
#
# config_file - full file name (with path)
# segment - segment number (file segment)
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
#
# Other Options will be passed directly to job script
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
while [ -n "$1" ]
do
    case "$1" in
	--) # Stop parsing
	    shift;  # Eat this
	    break;; # and stop parsing
	
	-*) # Some option
	    break;; # Stop parsing

	*)  # Not an option, fill next parameter value

	    if [ -z "$config_path" ]
	    then
		config_path=${1}
		shift
	    elif [ -z "$segment" ]
	    then
		segment=${1}
		shift
	    elif [ -z "$release_directory" ]
	    then
		release_directory=${1}
		shift
	    elif [ -z "$working_directory" ]
	    then
		working_directory=${1}
		shift
	    else
		break # Already filled everything
	    fi
	    ;;
    esac
done
#
# Set defaults if arguments aren't provided
if [ -z "$config_path" ]; then
  echo "No config_path specified!"
  echo "Usage: submit_faser_particlegun.sh config_file segment [release dir] [output dir]"
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
echo "Remaining: $@"
#
# Set up the release (do this automatically)?
# Must follow with -- to avoid command-line arguments being passed
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh --
#
# Always go back to the starting directory in case paths are relative
cd "$starting_directory"
cd "$release_directory"
#
# Do this by hand
asetup --input=calypso/asetup.faser Athena,22.0.49
source run/setup.sh
#
echo "ATLAS_POOLCOND_PATH = $ATLAS_POOLCOND_PATH"
#
#
# Try to find a release tag
cd calypso
gentag=`git describe --tags`
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
    tagstr=""
else
    tagstr="--reco=$tag"
fi
#
echo
echo "Submit job:"
echo faser_particlegun.py  "--conf=$config_path" "--segment=$seg_str" $tagstr "$@" 
echo
faser_particlegun.py  "--conf=$config_path" "--segment=$seg_str" $tagstr "$@" 
gen_code=$?
#
echo "Return code: $gen_code"
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
    echo "Output directory:"
    ls -l
    thefile=`ls *-HITS.root`
    if [ $? -eq 0 ]; then
	echo "copy $thefile to $outdest"
	eos mkdir -p $outdest
	eos cp $thefile ${outdest}/${thefile} || true

	# Check that it worked
	eos ls ${outdest}/${thefile} > /dev/null
	if [ $? -eq 0 ]; then
	    echo "file $thefile copied to $outdest"
	    copy_code=0
	else
	    echo "didnt find $thefile in $outdest !"
	    copy_code=1
	fi
    else
	echo "ls *-xAOD.root returned nothing!"
	copy_code=1
    fi

fi
#
# Also copy log file
if ! [ -z "$logdest" ]
then
    cd ..
    echo "Working directory:"
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

# Make sure to return an error is calypso failed
if [ $gen_code -ne 0 ] || [ $copy_code -ne 0 ]; then
  exit 1
else
  exit 0
fi
