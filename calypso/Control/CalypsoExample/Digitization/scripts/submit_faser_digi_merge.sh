#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faser_digi_merge.sh [--script_options --] dirpath slice nfiles [release_directory] [working_directory] [--other_options]
#
# Script Options:
#   --digiTag <tag> - override digitization tag for calo gain
#   --partial - allow missing files in merge
#   --geom - geometry setting
#   --out - specify output location (in EOS) to copy output HITS file
#   --log - specify output location (in EOS) for log file
# 
# dirpath - full directory path to HITS files
# slice - ordinal output file number
# nfiles - number of HITS files to process per slice
# release_directory - optional path to release install directory (default pwd)
# working_directory - optional path to output directory location (default pwd)
#
# Other Options will be passed to job script
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
# Job options strings
gainstr=""
timestr=""
partialstr=""
geomstr=""
#
# Parse command-line options
while [ -n "$1" ]
do 
  case "$1" in
      --digiTag)
	  echo "Override calo digi tag with $2"
	  gainstr="--digiTag $2"
	  shift;
	  shift;;

      --subtractTime)
	  echo "Subtract $2 ns from SCT RDOs"
	  timestr="--subtractTime $2"
	  shift;
	  shift;;

      --partial)
	  echo "Allowing partial merge"
	  partialstr="--partial"
	  shift;;

      -g | --geom)
	  geomstr="--geom $2";
	  shift;
	  shift;;

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
while [ -n "$1" ]
do
    case "$1" in
        --) # Stop parsing
            shift;  # Eat this
            break;; # and stop parsing

	-*) # Some option                                          
            break;; # Stop parsing

        *)  # Not an option, fill next parameter value

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
  echo "Usage: submit_faser_digi_merge.sh directory slice nfiles [release dir] [output dir]"
  exit 1
fi
#
if [ -z "$slice" ]
then
  echo "Slice number not specified!"
  echo "Usage: submit_faser_digi_merge.sh directory slice nfiles [release dir] [output dir]"
  exit 1
fi
#
if [ -z "$nfiles" ]
then
  echo "Files per slice not specified!"
  echo "Usage: submit_faser_digi_merge.sh directory slice nfiles [release dir] [output dir]"
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
file_stem="$faser-$short-$run_number-RDO-merge-$slice"
#
# This magic redirects everything in this script to our log file
logfile="${file_stem}.rdo.log"
exec >& "$output_directory/$logfile"
echo `date` - $HOSTNAME
echo "Directory: $dir_path"
echo "Geom: $geomstr"
echo "Gain: $gainstr"
echo "Time: $timestr"
echo "Slice: $slice"
echo "NFiles: $nfiles"
echo "Release: $release_directory"
echo "Output: $output_directory"
echo "Starting: $starting_directory"
echo "job: $file_stem"
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
echo
echo "Submit job:"
echo faser_digi_merge.py $partialstr $geomstr $gainstr $timestr $tagstr --slice $slice --files $nfiles "$@" $dir_path
echo
#
faser_digi_merge.py $partialstr $geomstr $gainstr $timestr $tagstr --slice $slice --files $nfiles "$@" $dir_path
digi_code=$?
echo "Return code: $digi_code"
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
    thefile=`ls *-RDO.root`
    if [ $? -eq 0 ]; then
        echo "copy $thefile to $outdest"
	eos mkdir -p $outdest
	eos cp $thefile ${outdest}/${thefile} || true

	# Check that this worked
	eos ls ${outdest}/${thefile} > /dev/null
        if [ $? -eq 0 ]; then
            echo "file $thefile copied to $outdest"
	    copy_code=0
	else
	    echo "didnt find $thefile in $outdest !"
	    copy_code=1
	fi
    else
        echo "ls *-RDO.root returned nothing!"
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
    echo "Working directory:"
    ls -l
    echo "copy $logfile to $outdest"
    eos mkdir -p $outdest
    eos cp $logfile $outdest/$logfile
fi

# Make sure to return an error is calypso failed                                
if [ $digi_code -ne 0 ] || [ $copy_code -ne 0 ]; then
  exit 1
else
  exit 0
fi

