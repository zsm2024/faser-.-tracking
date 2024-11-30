#!/bin/bash
#
# Wrapper to run any python script in the athena environment
# Typically used to set up a script for use in condor
#
# runFaserScript.sh --rel <release_directory> script.py arguments...
#
function print_usage {
  echo "Usage: runFaserScript.sh --rel <release_directory> [--log <logfile>] script.py [arguments]"
  echo "  The first uption must be the release directory where asetup is called"
  echo "  All other options are passed to script.py"
  echo "  Options: "
  echo "    -h - print usage"
  echo "    --rel <release_directory> - specify release directory"
  echo "    --log <logfile> - redirect script output to logfile"
  echo "    -- End of options considered by this script"
}
#
release_directory=""
logfile=""
while [ -n "$1" ]
do
  case "$1" in
      -h | --help)
	  print_usage
	  exit 0;;

      --rel)
	  release_directory="$2";
	  shift;
	  shift;;

      --log)
	  logfile="$2";
	  shift;
	  shift;;

      --) # Signal that everything else should be executed
	  shift;
	  break;; 

      *) 
	  # Nothing we recognize, execute everything remaining
	  break;;
  esac
done

if [ -z "$release_directory" ]; then
    echo "Must specify release"
    print_usage
    exit 1
fi

# Redirect to log file if requested
if [ ! -z "$logfile" ]; then
#logfile="post_`date +%m%d-%H%M%S`.log"
    exec >& "$logfile"
fi
#
# Set up release
starting_directory=`pwd`
echo "cd $release_directory"
cd $release_directory
#
# Set up the release 
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
# Must pass something or source will pass *this* script's arguments instead
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh --
echo "fsetup Athena,22.0.49"
asetup --input=calypso/asetup.faser Athena,22.0.49
echo "source run/setup.sh"
source run/setup.sh
#
# Go back to where we started
echo "cd $starting_directory"
cd $starting_directory
#
# Now run the command
echo "$@"
eval "$@"
