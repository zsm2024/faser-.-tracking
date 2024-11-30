This example package serves several purposes:


*  It illustrates how to initialize and access the detector description.

*  It checks that the expected number of detector elements are created.

*  It generates a set of neutral alignment constants for the Tracker, which can be folded into the conditions database.

*  It dumps a standalone GeoModel sqlite geometry database which could be used by external programs to display FASER without requiring any FASER software (only GeoModel)

This example does NOT setup the magnetic field (see GeoModelTest for that)

To run the algorithm, in a run directory where Calypso has been installed (displayed below as the command-line prompt `run >`), do

```
run > source ./setup.sh
run > python python/WriteAlignment/WriteAlignmentConfig.py
```

The file `WriteAlignmentConfig.py` can be edited to change some run options.

After running the algorithm, to add the alignment data into your local conditions DB, do:

```
run > CopyAlignFolder.sh
```

By default, Calypso uses a conditions database installed at /cvmfs/faser.cern.ch, so the end user should not need to generate them
