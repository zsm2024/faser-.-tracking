#!/usr/bin/env python

import ROOT

from PyCool import cool

dbSvc = cool.DatabaseSvcFactory.databaseService()
# Data DB name
connectString = 'sqlite://;schema=noisy_strips.db;dbname=CONDBR3'

print('Creating empty database')

dbSvc.dropDatabase(connectString)

db = dbSvc.createDatabase(connectString)

noisyStripsSpec = cool.RecordSpecification()
noisyStripsSpec.extend('sensor', cool.StorageType.Int32)
noisyStripsSpec.extend('nstrips', cool.StorageType.Int32)
noisyStripsSpec.extend('noisyList', cool.StorageType.String64k)

description = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'
noisyStripsFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, noisyStripsSpec)
noisyStripsFolder = db.createFolder('/SCT/DAQ/NoisyStrips', noisyStripsFolderSpec, description, True)

firstValid = cool.ValidityKeyMin
lastValid = cool.ValidityKeyMax

for sensor in range(192):
    noisyStripsRecord = cool.Record(noisyStripsSpec)
    noisyStripsRecord['sensor'] = int(sensor)
    noisyStripsRecord['nstrips'] = 0
    noisyStripsFolder.storeObject(firstValid, lastValid, noisyStripsRecord, sensor)

db.closeDatabase()

# Do this again for MC
connectString = 'sqlite://;schema=noisy_strips.db;dbname=OFLP200'

print('Creating empty MC database')

# Don't drop the DB we just made
#dbSvc.dropDatabase(connectString)

# Create new DB (different name in same file for MC
db = dbSvc.createDatabase(connectString)

noisyStripsFolder = db.createFolder('/SCT/DAQ/NoisyStrips', noisyStripsFolderSpec, description, True)

firstValid = cool.ValidityKeyMin
lastValid = cool.ValidityKeyMax

numNoisyStrips = 0  # This is used as a channel...
for sensor in range(192):
    noisyStripsRecord = cool.Record(noisyStripsSpec)
    noisyStripsRecord['sensor'] = int(sensor)
    noisyStripsRecord['nstrips'] = 0
    noisyStripsFolder.storeObject(firstValid, lastValid, noisyStripsRecord, sensor)

db.closeDatabase()

print('Database completed')
