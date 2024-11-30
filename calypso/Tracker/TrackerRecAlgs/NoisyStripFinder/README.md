Noisy strips are now part of the conditions database and are used in default reco

To update the database, the following steps should be used.

1) Determine the runs to update.  
A handy utility to find all runs of a given type is findFaserRunsByType.py:

`findFaserRunsByType.py -t Physics -o physics_runs.txt 7730-8370`

2) Submit jobs to create the noisy strip DB for each run
From a release directory, best to create a subdirectory for this and then:

`submitNoisyStripJobs.py --release .. physics_runs.txt`

Can also just specify run numbers (or a range) instead of a text file of runs

If you want to do this interactively, this script submits jobs to run the following:
`NoisyStripFinderJob.py`
`makeNoisyStripDB.py`

3) Check that all of the jobs finished successfully (or can check individual runs)

`checkNoisyStripJobs.py physics_runs.txt`

This can also write out a file of runs to submit again

4) Check that the actual noisy strips found make sense
This reads the individual DB files in each subdirectory, can also specify specific runs

`checkNoisyStripDB.py physics_runs.txt`

5) Merge the individual runs into a single DB
Note this script writes the individual runs with open-ended IOVs. 
This means the last run merged will be used for all later data until the DB is updated again.  
So if you are merging multiple times (with the --append option), it is important to do ths in chronological order.  

`mergeNoisyStripDB.py physics_runs.txt`

Note, this can be slow.  Use --verbose to watch the progress.

6) Test the database
The resulting database by default has the name noisy_strips.db.  
This can be copied to the data/sqlite200 subdirectory of the working directory and reco jobs will use this.
Check here for details:
https://gitlab.cern.ch/faser/calypso/-/blob/master/Database/ConnectionManagement/FaserAuthentication/data/dblookup.xml

7) Merge with production DB
This updated noisy strips folder /SCT/DAQ/NoisyStrips now needs to be merged into the production DB.  
First copy the current DB from CVMFS to some local directory.

`cp /cvmfs/faser.cern.ch/repo/sw/database/DBRelease/current/sqlite200/ALLP200.db .`

Next, use AtlCoolCopy to merge the updates into this file:

`AtlCoolCopy "sqlite://;schema=noisy_strips.db;dbname=CONDBR3" "sqlite://;schema=ALLP200.db;dbname=CONDBR3" `

This can also be slow.

Finally, the ALLP200.db file should be installed on cvmfs once everything is verified to be correct.

Older instructions from Tobias when he was developing this package are here:

Mask noisy strips:

1) Run the NoisyStripFinderDbg.py on raw data files

2) Run makeNoisyStripDB.py on the NoisyStripFinderHist.root produced by the NoisyStripFinderDbg.py script

3) Edit the path for the COOLOFL_INDET database in Database/ConnectionManagement/FaserAuthentication/data/dblookup.xml to point to the noisy_strips.db produced by the makeNoisyStripDB.py script

4) Set the checkBadChannels flag to True for the FaserSCT_ClusterizationCfg, e.g. like this  
   acc.merge(FaserSCT_ClusterizationCfg(ConfigFlags, checkBadChannels=True, DataObjectName="SCT_EDGEMODE_RDOs", ClusterToolTimingPattern="01X"))
