This package provides the infrastructure to read in FASER raw data files and convert the raw information to transient data classes.  Related packages include:

Event/FaserByteStreamCnvSvcBase - home of the ROBDataProvider

Event/FaserEventStorage - low-level file handling routines

faser-common/EventFormats - bytestream formaat definition used online

A test job to read in a raw data file and write Raw Data Objects to xAOD can be found in:

Event/FaserByteStreamCnvSvc/share/jobOptions_faserBSToRDO.py

Copy this file locally, update to point to a FASER raw data file, and run with:
athena.py jobOptions_faserBSToRDO.py
