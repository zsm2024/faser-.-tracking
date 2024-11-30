#!/bin/env python

# This script was used to add a new tag for the BField to both 
# the real data (CONDBR3) and MC (OFLP200) instances.
# This writes out to mysqlite.db which then needs to be merged
# with the production database using:
# AtlCoolCopy "sqlite://;schema=mysqlite.db;dbname=CONDBR3" "sqlite://;schema=ALLP200.db;dbname=CONDBR3"

description = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'

descriptionDCS = '<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName><cache>600</cache>'

descriptionAlign = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="256" clid="1170039409" /></addrHeader><typeName>AlignableTransformContainer</typeName>'

import sys
from PyCool import cool, coral
from CoolConvUtilities.AtlCoolLib import forceOpen

print('generating new field database')

dbSvc = cool.DatabaseSvcFactory.databaseService()
connectString = 'sqlite://;schema=mysqlite.db;dbname=CONDBR3'

# This should open or create as needed
try:
    print(f'forceOpen({connectString})')
    db = forceOpen( connectString )
except Exception as e:
    print(e)
    print('Problem opening DB!')
    sys.exit(1)

# Hierarchy of new tag is:
# OFLCOND-FASER-02 : GLOBAL-03 : GLOBAL-BField-03 : GLOBAL-BField-Maps-03

# Create new tag
glob = db.createFolderSet("/GLOBAL")
glob_bfield = db.createFolderSet("/GLOBAL/BField")

glob_bfield.createTagRelation("GLOBAL-03", "GLOBAL-BField-03")
glob.createTagRelation("OFLCOND-FASER-03", "GLOBAL-03")

print("Created tag GLOBAL-BField-03 and associated to OFLCOND-FASER-03")

mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

# New entry
mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable_v2.root"

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-03", True )
mapFolder.createTagRelation("GLOBAL-BField-03", "GLOBAL-BField-Maps-03")

print("Created new entry in /GLOBAL/BField/Maps with tag GLOBAL-BField-Maps-03")

# Also update the scale (since it sits in the same tag area)
scaleSpec = cool.RecordSpecification()
scaleSpec.extend( 'value', cool.StorageType.Float )

scaleRecord = cool.Record(scaleSpec)
scaleRecord['value'] = 1.0

scaleFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, scaleSpec)
scaleFolder = db.createFolder('/GLOBAL/BField/Scales', scaleFolderSpec, descriptionDCS, True )

# Channel names don't seem to be handled properly by Athena
scaleFolder.createChannel( 1, "Dipole_Scale" )

scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-03", True )
scaleFolder.createTagRelation("GLOBAL-BField-03", "GLOBAL-BField-Scale-03")

print("Created new entry in /GLOBAL/BField/Maps with tag GLOBAL-BField-Scale-03")

# These are the only tags in the CONDBR3 instance, so we are done

db.closeDatabase()

# Now do it again for the MC instance

connectString = 'sqlite://;schema=mysqlite.db;dbname=OFLP200'

# This should open or create as needed
try:
    print(f'forceOpen({connectString})')
    db = forceOpen( connectString )
except Exception as e:
    print(e)
    print('Problem opening DB!')
    sys.exit(1)

# Create new tag
glob = db.createFolderSet("/GLOBAL")
glob_bfield = db.createFolderSet("/GLOBAL/BField")

glob_bfield.createTagRelation("GLOBAL-03", "GLOBAL-BField-03")
glob.createTagRelation("OFLCOND-FASER-03", "GLOBAL-03")

print("Created tag GLOBAL-BField-03 and associated to OFLCOND-FASER-03")

mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

# New entry
mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable_v2.root"

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-03", True )
mapFolder.createTagRelation("GLOBAL-BField-03", "GLOBAL-BField-Maps-03")

print("Created new entry in /GLOBAL/BField/Maps with tag GLOBAL-BField-Maps-03")

# Also update the scale (since it sits in the same tag area)
scaleSpec = cool.RecordSpecification()
scaleSpec.extend( 'value', cool.StorageType.Float )

scaleRecord = cool.Record(scaleSpec)
scaleRecord['value'] = 1.0

scaleFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, scaleSpec)
scaleFolder = db.createFolder('/GLOBAL/BField/Scales', scaleFolderSpec, descriptionDCS, True )

# Channel names don't seem to be handled properly by Athena
scaleFolder.createChannel( 1, "Dipole_Scale" )

scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-03", True )
scaleFolder.createTagRelation("GLOBAL-BField-03", "GLOBAL-BField-Scale-03")

print("Created new entry in /GLOBAL/BField/Maps with tag GLOBAL-BField-Scale-03")

# Also make associations to /SCT and /TRACKER tags
#sct = db.createFolderSet("/SCT")
#sct.createTagRelation("OFLCOND-FASER-03", "SCT-02")
#tracker = db.createFolderSet("/Tracker")
#tracker.createTagRelation("OFLCOND-FASER-03", "TRACKER-02")
#print("Associated old tags for /SCT and /Tracker to OFLCOND-FASER-03")

# This doesn't work. 
# Instead, we need to go into AtlCoolConsole.py and 
# set the association there.
#
# settag /SCT SCT-02 OFLCOND-FASER-03
# settag /Tracker TRACKER-02 OFLCOND-FASER-03
#
# Check that this worked:
# tracetags / OFLCOND-FASER-03

db.closeDatabase()
