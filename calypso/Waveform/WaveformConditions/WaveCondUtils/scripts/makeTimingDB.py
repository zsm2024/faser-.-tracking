#!/bin/env python

# Requires python 3.8 or higher
#
# Can test results with
# AtlCoolConsole.py "sqlite://;schema=waveform_reco.db;dbname=OFLP200"

#filename = 'waveform_reco.db'
filename = 'ALLP200.db'

# Nominal trigger time in ns
nominal_data = {
    0: 820.,
    4272: 830.,
    6525: 820.
}

# offset_channels = 20

# Run 
# 0 - initial data
# 3395 - Testbeam
# 

ehn1_offsets = [ -20., -20., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. ]
ti12_offsets = [ -20., -20., -20., -20., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. ]

offset_data = {
  0:    [ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. ],
# Initial TI12
  1324: [ -10., -10., -10., -10.,   0.,   0., 0., 0., 0., 0., 0., 0., 18., 18., 0., 0. ],
# Testbeam geometry
  3247: [ -10., -10., -10., -10., -10., -10., 15., 15., -20., -20., 0., 0., 0., 0., 0., 0. ],
# TI12
  4272: ti12_offsets,
# EHN1 (interleaved with TI12 running)
  4360: ehn1_offsets,
  4399: ti12_offsets,
  4409: ehn1_offsets,
  4411: ti12_offsets,
  4429: ehn1_offsets,
  4439: ti12_offsets,
  4876: ehn1_offsets,
  4892: ti12_offsets,
  4904: ehn1_offsets,
  4912: ti12_offsets,
  4954: ehn1_offsets,
  4989: ti12_offsets,
  4991: ehn1_offsets,
  4993: ti12_offsets,
  4996: ehn1_offsets,
  4997: ti12_offsets,
  5042: ehn1_offsets,
  5050: ti12_offsets,
# IFT and VetoNu installed
  6525: [ -10., -10., -10., -10., -25., -25., 0., 0., 0., 0., 0., 0., 18., 18., 0., 0. ],
# 2024, add 2nd digitizer
  13847: [ -10., -10., -10., -10., -25., -25., 0., 0., 0., 0., 0., 0., 18., 18., 0., 0., -10., -10., -10., -10. ]
    
}

attr_list_desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="40774348" /></addrHeader><typeName>AthenaAttributeList</typeName>'

cond_attr_list_desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'

maxInt32 = 0xFFFFFFFF


# Look for data entry errors

print('Validating nominal data')

lastRun = -1
for run, data in nominal_data.items():
    assert isinstance(run, int), 'Run number is not integer'
    assert isinstance(data, float), 'Time is not float'
    assert run > lastRun, 'Run numbers out of order'
    assert run <= maxInt32, 'Run number out of range'
    lastRun = run

print('Validating offset data')
lastRun = -1
for run, data in offset_data.items():
    assert isinstance(run, int), 'Run number is not integer'
    assert run > lastRun, 'Run numbers out of order'
    assert run <= maxInt32, 'Run number out of range'
    lastRun = run
    #assert len(data) == offset_channels, 'Offset data does not have '+str(offset_channels)+' entries'
    for i in range(len(data)):
        assert isinstance(data[i], float), 'Offset time is not float'

# Data looks OK

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

dbSvc = cool.DatabaseSvcFactory.databaseService()
connectString = f'sqlite://;schema={filename};dbname=CONDBR3'

print('Opening database')

# Try to open existing, but if not create new
try:
    db = indirectOpen(connectString, readOnly=False)
except Exception as e:
    print(e)
    print("Couldn't open, try creating new")
    #dbSvc.dropDatabase( connectString )
    db = dbSvc.createDatabase( connectString )

# Nominal trigger times
nominalSpec = cool.RecordSpecification()
nominalSpec.extend( 'NominalTriggerTime', cool.StorageType.Float )

nominalFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, nominalSpec)
nominalFolder = db.createFolder('/WAVE/DAQ/Timing', nominalFolderSpec, attr_list_desc, True)

# There should be one record entered per IOV
lastValid = cool.ValidityKeyMax
for firstValidRun, time in reversed(nominal_data.items()):
    firstValid = (firstValidRun << 32)
    nominalRecord = cool.Record(nominalSpec)
    nominalRecord[ 'NominalTriggerTime' ] = float(time)
    nominalFolder.storeObject( firstValid, lastValid, nominalRecord, cool.ChannelId(0))
    lastValid = ((firstValidRun - 1) << 32) | (cool.ValidityKeyMax & 0x00000000FFFFFFFF)


# Trigger offset times

offsetSpec = cool.RecordSpecification()
offsetSpec.extend( 'TriggerOffset', cool.StorageType.Float )

offsetFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, offsetSpec)
offsetFolder = db.createFolder('/WAVE/DAQ/TimingOffset', offsetFolderSpec, cond_attr_list_desc, True)

# There should be one record entered per IOV
lastValid = cool.ValidityKeyMax
for firstValidRun, offset_list in reversed(offset_data.items()):
    firstValid = (firstValidRun << 32)
    for channel in range(len(offset_list)):
        offsetRecord = cool.Record(offsetSpec)
        offsetRecord[ 'TriggerOffset' ] = float(offset_list[channel])
        offsetFolder.storeObject( firstValid, lastValid, offsetRecord, cool.ChannelId(channel) )

    lastValid = ((firstValidRun - 1) << 32) | (cool.ValidityKeyMax & 0x00000000FFFFFFFF)


db.closeDatabase()

print('Database completed')

print('Working on MC database')

# Nominal data 
nominal_data = {
    0: 820.
}
# No offsets by default
# Expand this to 20 channels for 2024 data
offset_data = {
  0: [0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.]
}

# Validate again just in case
print('Validating nominal data')

lastRun = -1
for run, data in nominal_data.items():
    assert isinstance(run, int), 'Run number is not integer'
    assert isinstance(data, float), 'Time is not float'
    assert run > lastRun, 'Run numbers out of order'
    assert run <= maxInt32, 'Run number out of range'
    lastRun = run

print('Validating offset data')
lastRun = -1
for run, data in offset_data.items():
    assert isinstance(run, int), 'Run number is not integer'
    assert run > lastRun, 'Run numbers out of order'
    assert run <= maxInt32, 'Run number out of range'
    lastRun = run
    #assert len(data) == offset_channels, 'Offset data does not have '+str(offset_channels)+' entries'
    for i in range(len(data)):
        assert isinstance(data[i], float), 'Offset time is not float'

# Data looks OK

connectString = f'sqlite://;schema={filename};dbname=OFLP200'

print('Opening database')

# Try to open existing, but if not create new
try:
    db = indirectOpen(connectString, readOnly=False)
except Exception as e:
    print(e)
    print("Couldn't open, try creating new")
    #dbSvc.dropDatabase( connectString )
    db = dbSvc.createDatabase( connectString )

# Nominal trigger times
nominalSpec = cool.RecordSpecification()
nominalSpec.extend( 'NominalTriggerTime', cool.StorageType.Float )

nominalFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, nominalSpec)
nominalFolder = db.createFolder('/WAVE/DAQ/Timing', nominalFolderSpec, attr_list_desc, True)

# There should be one record entered per IOV
lastValid = cool.ValidityKeyMax
for firstValidRun, time in reversed(nominal_data.items()):
    firstValid = (firstValidRun << 32)
    nominalRecord = cool.Record(nominalSpec)
    nominalRecord[ 'NominalTriggerTime' ] = float(time)
    nominalFolder.storeObject( firstValid, lastValid, nominalRecord, cool.ChannelId(0))
    lastValid = ((firstValidRun - 1) << 32) | (cool.ValidityKeyMax & 0x00000000FFFFFFFF)


# Trigger offset times

offsetSpec = cool.RecordSpecification()
offsetSpec.extend( 'TriggerOffset', cool.StorageType.Float )

offsetFolderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, offsetSpec)
offsetFolder = db.createFolder('/WAVE/DAQ/TimingOffset', offsetFolderSpec, cond_attr_list_desc, True)

# There should be one record entered per IOV
lastValid = cool.ValidityKeyMax
for firstValidRun, offset_list in reversed(offset_data.items()):
    firstValid = (firstValidRun << 32)
    for channel in range(len(offset_list)):
        offsetRecord = cool.Record(offsetSpec)
        offsetRecord[ 'TriggerOffset' ] = float(offset_list[channel])
        offsetFolder.storeObject( firstValid, lastValid, offsetRecord, cool.ChannelId(channel) )

    lastValid = ((firstValidRun - 1) << 32) | (cool.ValidityKeyMax & 0x00000000FFFFFFFF)


db.closeDatabase()

print('Database completed')
