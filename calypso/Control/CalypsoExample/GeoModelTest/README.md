This example package serves several purposes:


*  It illustrates how to initialize and access the detector description and magnetic field.

*  It checks that the expected number of detector elements are created.

*  It dumps a standalone GeoModel sqlite geometry database which could be used by external programs to display FASER without requiring any FASER software (only GeoModel)

To run the algorithm, in a run directory where Calypso has been installed (displayed below as the command-line prompt `run >`), do

```
run > source ./setup.sh
run > python python/GeoModelTest/GeoModelTestConfig.py
```

The file `GeoModelTestConfig.py` can be edited to change some run options.

