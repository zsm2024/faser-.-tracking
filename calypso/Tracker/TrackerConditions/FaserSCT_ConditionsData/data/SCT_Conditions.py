#!/bin/env python


sctChannels = [
       '2147483648', '2149580800', '2151677952', '2153775104', '2155872256', '2157969408', '2160066560', '2162163712', '2164260864', '2166358016', '2168455168', '2170552320', '2172649472', '2174746624', '2176843776', '2178940928', '2181038080', '2183135232', '2185232384', '2187329536',
       '2189426688', '2191523840', '2193620992', '2195718144', '2197815296', '2199912448', '2202009600', '2204106752', '2206203904', '2208301056', '2210398208', '2212495360', '2214592512', '2216689664', '2218786816', '2220883968', '2222981120', '2225078272', '2227175424', '2229272576',
       '2231369728', '2233466880', '2235564032', '2237661184', '2239758336', '2241855488', '2243952640', '2246049792', '2281701376', '2283798528', '2285895680', '2287992832', '2290089984', '2292187136', '2294284288', '2296381440', '2298478592', '2300575744', '2302672896', '2304770048',
       '2306867200', '2308964352', '2311061504', '2313158656', '2315255808', '2317352960', '2319450112', '2321547264', '2323644416', '2325741568', '2327838720', '2329935872', '2332033024', '2334130176', '2336227328', '2338324480', '2340421632', '2342518784', '2344615936', '2346713088',
       '2348810240', '2350907392', '2353004544', '2355101696', '2357198848', '2359296000', '2361393152', '2363490304', '2365587456', '2367684608', '2369781760', '2371878912', '2373976064', '2376073216', '2378170368', '2380267520', '2415919104', '2418016256', '2420113408', '2422210560',
       '2424307712', '2426404864', '2428502016', '2430599168', '2432696320', '2434793472', '2436890624', '2438987776', '2441084928', '2443182080', '2445279232', '2447376384', '2449473536', '2451570688', '2453667840', '2455764992', '2457862144', '2459959296', '2462056448', '2464153600',
       '2466250752', '2468347904', '2470445056', '2472542208', '2474639360', '2476736512', '2478833664', '2480930816', '2483027968', '2485125120', '2487222272', '2489319424', '2491416576', '2493513728', '2495610880', '2497708032', '2499805184', '2501902336', '2503999488', '2506096640',
       '2508193792', '2510290944', '2512388096', '2514485248' ]

iftChannels = [
       '2147483648', '2149580800', '2151677952', '2153775104', '2155872256', '2157969408', '2160066560', '2162163712', '2164260864', '2166358016', '2168455168', '2170552320', '2172649472', '2174746624', '2176843776', '2178940928', '2181038080', '2183135232', '2185232384', '2187329536',
       '2189426688', '2191523840', '2193620992', '2195718144', '2197815296', '2199912448', '2202009600', '2204106752', '2206203904', '2208301056', '2210398208', '2212495360', '2214592512', '2216689664', '2218786816', '2220883968', '2222981120', '2225078272', '2227175424', '2229272576',
       '2231369728', '2233466880', '2235564032', '2237661184', '2239758336', '2241855488', '2243952640', '2246049792', '2281701376', '2283798528', '2285895680', '2287992832', '2290089984', '2292187136', '2294284288', '2296381440', '2298478592', '2300575744', '2302672896', '2304770048',
       '2306867200', '2308964352', '2311061504', '2313158656', '2315255808', '2317352960', '2319450112', '2321547264', '2323644416', '2325741568', '2327838720', '2329935872', '2332033024', '2334130176', '2336227328', '2338324480', '2340421632', '2342518784', '2344615936', '2346713088',
       '2348810240', '2350907392', '2353004544', '2355101696', '2357198848', '2359296000', '2361393152', '2363490304', '2365587456', '2367684608', '2369781760', '2371878912', '2373976064', '2376073216', '2378170368', '2380267520', '2415919104', '2418016256', '2420113408', '2422210560',
       '2424307712', '2426404864', '2428502016', '2430599168', '2432696320', '2434793472', '2436890624', '2438987776', '2441084928', '2443182080', '2445279232', '2447376384', '2449473536', '2451570688', '2453667840', '2455764992', '2457862144', '2459959296', '2462056448', '2464153600',
       '2466250752', '2468347904', '2470445056', '2472542208', '2474639360', '2476736512', '2478833664', '2480930816', '2483027968', '2485125120', '2487222272', '2489319424', '2491416576', '2493513728', '2495610880', '2497708032', '2499805184', '2501902336', '2503999488', '2506096640',
       '2508193792', '2510290944', '2512388096', '2514485248', '2550136832', '2552233984', '2554331136', '2556428288', '2558525440', '2560622592', '2562719744', '2564816896', '2566914048', '2569011200', '2571108352', '2573205504', '2575302656', '2577399808', '2579496960', '2581594112',
       '2583691264', '2585788416', '2587885568', '2589982720', '2592079872', '2594177024', '2596274176', '2598371328', '2600468480', '2602565632', '2604662784', '2606759936', '2608857088', '2610954240', '2613051392', '2615148544', '2617245696', '2619342848', '2621440000', '2623537152',
       '2625634304', '2627731456', '2629828608', '2631925760', '2634022912', '2636120064', '2638217216', '2640314368', '2642411520', '2644508672', '2646605824', '2648702976' ]

tbChannels = [
       '2147483648', '2151677952', '2155872256', '2160066560', '2164260864', '2168455168', '2172649472', '2176843776', '2181038080', '2185232384', '2189426688', '2193620992', '2197815296', '2202009600', '2206203904', '2210398208', '2214592512', '2218786816', '2222981120', '2227175424',
       '2231369728', '2235564032', '2239758336', '2243952640', '2248146944', '2252341248', '2256535552', '2260729856', '2264924160', '2269118464', '2273312768', '2277507072', '2281701376', '2285895680', '2290089984', '2294284288', '2298478592', '2302672896', '2306867200', '2311061504',
       '2315255808', '2319450112', '2323644416', '2327838720', '2332033024', '2336227328', '2340421632', '2344615936' ]

description = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header clid="1238547719" service_type="71" /></addrHeader><typeName>CondAttrListCollection</typeName>'

descriptionDCS = '<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName><cache>600</cache>'

descriptionAlign = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="256" clid="1170039409" /></addrHeader><typeName>AlignableTransformContainer</typeName>'

from PyCool import cool, coral

dbSvc = cool.DatabaseSvcFactory.databaseService()
connectString = 'sqlite://;schema=ALLP200.db;dbname=OFLP200'

print('recreating database')
dbSvc.dropDatabase( connectString )
db = dbSvc.createDatabase( connectString )

alignSpec = cool.RecordSpecification()
alignSpec.extend( 'PoolRef', cool.StorageType.String4k )
alignFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, alignSpec, cool.PayloadMode.INLINEPAYLOAD)

tracker = db.createFolderSet("/Tracker")
align = db.createFolder("/Tracker/Align", alignFolderSpec, descriptionAlign, True)
sct = db.createFolderSet("/SCT")
sct_dcs = db.createFolderSet("/SCT/DCS")
sct_daq = db.createFolderSet("/SCT/DAQ")
sct_daq_calibration = db.createFolderSet("/SCT/DAQ/Calibration")

align.createTagRelation("TRACKER-01","TRACKER-ALIGN-01")
tracker.createTagRelation("OFLCOND-FASER-01","TRACKER-01")
sct_daq_calibration.createTagRelation("SCT-DAQ-01", "SCT-DAQ-Calibration-01")
sct_daq.createTagRelation("SCT-01", "SCT-DAQ-01")
sct_dcs.createTagRelation("SCT-01", "SCT-DCS-01")
sct.createTagRelation("OFLCOND-FASER-01", "SCT-01")

align.createTagRelation("TRACKER-02","TRACKER-ALIGN-02")
tracker.createTagRelation("OFLCOND-FASER-02","TRACKER-02")
sct_daq_calibration.createTagRelation("SCT-DAQ-02", "SCT-DAQ-Calibration-02")
sct_daq.createTagRelation("SCT-02", "SCT-DAQ-02")
sct_dcs.createTagRelation("SCT-02", "SCT-DCS-02")
sct.createTagRelation("OFLCOND-FASER-02", "SCT-02")

align.createTagRelation("TRACKER-TB00","TRACKER-ALIGN-TB00")
tracker.createTagRelation("OFLCOND-FASER-TB00","TRACKER-TB00")
sct_daq_calibration.createTagRelation("SCT-DAQ-TB00", "SCT-DAQ-Calibration-TB00")
sct_daq.createTagRelation("SCT-TB00", "SCT-DAQ-TB00")
sct_dcs.createTagRelation("SCT-TB00", "SCT-DCS-TB00")
sct.createTagRelation("OFLCOND-FASER-TB00", "SCT-TB00")

glob = db.createFolderSet("/GLOBAL")
glob_bfield = db.createFolderSet("/GLOBAL/BField")

glob_bfield.createTagRelation("GLOBAL-01", "GLOBAL-BField-01")
glob.createTagRelation("OFLCOND-FASER-01", "GLOBAL-01")

glob_bfield.createTagRelation("GLOBAL-02", "GLOBAL-BField-02")
glob.createTagRelation("OFLCOND-FASER-02", "GLOBAL-02")

glob_bfield.createTagRelation("GLOBAL-TB00", "GLOBAL-BField-TB00")
glob.createTagRelation("OFLCOND-FASER-TB00", "GLOBAL-TB00")

gainSpec = cool.RecordSpecification()
gainSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
gainSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
gainSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
gainSpec.extend( 'gainByChip'            , cool.StorageType.String4k )
gainSpec.extend( 'gainRMSByChip'         , cool.StorageType.String4k )
gainSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
gainSpec.extend( 'offsetRMSByChip'       , cool.StorageType.String4k )
gainSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )
gainSpec.extend( 'noiseRMSByChip'        , cool.StorageType.String4k )

gainRecord = cool.Record(gainSpec)
gainRecord[ 'serialNumber'    ] = 0
gainRecord[ 'runNumber'       ] = 0
gainRecord[ 'scanNumber'      ] = 0
gainRecord[ 'gainByChip'      ] = '52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0'
gainRecord[ 'gainRMSByChip'   ] = '1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25'
gainRecord[ 'offsetByChip'    ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'
gainRecord[ 'offsetRMSByChip' ] = '1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75'
gainRecord[ 'noiseByChip'     ] = '1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0'
gainRecord[ 'noiseRMSByChip'  ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'

gainFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, gainSpec)
gainFolder = db.createFolder('/SCT/DAQ/Calibration/ChipGain', gainFolderSpec, description, True)

for channel in sctChannels:
    gainFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, gainRecord, int(channel), "SCT-DAQ-Calibration-ChipGain-01", True )
gainFolder.createTagRelation("SCT-DAQ-Calibration-01", "SCT-DAQ-Calibration-ChipGain-01")

noiseSpec = cool.RecordSpecification()
noiseSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
noiseSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
noiseSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
noiseSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyByChip'       , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyRMSByChip'    , cool.StorageType.String4k )
noiseSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )

noiseRecord = cool.Record(noiseSpec)
noiseRecord[ 'serialNumber'       ] = 0
noiseRecord[ 'runNumber'          ] = 0
noiseRecord[ 'scanNumber'         ] = 0
noiseRecord[ 'offsetByChip'       ] = '60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0'
noiseRecord[ 'occupancyByChip'    ] = '3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05'
noiseRecord[ 'occupancyRMSByChip' ] = '2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05'

noiseFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, noiseSpec)
noiseFolder = db.createFolder('/SCT/DAQ/Calibration/ChipNoise', noiseFolderSpec, description, True )

for channel in sctChannels:
    noiseFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, noiseRecord, int(channel), "SCT-DAQ-Calibration-Noise-01", True )
noiseFolder.createTagRelation("SCT-DAQ-Calibration-01", "SCT-DAQ-Calibration-Noise-01")

chanstatSpec = cool.RecordSpecification()
chanstatSpec.extend( 'LVCHSTAT_RECV' , cool.StorageType.Int32 )
chanstatSpec.extend( 'STATE'         , cool.StorageType.UInt32 )

chanstatRecord = cool.Record(chanstatSpec)
chanstatRecord[ 'LVCHSTAT_RECV' ] = 209
chanstatRecord[ 'STATE' ] = 17

chanstatFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, chanstatSpec)
chanstatFolder = db.createFolder('/SCT/DCS/CHANSTAT', chanstatFolderSpec, descriptionDCS, True )

for channel in sctChannels:
    chanstatFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, chanstatRecord, int(channel), "SCT-DCS-Status-01", True )
chanstatFolder.createTagRelation("SCT-DCS-01", "SCT-DCS-Status-01")

hvSpec = cool.RecordSpecification()
hvSpec.extend( 'HVCHVOLT_RECV' , cool.StorageType.Float )
hvSpec.extend( 'HVCHCURR_RECV' , cool.StorageType.Float )

hvRecord = cool.Record(hvSpec)
hvRecord[ 'HVCHVOLT_RECV' ] = 150.0
hvRecord[ 'HVCHCURR_RECV' ] = 10.0

hvFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, hvSpec)
hvFolder = db.createFolder('/SCT/DCS/HV', hvFolderSpec, descriptionDCS, True )

for channel in sctChannels:
    hvFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, hvRecord, int(channel), "SCT-DCS-HV-01", True )
hvFolder.createTagRelation("SCT-DCS-01", "SCT-DCS-HV-01")

modtempSpec = cool.RecordSpecification()
modtempSpec.extend( 'MOCH_TM0_RECV' , cool.StorageType.Float )
modtempSpec.extend( 'MOCH_TM1_RECV' , cool.StorageType.Float )

modtempRecord = cool.Record(modtempSpec)
modtempRecord[ 'MOCH_TM0_RECV' ] = 7.0
modtempRecord[ 'MOCH_TM1_RECV' ] = 7.0

modtempFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, modtempSpec)
modtempFolder = db.createFolder('/SCT/DCS/MODTEMP', modtempFolderSpec, descriptionDCS, True )

for channel in sctChannels:
    modtempFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, modtempRecord, int(channel), "SCT-DCS-Temp-01", True )
modtempFolder.createTagRelation("SCT-DCS-01", "SCT-DCS-Temp-01")


mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable.root"

mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-01", True )
mapFolder.createTagRelation("GLOBAL-BField-01", "GLOBAL-BField-Maps-01")


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

# Start of IFT

gainSpec = cool.RecordSpecification()
gainSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
gainSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
gainSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
gainSpec.extend( 'gainByChip'            , cool.StorageType.String4k )
gainSpec.extend( 'gainRMSByChip'         , cool.StorageType.String4k )
gainSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
gainSpec.extend( 'offsetRMSByChip'       , cool.StorageType.String4k )
gainSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )
gainSpec.extend( 'noiseRMSByChip'        , cool.StorageType.String4k )

gainRecord = cool.Record(gainSpec)
gainRecord[ 'serialNumber'    ] = 0
gainRecord[ 'runNumber'       ] = 0
gainRecord[ 'scanNumber'      ] = 0
gainRecord[ 'gainByChip'      ] = '52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0'
gainRecord[ 'gainRMSByChip'   ] = '1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25'
gainRecord[ 'offsetByChip'    ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'
gainRecord[ 'offsetRMSByChip' ] = '1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75'
gainRecord[ 'noiseByChip'     ] = '1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0'
gainRecord[ 'noiseRMSByChip'  ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'

# gainFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, gainSpec)
# gainFolder = db.createFolder('/SCT/DAQ/Calibration/ChipGain', gainFolderSpec, description, True)

for channel in iftChannels:
    gainFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, gainRecord, int(channel), "SCT-DAQ-Calibration-ChipGain-02", True )
gainFolder.createTagRelation("SCT-DAQ-Calibration-02", "SCT-DAQ-Calibration-ChipGain-02")

noiseSpec = cool.RecordSpecification()
noiseSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
noiseSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
noiseSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
noiseSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyByChip'       , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyRMSByChip'    , cool.StorageType.String4k )
noiseSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )

noiseRecord = cool.Record(noiseSpec)
noiseRecord[ 'serialNumber'       ] = 0
noiseRecord[ 'runNumber'          ] = 0
noiseRecord[ 'scanNumber'         ] = 0
noiseRecord[ 'offsetByChip'       ] = '60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0'
noiseRecord[ 'occupancyByChip'    ] = '3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05'
noiseRecord[ 'occupancyRMSByChip' ] = '2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05'

# noiseFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, noiseSpec)
# noiseFolder = db.createFolder('/SCT/DAQ/Calibration/ChipNoise', noiseFolderSpec, description, True )

for channel in iftChannels:
    noiseFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, noiseRecord, int(channel), "SCT-DAQ-Calibration-Noise-02", True )
noiseFolder.createTagRelation("SCT-DAQ-Calibration-02", "SCT-DAQ-Calibration-Noise-02")

chanstatSpec = cool.RecordSpecification()
chanstatSpec.extend( 'LVCHSTAT_RECV' , cool.StorageType.Int32 )
chanstatSpec.extend( 'STATE'         , cool.StorageType.UInt32 )

chanstatRecord = cool.Record(chanstatSpec)
chanstatRecord[ 'LVCHSTAT_RECV' ] = 209
chanstatRecord[ 'STATE' ] = 17

# chanstatFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, chanstatSpec)
# chanstatFolder = db.createFolder('/SCT/DCS/CHANSTAT', chanstatFolderSpec, descriptionDCS, True )

for channel in iftChannels:
    chanstatFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, chanstatRecord, int(channel), "SCT-DCS-Status-02", True )
chanstatFolder.createTagRelation("SCT-DCS-02", "SCT-DCS-Status-02")

hvSpec = cool.RecordSpecification()
hvSpec.extend( 'HVCHVOLT_RECV' , cool.StorageType.Float )
hvSpec.extend( 'HVCHCURR_RECV' , cool.StorageType.Float )

hvRecord = cool.Record(hvSpec)
hvRecord[ 'HVCHVOLT_RECV' ] = 150.0
hvRecord[ 'HVCHCURR_RECV' ] = 10.0

# hvFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, hvSpec)
# hvFolder = db.createFolder('/SCT/DCS/HV', hvFolderSpec, descriptionDCS, True )

for channel in iftChannels:
    hvFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, hvRecord, int(channel), "SCT-DCS-HV-02", True )
hvFolder.createTagRelation("SCT-DCS-02", "SCT-DCS-HV-02")

modtempSpec = cool.RecordSpecification()
modtempSpec.extend( 'MOCH_TM0_RECV' , cool.StorageType.Float )
modtempSpec.extend( 'MOCH_TM1_RECV' , cool.StorageType.Float )

modtempRecord = cool.Record(modtempSpec)
modtempRecord[ 'MOCH_TM0_RECV' ] = 7.0
modtempRecord[ 'MOCH_TM1_RECV' ] = 7.0

# modtempFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, modtempSpec)
# modtempFolder = db.createFolder('/SCT/DCS/MODTEMP', modtempFolderSpec, descriptionDCS, True )

for channel in iftChannels:
    modtempFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, modtempRecord, int(channel), "SCT-DCS-Temp-02", True )
modtempFolder.createTagRelation("SCT-DCS-02", "SCT-DCS-Temp-02")


mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable.root"

# mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
# mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-02", True )
mapFolder.createTagRelation("GLOBAL-BField-02", "GLOBAL-BField-Maps-02")

scaleSpec = cool.RecordSpecification()
scaleSpec.extend( 'value', cool.StorageType.Float )

scaleRecord = cool.Record(scaleSpec)
scaleRecord['value'] = 1.0

# scaleFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, scaleSpec)
# scaleFolder = db.createFolder('/GLOBAL/BField/Scales', scaleFolderSpec, descriptionDCS, True )

# Channel names don't seem to be handled properly by Athena
# scaleFolder.createChannel( 1, "Dipole_Scale" )
scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-02", True )
scaleFolder.createTagRelation("GLOBAL-BField-02", "GLOBAL-BField-Scale-02")

# Start of TestBeam

gainSpec = cool.RecordSpecification()
gainSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
gainSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
gainSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
gainSpec.extend( 'gainByChip'            , cool.StorageType.String4k )
gainSpec.extend( 'gainRMSByChip'         , cool.StorageType.String4k )
gainSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
gainSpec.extend( 'offsetRMSByChip'       , cool.StorageType.String4k )
gainSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )
gainSpec.extend( 'noiseRMSByChip'        , cool.StorageType.String4k )

gainRecord = cool.Record(gainSpec)
gainRecord[ 'serialNumber'    ] = 0
gainRecord[ 'runNumber'       ] = 0
gainRecord[ 'scanNumber'      ] = 0
gainRecord[ 'gainByChip'      ] = '52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0 52.0'
gainRecord[ 'gainRMSByChip'   ] = '1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25 1.25'
gainRecord[ 'offsetByChip'    ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'
gainRecord[ 'offsetRMSByChip' ] = '1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75 1.75'
gainRecord[ 'noiseByChip'     ] = '1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0 1600.0'
gainRecord[ 'noiseRMSByChip'  ] = '45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0 45.0'

# gainFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, gainSpec)
# gainFolder = db.createFolder('/SCT/DAQ/Calibration/ChipGain', gainFolderSpec, description, True)

for channel in tbChannels:
    gainFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, gainRecord, int(channel), "SCT-DAQ-Calibration-ChipGain-TB00", True )
gainFolder.createTagRelation("SCT-DAQ-Calibration-TB00", "SCT-DAQ-Calibration-ChipGain-TB00")

noiseSpec = cool.RecordSpecification()
noiseSpec.extend( 'serialNumber'          , cool.StorageType.UInt63 )
noiseSpec.extend( 'runNumber'             , cool.StorageType.UInt32 )
noiseSpec.extend( 'scanNumber'            , cool.StorageType.UInt32 )
noiseSpec.extend( 'offsetByChip'          , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyByChip'       , cool.StorageType.String4k )
noiseSpec.extend( 'occupancyRMSByChip'    , cool.StorageType.String4k )
noiseSpec.extend( 'noiseByChip'           , cool.StorageType.String4k )

noiseRecord = cool.Record(noiseSpec)
noiseRecord[ 'serialNumber'       ] = 0
noiseRecord[ 'runNumber'          ] = 0
noiseRecord[ 'scanNumber'         ] = 0
noiseRecord[ 'offsetByChip'       ] = '60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0 60.0'
noiseRecord[ 'occupancyByChip'    ] = '3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05 3.50e-05'
noiseRecord[ 'occupancyRMSByChip' ] = '2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05 2.50e-05'

# noiseFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, noiseSpec)
# noiseFolder = db.createFolder('/SCT/DAQ/Calibration/ChipNoise', noiseFolderSpec, description, True )

for channel in tbChannels:
    noiseFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, noiseRecord, int(channel), "SCT-DAQ-Calibration-Noise-TB00", True )
noiseFolder.createTagRelation("SCT-DAQ-Calibration-TB00", "SCT-DAQ-Calibration-Noise-TB00")

chanstatSpec = cool.RecordSpecification()
chanstatSpec.extend( 'LVCHSTAT_RECV' , cool.StorageType.Int32 )
chanstatSpec.extend( 'STATE'         , cool.StorageType.UInt32 )

chanstatRecord = cool.Record(chanstatSpec)
chanstatRecord[ 'LVCHSTAT_RECV' ] = 209
chanstatRecord[ 'STATE' ] = 17

# chanstatFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, chanstatSpec)
# chanstatFolder = db.createFolder('/SCT/DCS/CHANSTAT', chanstatFolderSpec, descriptionDCS, True )

for channel in tbChannels:
    chanstatFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, chanstatRecord, int(channel), "SCT-DCS-Status-TB00", True )
chanstatFolder.createTagRelation("SCT-DCS-TB00", "SCT-DCS-Status-TB00")

hvSpec = cool.RecordSpecification()
hvSpec.extend( 'HVCHVOLT_RECV' , cool.StorageType.Float )
hvSpec.extend( 'HVCHCURR_RECV' , cool.StorageType.Float )

hvRecord = cool.Record(hvSpec)
hvRecord[ 'HVCHVOLT_RECV' ] = 150.0
hvRecord[ 'HVCHCURR_RECV' ] = 10.0

# hvFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, hvSpec)
# hvFolder = db.createFolder('/SCT/DCS/HV', hvFolderSpec, descriptionDCS, True )

for channel in tbChannels:
    hvFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, hvRecord, int(channel), "SCT-DCS-HV-TB00", True )
hvFolder.createTagRelation("SCT-DCS-TB00", "SCT-DCS-HV-TB00")

modtempSpec = cool.RecordSpecification()
modtempSpec.extend( 'MOCH_TM0_RECV' , cool.StorageType.Float )
modtempSpec.extend( 'MOCH_TM1_RECV' , cool.StorageType.Float )

modtempRecord = cool.Record(modtempSpec)
modtempRecord[ 'MOCH_TM0_RECV' ] = 7.0
modtempRecord[ 'MOCH_TM1_RECV' ] = 7.0

# modtempFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, modtempSpec)
# modtempFolder = db.createFolder('/SCT/DCS/MODTEMP', modtempFolderSpec, descriptionDCS, True )

for channel in tbChannels:
    modtempFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, modtempRecord, int(channel), "SCT-DCS-Temp-TB00", True )
modtempFolder.createTagRelation("SCT-DCS-TB00", "SCT-DCS-Temp-TB00")


mapSpec = cool.RecordSpecification()
mapSpec.extend( 'FieldType', cool.StorageType.String4k )
mapSpec.extend( 'MapFileName', cool.StorageType.String4k )

mapRecord = cool.Record(mapSpec)
mapRecord['FieldType'] = "GlobalMap"
mapRecord['MapFileName'] = "file:MagneticFieldMaps/FaserFieldTable.root"

# mapFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, mapSpec)
# mapFolder = db.createFolder('/GLOBAL/BField/Maps', mapFolderSpec, descriptionDCS, True )

mapFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, mapRecord, 1, "GLOBAL-BField-Maps-TB00", True )
mapFolder.createTagRelation("GLOBAL-BField-TB00", "GLOBAL-BField-Maps-TB00")

scaleSpec = cool.RecordSpecification()
scaleSpec.extend( 'value', cool.StorageType.Float )

scaleRecord = cool.Record(scaleSpec)
scaleRecord['value'] = 0.0

# scaleFolderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, scaleSpec)
# scaleFolder = db.createFolder('/GLOBAL/BField/Scales', scaleFolderSpec, descriptionDCS, True )

# Channel names don't seem to be handled properly by Athena
# scaleFolder.createChannel( 1, "Dipole_Scale" )
scaleFolder.storeObject( cool.ValidityKeyMin, cool.ValidityKeyMax, scaleRecord, 1, "GLOBAL-BField-Scale-TB00", True )
scaleFolder.createTagRelation("GLOBAL-BField-TB00", "GLOBAL-BField-Scale-TB00")

db.closeDatabase()
