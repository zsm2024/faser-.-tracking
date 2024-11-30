#!/bin/env python
#
# Create digitization DB 
#
# Can test results with
# AtlCoolConsole.py "sqlite://;schema=waveform_digi.db;dbname=OFLP200"
#
# Can merge with ALLP200.db with
# AtlCoolCopy "sqlite://;schema=waveform_digi.db;dbname=OFLP200" "sqlite://;schema=ALLP200.db;dbname=OFLP200"

filename = "waveform_digi.db"

import os
import sys
from PyCool import cool

# Useful utilities for manipulating COOL files
# See https://gitlab.cern.ch/atlas/athena/-/blob/master/Database/CoolConvUtilities/python/AtlCoolLib.py
import CoolConvUtilities.AtlCoolLib as AtlCoolLib

# Delete file if it exists
if os.path.exists(filename):
    print(f"File {filename} exists, deleting...")
    os.remove(filename)

# Open new DB (forceOpen will create if it doesn't exist)
connect_string = f'sqlite://;schema={filename};dbname=OFLP200'
db = AtlCoolLib.forceOpen(connect_string)

if not db:
    print(f"Error opening {connect_string}")
    sys.exit(1)

# Description creates type of folder (run/lumi keyed, list of channels)
description = AtlCoolLib.athenaDesc(runLumi=True, datatype="CondAttrListCollection")

# Define the folder payload
spec = cool.RecordSpecification()
spec.extend("base_mean", cool.StorageType.Float)
spec.extend("base_rms",  cool.StorageType.Float)
spec.extend("cb_norm", cool.StorageType.Float)
spec.extend("cb_mean", cool.StorageType.Float)
spec.extend("cb_sigma", cool.StorageType.Float)
spec.extend("cb_alpha", cool.StorageType.Float)
spec.extend("cb_n", cool.StorageType.Float)

# ensureFolder creates as needed
wave_folder = db.createFolderSet("/WAVE")
folder = AtlCoolLib.ensureFolder(db, '/WAVE/Digitization', spec, description, 
                                 version=cool.FolderVersioning.MULTI_VERSION)

if not folder:
    print("Could not access or create folders!")
    print(e)
    sys.exit(1)

# Crystalball function Parameters estimated from Deion's slides uploaded at
# https://indico.cern.ch/event/1099652/contributions/4626975/attachments/2352595/4013927/Faser-Physics-run3933-plots.pdf  (20/01/2022)

# Fill values into record
# Values in ADC counts
base_mean = 15650.
base_rms = 3.

# Calorimeter default (low gain/filters)
calo_record = cool.Record(spec)
calo_record['base_mean'] = base_mean
calo_record['base_rms'] = base_rms
calo_record['cb_norm'] = 0.56
calo_record['cb_mean'] =  820 # Time in ns
calo_record['cb_sigma'] = 3.67
calo_record['cb_alpha'] = -0.32
calo_record['cb_n'] = 1000.

vetonu_record = cool.Record(spec)
vetonu_record['base_mean'] = base_mean
vetonu_record['base_rms'] = base_rms
vetonu_record['cb_norm'] = 7040
vetonu_record['cb_mean'] =  815 # Time in ns
vetonu_record['cb_sigma'] = 5.0
vetonu_record['cb_alpha'] = -0.28
vetonu_record['cb_n'] = 1000.

veto_record = cool.Record(spec)
veto_record['base_mean'] = base_mean
veto_record['base_rms'] = base_rms
veto_record['cb_norm'] = 6840
veto_record['cb_mean'] =  815 # Time in ns
veto_record['cb_sigma'] = 3.35
veto_record['cb_alpha'] = -0.32
veto_record['cb_n'] = 9.

timing_record = cool.Record(spec)
timing_record['base_mean'] = base_mean
timing_record['base_rms'] = base_rms
timing_record['cb_norm'] = 4240
timing_record['cb_mean'] =  815 # Time in ns
timing_record['cb_sigma'] = 3.21
timing_record['cb_alpha'] = -0.424
timing_record['cb_n'] = 6.14

preshower_record = cool.Record(spec)
preshower_record['base_mean'] = base_mean
preshower_record['base_rms'] = base_rms
preshower_record['cb_norm'] = 400
preshower_record['cb_mean'] =  846 # Time in ns
preshower_record['cb_sigma'] = 4.0
preshower_record['cb_alpha'] = -0.32
preshower_record['cb_n'] = 1000.

# Dec. 10, 2022
# Values: 0.56, 5.6, 16.8, 168
# Dec. 19, 2022
# Lowered by 10% to 0.51, 5.1, 15.3, 153

# Low gain, filters in default
calo_record['cb_norm'] = 0.51
tag = "WAVE-Digitization-01-LG"
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, calo_record, 0, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, vetonu_record, 1, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, veto_record, 2, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, timing_record, 3, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, preshower_record, 4, tag)
folder.createTagRelation("WAVE-01", tag)

# Also associate this default with all of the global tags
# We will use a folder override to set other conditions
wave_folder.createTagRelation("OFLCOND-FASER-TB00", "WAVE-01")
wave_folder.createTagRelation("OFLCOND-FASER-01", "WAVE-01")
wave_folder.createTagRelation("OFLCOND-FASER-02", "WAVE-01")
wave_folder.createTagRelation("OFLCOND-FASER-03", "WAVE-01")

# Low gain, filters out
tag = "WAVE-Digitization-01-LG-nofilt"
calo_record['cb_norm'] = 5.1
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, calo_record, 0, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, vetonu_record, 1, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, veto_record, 2, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, timing_record, 3, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, preshower_record, 4, tag)

# High gain
tag = "WAVE-Digitization-01-HG"
calo_record['cb_norm'] = 15.3
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, calo_record, 0, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, vetonu_record, 1, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, veto_record, 2, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, timing_record, 3, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, preshower_record, 4, tag)

# High gain, filters out
tag = "WAVE-Digitization-01-HG-nofilt"
calo_record['cb_norm'] = 153
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, calo_record, 0, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, vetonu_record, 1, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, veto_record, 2, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, timing_record, 3, tag)
folder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, preshower_record, 4, tag)

# All done
db.closeDatabase()
print('Database completed')
