#!/bin/bash
#
# Used with a condor file to submit a physics ntuple job
#
# Usage:
# submit_ntuple_maker.sh [options --] dirpath slice nfiles [release_directory] [working_directory] --other_options
#
# Options:
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
#   --isMC - needed for MC reco
#   --partial - allow missing files
#   --merge - merge factor of reco files (for MC)
#   --fluka - create fluka weights
#   --genie - create genie weights
#
# Other --option will be passed to faser_ntuple_maker.py
#
# dirpath - full directory path to HITS files
# slice - ordinal output file number
# nfiles - number of HITS files to process per slice
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
# lastfile - number of files in this slice (for last file, default 0)
#
# The release directory must already be set up 
# (so an unqualified asetup can set up the release properly)
#
#----------------------------------------
# Keep track of time
SECONDS=0
#
# Defaults
ismc=""
partialstr=""
mergestr=""
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

      --isMC)
	  ismc="--isMC"
	  shift;;

      --partial)
	  echo "Allowing partial merge"
	  partialstr="--partial"
	  shift;;

      --merge)
	  mergestr="--merge $2";
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
while [ -n "$1" ]
do
  case "$1" in
    --) # Stop parsing
      shift; # Eat this
      break;; # And stop parsing

    -*) # Some option
      break;; # Stop parsing

    *)  # Not an option, fill next value

      if [ -z "$dir_path" ] 
      then
	  dir_path=${1}
	  shift
      elif [ -z "$slice" ]
      then
	  slice=${1}
	  shift
      elif [ -z "$nfiles" ] 
      then
	  nfiles=${1}
	  shift
      elif [ -z "$release_directory" ]
      then
	  release_directory=${1}
	  shift
      elif [ -z "$working_directory" ]
      then
	  working_directory=${1}
	  shift
      elif [ -z "$last_file" ]
      then
	  last_file=${1}
	  shift
      else
	  break # Already filled everything
      fi
      ;;
  esac
done
#
# Set defaults if arguments aren't provided
if [ -z "$dir_path" ]
then
  echo "No directory specified!"
  echo "Usage: submit_ntuple_maker.sh directory slice nfiles [release dir] [output dir]"
  exit 1
fi
#
if [ -z "$slice" ]
then
  echo "Slice number not specified!"
  echo "Usage: submit_ntuple_maker.sh directory slice nfiles [release dir] [output dir]"
  exit 1
fi
#
if [ -z "$nfiles" ]
then
  echo "Files per slice not specified!"
  echo "Usage: submit_ntuple_maker.sh directory slice nfiles [release dir] [output dir]"
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
# First, get an example filename
file_name=`ls -1 $dir_path | head -1`
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
# Need to make up an output name
printf -v slicestr "%04d" $slice
#
file_stem="$faser-$short-$run_number-$slicestr-PHYS"
#
# This magic redirects everything in this script to our log file
logfile="${file_stem}.ntp.log"
exec >& "$output_directory/$logfile"
echo `date` - $HOSTNAME
echo "Directory: $dir_path"
echo "Slice: $slice"
echo "NFiles: $nfiles"
echo "Merge: $mergestr"
echo "Release: $release_directory"
echo "Output: $output_directory"
echo "Starting: $starting_directory"
echo "job: $file_stem"
echo "Last: $last_file"
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
echo "ATLAS_POOLCOND_PATH = $ATLAS_POOLCOND_PATH"
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
if [[ -z "$last_file" ]]; then
    last_file_str=""
else
    last_file_str="--last $last_file"
fi
#
# Move to the run directory
cd "$starting_directory"
cd "$output_directory"
#
# Make run directory
if [[ -e "$file_stem" ]]; then
    echo "Directory $file_stem already exists"
else
    mkdir "$file_stem"
fi
cd "$file_stem"
#
# Check if there are data overrides in the relese directory
if [[ -z "$cond_directory" ]]; then
    echo "No local conditions directory found!"
else
    echo "Local conditions directory found!  Copying to run directory..."
    echo Copying $cond_directory
    cp -r $cond_directory .
    ls -R data
fi
#
# Run job
#
echo
echo "Submit job:"
echo faser_ntuple_maker.py $last_file_str $partialstr $tagstr $ismc --slice $slice --files $nfiles $mergestr "$@" $dir_path 
echo
#
faser_ntuple_maker.py $last_file_str $partialstr $tagstr $ismc --slice $slice --files $nfiles $mergestr "$@" $dir_path 
ntup_code=$?
echo "Return code: $ntup_code"
#
# Print out ending time
date
echo "Job finished after $SECONDS seconds"
# 
# Copy output to EOS if desired
#
export EOS_MGM_URL=root://eospublic.cern.ch
#
if ! [ -z "$outdest" ]
then
    ls -l
    echo "copy *-PHYS.root to $outdest"
    mkdir -p $outdest
    eos cp *-PHYS.root ${outdest}/ || true
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

# Make sure to return an error if ntuple failed
exit $ntup_code
