#!/usr/bin/bash
#
# Script to filter events likely to have tracks and reconstruct them
# Need to build calypso AND faser-common for this to work
# Execute from the run folder in a release directory
#
# Usage:
#  ./filterRun.sh [--seg] [--install] <runnumber>
#
# Options:
#   --seg - filter individual segments
#           by default, run is filtered into single file
#   --install - copy output to EOS
#   --cleanup - delete local files after EOS copy
#
# During the first 13.6 collision event, this was run with:
#  watch -n 150 ./filterRun.sh --install --seg 7733
#
# The --cleanup flag doesn't work for this, as the script checks the
# local directory (not EOS) to see if a segment has already been processed.
#
# To just make a single filtered file for a run and cleanup afterwards:
#  ./filterRun.sh --install --cleanup 7733
#
# Hacky parameters
# These are used to label the output files, correct for when this was used,
# but may not be correct in the future
rtag='r0008'
filter='TrigMask08'
#
# Usage function
print_usage () {
    echo "Usage: filterRun [--seg] [--install] [--cleanup] <runnum>"
}

# EOS copy function
copy_to_eos () {
    local filtdir=/eos/experiment/faser/filter/2022/$filter
    local rawdir=$filtdir/raw/$run_number
    local recdir=$filtdir/$rtag/$run_number
    local logdir=$filtdir/${rtag}_log/$run_number

    # Make sure directories exist
    mkdir -p $rawdir
    mkdir -p $recdir
    mkdir -p $logdir

    # Copy files
    echo "Copy $outfile to $rawdir"
    xrdcp --silent --force $outfile root://eospublic.cern.ch/${rawdir}
    echo "Copy $recfile to $recdir"
    xrdcp --silent --force $recfile root://eospublic.cern.ch/${recdir}
    echo "Copy $logfile to $logdir"
    xrdcp --silent --force $logfile root://eospublic.cern.ch/${logdir}
}

# Parse commaand-line options
while [ -n "$1" ]
do
  case "$1" in
    -s | --seg)
	  echo "Filtering individual segments"
	  seg=1
	  shift;; # This eats the option

    -i | --install)
          echo "Copying files to EOS when done"
          install=1
          shift;;

    --cleanup)
	  cleanup=1
	  shift;;

    -*)
	  echo "Unknown option $1"
	  shift;;

    *) break;; # Not an option, don't shift
  esac
done
#
if [ -z $1 ]
then
    echo "No run number specified!"
    print_usage
    exit 1
fi
#
printf -v run "%06d" ${1}
echo Filtering run ${1}
#
eosdir=/eos/experiment/faser/raw/2022/$run
appstr=''
for file in $eosdir/*
do
    if ! [ -f $file ]; then
	continue
    fi

    if ! [[ $file == $eosdir/Faser-Physics-* ]]; then
	continue
    fi

    filename=`basename $file`

    # Split filename
    OIFS=$IFS
    IFS='.'
    read file_stem ext <<< "$filename"

    IFS='-'
    read faser type run_number segment <<< "$file_stem"

    # Put the delimieter baack
    IFS=$OIFS

    # Do this one file at a time?
    if [ -z $seg ]
    then
	# Append filtered output to one file
	outfile=Faser-$type-$run_number-$filter.$ext
	logfile=Faser-$type-$run_number-$filter.log

	# Make sure logfile exists for first iteration
	touch $logfile
	echo Filtering $filename to $outfile 
	echo Filtering $filename to $outfile >> $logfile

	# Split file to get file name
	../calypso/faser-common/build/EventFormats/eventFilter $appstr -t 0x08 $file $outfile >> $logfile 2>&1

	# Append from here on out
	appstr='-a'

    else
	# Separate files for each segment
	outfile=Faser-$type-$run_number-$segment-$filter.$ext
	logfile=Faser-$type-$run_number-$segment-$filter.log

	# Don't overwrite
	if [ -f "$outfile" ]; then
	    echo $outfile already done - skipping
	    continue
	fi

	echo Filtering $filename to $outfile 
	echo Filtering $filename to $outfile > $logfile

	# Filter
	../calypso/faser-common/build/EventFormats/eventFilter -t 0x08 $file $outfile >> $logfile 2>&1

	echo Filtered `grep "#fragments=" $logfile | wc -l` events
	echo Filtered `grep "#fragments=" $logfile | wc -l` events >> $logfile

	# And reconstruct it if not empty
	if ! [ -s $outfile ]; then
	    echo "No events filtered" >> $logfile
	    continue
	fi

	# Recontruct
	echo "Reco $outfile"
	faser_reco.py -r $rtag $outfile TI12Data03 >> $logfile 2>&1

	# Do we want to save this to EOS?
	if [ -z $install ]; then
	    continue
	fi

	# Copy files
	recfile=Faser-Physics-$run_number-$segment-$filter-$rtag-xAOD.root
	copy_to_eos

    fi

done

# Did we write one big file?
if [ -z $seg ]
then

  echo Filtered `grep "#fragments=" $logfile | wc -l` events
  echo Filtered `grep "#fragments=" $logfile | wc -l` events >> $logfile

  # Run reco if file isn't empty
  if [ -s $outfile ]; then
    echo "Reco $outfile"
    faser_reco.py -r $rtag $outfile TI12Data03 >> $logfile 2>&1

    # Do we want to save this to EOS?
    if [ -z $install ]; then
	exit 0
    fi

    recfile=Faser-Physics-$run_number-$filter-$rtag-xAOD.root
    copy_to_eos

  else
      echo "No events filtered" >> $logfile
  fi
fi

if ! [ -z $cleanup ]; then
  echo "Removing files"
  rm Faser-Physics-$run_number-*
  rm PoolFileCatalog.xml
  rm SegmentFitHistograms.root
  rm eventLoopHeartBeat.txt
fi
