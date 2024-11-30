#!/bin/bash
# Used with a condor file to submit to vanilla universe
#
# Usage:
# submit_faser_simluate.sh [--script_options --] input_file output_file [release_directory] [working_directory] [skip] [nevts] [--other_options]
#
# Script Options:
# --shift - apply crossing angle (and FASER shift)
#  --geom - specify geometry (default TI12)
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
# Other options will be passed to faser_simulate.py
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

      --fshift)
	  echo "Applying FLUKA crossing-angle shift"
	  xangle=2
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
# Parse command-line arguments
while [ -n "$1" ]
do
    case "$1" in
	--) # Stop parsing
	    shift; # Eat this
	    break;; # and stop parsing

	-*) # Some option
	    break;; # Stop parsing

	*)  # Not an option, fill next parameter value

	    if [ -z "$infile" ]
	    then
		infile=${1}
		shift
	    elif [ -z "$outfile" ]
	    then
		outfile=${1}
		shift
	    elif [ -z "$release_directory" ]
	    then
		release_directory=${1}
		shift
	    elif [ -z "$working_directory" ]
	    then
		working_directory=${1}
		shift
	    elif [ -z "$skip_events" ]
	    then
		skip_events=${1}
		shift
	    elif [ -z "$nevts" ]
	    then	    
		nevts=${1}
		shift
	    else
		break # Already filled everything
	    fi
	    ;;
    esac
done
#
# Set defaults if arguments aren't provided
if [ -z "$infile" ]
then
  echo "No input file specified!"
  echo "Usage: submit_faser_simulate.sh input_file output_file [release dir] [output dir]"
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
echo "Geom: $geomstr"
echo "Remaining: $@"
#
# Set up the release (do this automatically)?
# Must follow with -- to avoid command-line arguments being passed
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh --
#
# Try automatic
# Always go back to the starting directory in case paths are relative
cd "$starting_directory"
cd "$release_directory"
#
# Do this by hand
asetup --input=calypso/asetup.faser Athena,22.0.49
source run/setup.sh
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
#
if [[ -z "$xangle" ]]; then
    angle_string=""
elif [[ $xangle == 1 ]]; then
    echo "Applying standard crossing angle"
    angle_string="--yangle -0.000160 --yshift 12.0"
else
    echo "Applying double crossing-angle shift for FLUKA!"
    angle_string="--yangle -0.000310 --yshift 12.0"
fi
#
echo
echo "Submit job:"
echo faser_simulate.py $geomstr $angle_string "$@" --skip "$skip_events" -n "$nevts" "$infile" "$outfile"
echo
#
faser_simulate.py $geomstr $angle_string "$@" --skip "$skip_events" -n "$nevts" "$infile" "$outfile"
sim_code=$?

echo "Return code: $sim_code"
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
        echo "ls *-HITS.root returned nothing!"
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
if [ $sim_code -ne 0 ] || [ $copy_code -ne 0 ]; then
  exit 1
else
  exit 0
fi

