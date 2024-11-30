This package will translate Geant4 hits (HITS) into RawDataObject (RDO) data files.

Because input data and alignment is needed, there are a few steps to prepare. From a run directory where calypso is installed (displayed in the command-line prompt as `run >` below), do:

```
run > source ./setup.sh
```

This sets up the runtime environment after building. The "." is mandatory.

```
run > runG4.py >& runG4.log
```

This will create a HITS file named my.HITS.pool.root.

Now you can run digitization on the HITS file you generated with the command:

```
run > FaserSCT_DigitizationDbg.py >& digi.log
```

This will read your `my.HITS.pool.root` and generate an RDO data file 'my.RDO.pool.root' with digitized raw data objects.

