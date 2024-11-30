# FaserSCT_ConditionsData

For more information on conditions in general, see [[https://twiki.cern.ch/twiki/bin/view/FASER/OfflineDatabases][OfflineDatabases]].

## Creating databases

The scripts 
```
data/SCT_Conditions.py
data/BField_DataConditions.py
```
were used to create the intial databases for MC (OFLP200) and data (CONDBR3).  

## Updating field map

The following was used to update the magnetic field map on 10 Nov. 22:
```
cp /cvmfs/faser.cern.ch/repo/sw/database/DBRelease/current/sqlite200/ALLP200.db .

# This creates new field and scale entries with tag OFLCOND-FASER-03 in mysqlite.db
BField_Conditions_Update03.py

# Now we need to merge this into the existing DB
AtlCoolCopy "sqlite://;schema=mysqlite.db;dbname=OFLP200" "sqlite://;schema=ALLP200.db;dbname=OFLP200"
AtlCoolCopy "sqlite://;schema=mysqlite.db;dbname=CONDBR3" "sqlite://;schema=ALLP200.db;dbname=CONDBR3"

# For the MC instance, we also need to associate the existing /SCT and /Tracker tags
AtlCoolConsole.py "sqlite://;schema=ALLP200.db;dbname=OFLP200"
settag /SCT SCT-02 OFLCOND-FASER-03
settag /Tracker TRACKER-02 OFLCOND-FASER-03
```

After installing the new tags in ALLP200.db, we can test this locally by copying it to the data/sqlite200 subdirectory of the run directory, and eventually install it on cvmfs.
