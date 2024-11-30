#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faser_reco.sh [options --] file_path [release_directory] [working_directory] [nevents] --other_options
# 
# Options:
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
#   --geom - specify geometry
#
# Monte Carlo options:
#   --isMC - needed for MC reco
#   --caloTag <tag> - override MC reco tag for calo gain (to match digi tag)
#
# Other --options will be passed to faser_reco.py
# 
# file_path - full file name (with path)
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
# nevents - optional number of events to process (default: -1 - all)
#
# The release directory must already be set up 
# (so an unqualified asetup can set up the release properly)
#
# Script will use git describe to find the release tag.  
# If this matches reco/r???? it will be passed to the reco job
#
#----------------------------------------
# Keep track of time
SECONDS=0
#
# Job option strings
gainstr=""
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

      -g | --geom)
	  geom="$2";
	  shift;
	  shift;;

      --isMC)
	  echo "Set isMC true"
	  ismc=1
	  shift;;

      --caloTag)
	  echo "Override calo digi tag with $2"
	  gainstr="--MC_calibTag $2"
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
while [ -n "$1" ]
do
    case "$1" in
     --) # Stop parsing
	 shift; # Eat this
	 break;; # and stop parsing

     -*) # Some option
	 break;; # Stop parsing

     *)  # Not an option, fill next value

	 if [ -z "$file_path" ]
	 then
	     file_path=${1}
	     shift
	 elif [ -z "$release_directory" ]
	 then
	     release_directory=${1}
	     shift
	 elif [ -z "$working_directory" ]
	 then
	     working_directory=${1}
	     shift
	 elif [ -z "$nevents" ]
	 then
	     nevents=${1}
	     shift
	 else
	     break # Already filled everything
	 fi
	 ;;
    esac
done
#
# Set defaults if arguments aren't provided
if [ -z "$file_path" ]
then
  echo "No file_path specified!"  
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
if [ -z "$nevents" ]
then
  nevents="-1"
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
read faser type run_number segment <<< "$file_stem"
#
# Set the IFS delimeter back or else echo doesn't work...
IFS=$defaultIFS
#
# Make output directory if needed
output_directory="$working_directory/${run_number}"
mkdir -p "$output_directory"
#
# This magic redirects everything in this script to our log file
logfile="${file_stem}.rec.log"
exec >& "$output_directory/$logfile"
echo `date` - $HOSTNAME
echo "File: $file_name"
echo "Filepath: $file_path"
echo "Geom: $geom"
echo "Gain: $gainstr"
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
#asetup --input=calypso/asetup.faser Athena,22.0.49
asetup --input=calypso/asetup.faser Athena,24.0.41
source run/setup.sh
#
echo "Initial ATLAS_POOLCOND_PATH = $ATLAS_POOLCOND_PATH"
#
# Check if there are data overrides
if [ -d "run/data/sqlite200" ]; then
   cond_directory=`pwd -P`/run/data  # Get absolute path
fi
#
# Try to find a release tag
cd calypso
recotag=`git describe --tags`
if [[ "$recotag" == "reco/r"???? ]]; then
  tag=`echo "$recotag" | cut -c 6-11`
  echo "Found reco tag: $tag"
fi
if [[ "$recotag" == "reco/p"???? ]]; then
  tag=`echo "$recotag" | cut -c 6-11`
  echo "Found proc tag: $tag"
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
# Check if there were data overrides in the release directory
if [[ -z "$cond_directory" ]]; then
    echo "No local conditions directory found!"
else
    echo "Local conditions directory found!  Copying to run directory..."
    echo Copying $cond_directory
    cp -r $cond_directory .
    ls -R data
fi
# 
# Further check if there is a pool conditions override
if [[ -d "data/poolcond" ]]; then
    echo "Local POOL directory found!"
    echo "Change ATLAS_POOLCOND_PATH"
    echo " from $ATLAS_POOLCOND_PATH"
    export ATLAS_POOLCOND_PATH=`pwd -P`/data
    echo " to $ATLAS_POOLCOND_PATH"
else
    echo "No local pool files found, use default:"
    echo " $ATLAS_POOLCOND_PATH"
fi
echo "Final ATLAS_POOLCOND_PATH: $ATLAS_POOLCOND_PATH"
#
# Run job
if [[ -z "$tag" ]]; then
    tagstr=""
else
    tagstr="--reco=$tag"
fi
#
if [[ -z "$geom" ]]; then
    geomstr=""
else
    geomstr="--geom $geom"
fi
#
if [[ -z "$ismc" ]]; then
    mcstr=""
else
    mcstr="--isMC"
fi
#
echo
echo "Submit job:"
echo faser_reco.py "--nevents=$nevents" $geomstr $tagstr $mcstr $gainstr "$@" "$file_path"
echo
#
faser_reco.py "--nevents=$nevents" $geomstr $tagstr $mcstr $gainstr "$@" "$file_path" 
reco_code=$?
echo "Return code: $reco_code"
#
# Print out ending time
date
echo "Job finished after $SECONDS seconds"
# 
# Copy output to EOS if desired
export EOS_MGM_URL=root://eospublic.cern.ch
copy_code=0
#
# Now copy output file
if ! [ -z "$outdest" ]
then
    echo "Output directory:"
    ls -l
    thefile=`ls *-xAOD.root`
    if [ $? -eq 0 ]; then
	echo "copy $thefile to $outdest"
	eos mkdir -p $outdest
	# Keep this line from stopping script, so we might get a log file
	# || true ensures script continues even if copy fails
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
# Copy log file second
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
    echo "Working directory:"
    ls -l
    echo "copy $logfile to $outdest"
    eos mkdir -p $outdest
    eos cp $logfile $outdest/$logfile
fi

# Make sure to return an error is calypso failed
if [ $reco_code -ne 0 ] || [ $copy_code -ne 0 ]; then
  exit 1
else
  exit 0
fi

