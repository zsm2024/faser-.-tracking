#!/bin/env python

# Use this to add a field map to the CONDBR3 database for real data
# Copied the result from the OFLP200 DB
# Note that the testbeam turns off the field by setting scale = 0

# This file creates the DB from scratch
# To add new tags, look at BField_Conditions_Update03.py

description = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'

descriptionDCS = '<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName><cache>600</cache>'

descriptionAlign = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="256" clid="1170039409" /></addrHeader><typeName>AlignableTransformContainer</typeName>'

import sys
from PyCool import cool, coral
from CoolConvUtilities.AtlCoolLib import forceOpen

dbSvc = cool.DatabaseSvcFactory.databaseService()
connectString = 'sqlite://;schema=ALLP200.db;dbname=CONDBR3'

print('generating field database')
#dbSvc.dropDatabase( connectString )
try:
    # Open existing instead?
    # This actually should open or create as needed
    print(f'Try forceOpen({connectString})')
    db = forceOpen( connectString )
except Exception as e:
    print(e)
    print('Problem opening DB, create instead')
    db = dbSvc.createDatabase( connectString )

glob = db.createFolderSet("/GLOBAL")
glob_bfield = db.createFolderSet("/GLOBAL/BField")

glob_bfield.createTagRelation("GLOBAL-01", "GLOBAL-BField-01")
glob.createTagRelation("OFLCOND-FASER-01", "GLOBAL-01")

glob_bfield.createTagRelation("GLOBAL-02", "GLOBAL-BField-02")
glob.createTagRelation("OFLCOND-FASER-02", "GLOBAL-02")

glob_bfield.createTagRelation("GLOBAL-03", "GLOBAL-BField-03")
glob.createTagRelation("OFLCOND-FASER-03", "GLOBAL-03")

glob_bfield.createTagRelation("GLOBAL-TB00", "GLOBAL-BField-TB00")
glob.createTagRelation("OFLCOND-FASER-TB00", "GLOBAL-TB00")

mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable_v101.root"

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-01", True )
mapFolder.createTagRelation("GLOBAL-BField-01", "GLOBAL-BField-Maps-01")

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-02", True )
mapFolder.createTagRelation("GLOBAL-BField-02", "GLOBAL-BField-Maps-02")

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-TB00", True )
mapFolder.createTagRelation("GLOBAL-BField-TB00", "GLOBAL-BField-Maps-TB00")

# New record
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable_v2.root"

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-03", True )
mapFolder.createTagRelation("GLOBAL-BField-03", "GLOBAL-BField-Maps-03")

scaleSpec = cool.RecordSpecification()
scaleSpec.extend( 'value', cool.StorageType.Float )

scaleRecord = cool.Record(scaleSpec)
scaleRecord['value'] = 1.0

scaleFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, scaleSpec)
scaleFolder = db.createFolder('/GLOBAL/BField/Scales', scaleFolderSpec, descriptionDCS, True )

# Channel names don't seem to be handled properly by Athena
scaleFolder.createChannel( 1, "Dipole_Scale" )
scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-01", True )
scaleFolder.createTagRelation("GLOBAL-BField-01", "GLOBAL-BField-Scale-01")


scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-02", True )
scaleFolder.createTagRelation("GLOBAL-BField-02", "GLOBAL-BField-Scale-02")


scaleRecord['value'] = 0.0
scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-TB00", True )
scaleFolder.createTagRelation("GLOBAL-BField-TB00", "GLOBAL-BField-Scale-TB00")

db.closeDatabase()
