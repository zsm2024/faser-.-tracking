#!/usr/bin/env python 
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.DetectorConfigFlags import setupDetectorsFromList #, enableDetectors, disableDetectors

configFlags = initConfigFlags()
configFlags._loadDynaFlags('GeoModel')
configFlags._loadDynaFlags('Detector')

test_tags = [
    'FASER-00',
    'FASER-01'   # example 
]

flags_runs = {t: configFlags.clone() for t in test_tags}

for tag in test_tags:
    flags = flags_runs[tag]
    flags.GeoModel.FaserVersion = tag

    print(f'{flags.GeoModel.FaserVersion} - {flags.GeoModel.Run}')
    flags.dump('Detector.(Geometry|Enable)', True)
    print()

    # test flags that change with time
    assert flags.Detector.EnableEcal == (tag != 'FASER-00')
    # assert flags.Detector.EnableBCM == (tag != 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableDBM == (tag in ['ATLAS-R2-2016-01-00-01', 'ATLAS-R3S-2021-01-00-01'])
    # assert flags.Detector.EnablePixel == (tag != 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableSCT == (tag != 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableTRT == (tag != 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableITkPixel == (tag == 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableITkStrip == (tag == 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableHGTD == (tag == 'ATLAS-P2-ITK-24-00-00')
    # assert flags.Detector.EnableCSC == (tag in ['ATLAS-R1-2012-03-01-00', 'ATLAS-R2-2016-01-00-01'])
    # assert flags.Detector.EnablesTGC == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-24-00-00'])
    # assert flags.Detector.EnableMM == (tag in ['ATLAS-R3S-2021-01-00-01', 'ATLAS-P2-ITK-24-00-00'])


# test setup for Run 3
flags = flags_runs['FASER-01']

print("Test: validate ['Tracker', 'Scintillator', 'FaserCalo', 'Dipole']")
assert not setupDetectorsFromList(flags, ['Tracker', 'Scintillator', 'FaserCalo', 'Dipole'], validate_only=True)
print()

print("Test: validate ['Magnet']")
assert setupDetectorsFromList(flags, ['Magnet'], validate_only=True)
print()

print("Test: setup ['Magnet']")
assert setupDetectorsFromList(flags, ['Magnet'], toggle_geometry=True)
assert flags.Detector.GeometryDipole
assert not flags.Detector.EnableFaserSCT
assert not flags.Detector.EnableEcal
assert not flags.Detector.EnableVeto
assert not flags.Detector.EnableVetoNu
assert not flags.Detector.EnableTrigger
assert not flags.Detector.EnablePreshower
assert not flags.Detector.GeometryFaserSCT
assert not flags.Detector.GeometryEcal
assert not flags.Detector.GeometryVeto
assert not flags.Detector.GeometryVetoNu
assert not flags.Detector.GeometryTrigger
assert not flags.Detector.GeometryPreshower
print()

# print("Test: setup ['ID'] (geometry included)")
# assert setupDetectorsFromList(flags, ['ID'], toggle_geometry=True)
# assert flags.Detector.EnablePixel
# assert flags.Detector.GeometryPixel
# assert flags.Detector.EnableSCT
# assert flags.Detector.GeometrySCT
# assert flags.Detector.EnableTRT
# assert flags.Detector.GeometryTRT
# assert not flags.Detector.EnableCSC
# assert not flags.Detector.GeometryCSC
# assert not flags.Detector.EnableMM
# print()

# print("Test: enable ['ID'] (geometry included)")
# assert not enableDetectors(flags, ['ID'], toggle_geometry=True)
# print()

# print("Test: disable ['ID'] (geometry included)")
# assert disableDetectors(flags, ['ID'], toggle_geometry=True)
# assert not flags.Detector.EnablePixel
# assert not flags.Detector.GeometryPixel
# assert not flags.Detector.EnableSCT
# assert not flags.Detector.GeometrySCT
# assert not flags.Detector.EnableTRT
# assert not flags.Detector.GeometryTRT
# print()

# print("Test: enable ['MDT', 'RPC']")
# assert enableDetectors(flags, ['MDT', 'RPC'])
# assert flags.Detector.EnableMDT
# assert not flags.Detector.GeometryMDT
# assert flags.Detector.EnableRPC
# assert not flags.Detector.GeometryRPC
# assert not flags.Detector.EnableCSC
# assert not flags.Detector.GeometryCSC
# print()

# print("Test: enable ['LAr']")
# assert enableDetectors(flags, ['LAr'])
# assert flags.Detector.EnableLAr
# assert flags.Detector.EnableL1Calo
# print()

# print("Test: enable ['Forward']")
# assert enableDetectors(flags, ['Forward'])
# assert flags.Detector.EnableLucid
# assert flags.Detector.EnableZDC
# assert flags.Detector.EnableALFA
# assert flags.Detector.EnableAFP
# assert flags.Detector.EnableFwdRegion
# print()


# # test setup for Run 4
# flags = flags_runs['ATLAS-P2-ITK-24-00-00']

# print("Test: validate ['ITk', 'HGTD', 'Calo', 'Muon']")
# assert not setupDetectorsFromList(flags, ['ITk', 'HGTD', 'Calo', 'Muon'], validate_only=True)
# print()

# print("Test: enable ['ITkStrip']")
# assert setupDetectorsFromList(flags, ['ITkStrip'], toggle_geometry=True)
# print()
