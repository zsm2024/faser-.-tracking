# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from CalypsoConfiguration.AutoConfigFlags import getDefaultDetectors
# This module is based upon Control/AthenaCommon/python/DetFlags.py
# Only some flags have been migrated. 


allDetectors = [
    'Emulsion', 'Veto', 'Trigger', 'Preshower', 'VetoNu', 'FaserSCT', 'Ecal', 'Dipole', 'Trench'
]

allGroups = {
    'Neutrino' : [ 'Emulsion' ],
    'Tracker' : ['SCT'],
    'Scintillator' : ['Veto', 'Trigger', 'Preshower', 'VetoNu'],
    'FaserCalo'  : ['Ecal'],
    'Magnet'     : ['Dipole'],
    'Cavern'     : ['Trench']
}

def createDetectorConfigFlags():
    dcf=AthConfigFlags()

    #Detector.Geometry - merger of the old geometry and detdescr tasks

    dcf.addFlag('Detector.GeometryEmulsion',       False)
    dcf.addFlag('Detector.GeometryNeutrino',       lambda prevFlags : (prevFlags.Detector.GeometryEmulsion))

    dcf.addFlag('Detector.GeometryDipole',          False)
    dcf.addFlag('Detector.GeometryMagnet',          lambda prevFlags : (prevFlags.Detector.GeometryDipole ))
    
    dcf.addFlag('Detector.GeometryVeto',            False)
    dcf.addFlag('Detector.GeometryTrigger',         False)
    dcf.addFlag('Detector.GeometryPreshower',       False)
    dcf.addFlag('Detector.GeometryVetoNu',          False)
    dcf.addFlag('Detector.GeometryScintillator',    lambda prevFlags : (prevFlags.Detector.GeometryVeto or
                                                                        prevFlags.Detector.GeometryTrigger or
                                                                        prevFlags.Detector.GeometryPreshower or
                                                                        prevFlags.Detector.GeometryVetoNu))
    dcf.addFlag('Detector.GeometryFaserSCT',        False)
    dcf.addFlag('Detector.GeometryTracker',         lambda prevFlags : prevFlags.Detector.GeometryFaserSCT )
    
    dcf.addFlag('Detector.GeometryEcal',            False)
    dcf.addFlag('Detector.GeometryFaserCalo',       lambda prevFlags : prevFlags.Detector.GeometryEcal)

    # Trench (disabled by default)
    dcf.addFlag('Detector.GeometryTrench',False)
    dcf.addFlag('Detector.GeometryFaserCavern',     lambda prevFlags : (prevFlags.Detector.GeometryTrench ))

    # dcf.addFlag('Detector.GeometryFaser',           lambda prevFlags : (prevFlags.Detector.GeometryDecayVolume or
    #                                                                     prevFlags.Detector.GeometryScintillator or
    #                                                                     prevFlags.Detector.GeometryTracker or
    #                                                                     prevFlags.Detector.GeometryFaserCalo))



    #Detector.Enable
    # dcf.addFlag('Detector.EnableUpstreamDipole',  False)
    # dcf.addFlag('Detector.EnableCentralDipole',   False)
    # dcf.addFlag('Detector.EnableDownstreamDipole',False)
    # dcf.addFlag('Detector.EnableDipole',          lambda prevFlags : (prevFlags.Detector.EnableUpstreamDipole or
    #                                                                     prevFlags.Detector.EnableCentralDipole or
    #                                                                     prevFlags.Detector.EnableDownstreamDipole))
    # dcf.addFlag('Detector.EnableDecayVolume', False)

    dcf.addFlag('Detector.EnableEmulsion',   lambda prevFlags : 'Emulsion' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableNeutrino',   lambda prevFlags : (prevFlags.Detector.EnableEmulsion))

    dcf.addFlag('Detector.EnableVeto',        lambda prevFlags : 'Veto' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableTrigger',     lambda prevFlags : 'Trigger' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnablePreshower',   lambda prevFlags : 'Preshower' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableVetoNu',      lambda prevFlags : 'VetoNu' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableScintillator',lambda prevFlags : (prevFlags.Detector.EnableVeto or
                                                                    prevFlags.Detector.EnableTrigger or
                                                                    prevFlags.Detector.EnablePreshower or
                                                                    prevFlags.Detector.EnableVetoNu))
    dcf.addFlag('Detector.EnableFaserSCT',    lambda prevFlags : 'FaserSCT' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableTracker',     lambda prevFlags : prevFlags.Detector.EnableFaserSCT )
    dcf.addFlag('Detector.EnableEcal',        lambda prevFlags : 'Ecal' in getDefaultDetectors(prevFlags.GeoModel.FaserVersion))
    dcf.addFlag('Detector.EnableFaserCalo',   lambda prevFlags : prevFlags.Detector.EnableEcal)
    # dcf.addFlag('Detector.EnableFaser',       lambda prevFlags : (prevFlags.Detector.EnableDecayVolume or
    #                                                                 prevFlags.Detector.EnableScintillator or 
    #                                                                 prevFlags.Detector.EnableTracker or
    #                                                                 prevFlags.Detector.EnableFaserCalo))

    # Reco flags
    dcf.addFlag('Detector.RecoEmulsion', False)
    dcf.addFlag('Detector.RecoNeutrino', lambda prevFlags : (prevFlags.Detector.RecoEmulsion))

    dcf.addFlag('Detector.RecoVeto', False)
    dcf.addFlag('Detector.RecoTrigger', False)
    dcf.addFlag('Detector.RecoPreshower', False)
    dcf.addFlag('Detector.RecoEcal', False)
    dcf.addFlag('Detector.RecoVetoNu', False)
    dcf.addFlag('Detector.RecoWaveform', lambda prevFlags : (prevFlags.Detector.RecoVeto or prevFlags.Detector.RecoTrigger or 
                                                             prevFlags.Detector.RecoPreshower or prevFlags.Detector.RecoVetoNu or prevFlags.Detector.RecoEcal))
    dcf.addFlag('Detector.RecoFaserSCT', False)
    dcf.addFlag('Detector.RecoTracker', lambda prevFlags : (prevFlags.Detector.RecoFaserSCT))

    return dcf

def _parseDetectorsList(flags, detectors):
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # load flags
    flags._loadDynaFlags('Detector')
    # first check if we have groups
    groups = [d for d in detectors if d in allGroups.keys()]
    detectors = [d for d in detectors if d not in allGroups.keys()]
    for g in groups:
        log.debug("Evaluating detector group '%s'", g)
        # in case of groups only enable defaults
        for d in allGroups[g]:
            if d in getDefaultDetectors(flags.GeoModel.FaserVersion):
                log.debug("Appending detector '%s'", d)
                detectors.append(d)

    # check if valid detectors are required
    for d in detectors:
        if d not in allDetectors:
            log.warning("Unknown detector '%s'", d)

    return detectors

def setupDetectorsFromList(flags, detectors = allDetectors, toggle_geometry=False, validate_only=False):
    """Setup (toggle) detectors from a pre-defined list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # print summary
    if validate_only:
        log.info('Required detectors: %s', detectors)
    else:
        log.info('Setting detectors to: %s', detectors)

    # go through all of the detectors and check what should happen
    for d in allDetectors:
        status = d in detectors
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags[name] != status:
                changed = True
                if validate_only:
                    log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                else:
                    log.info("Toggling '%s' from %s to %s", name, not status, status)
                    flags._set(name, status)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags[name] != status:
                    changed = True
                    if validate_only:
                        log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                    else:
                        log.info("Toggling '%s' from %s to %s", name, not status, status)
                        flags._set(name, status)

    return changed

def enableDetectors(flags, detectors, toggle_geometry=False):
    """Enable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Enabling detectors: %s', detectors)

    # go through all of the detectors and enable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags[name] is not True:
                changed = True
                log.info("Enabling '%s'", name)
                flags._set(name, True)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags[name] is not True:
                    changed = True
                    log.info("Enabling '%s'", name)
                    flags._set(name, True)

    return changed


def disableDetectors(flags, detectors, toggle_geometry=False):
    """Disable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Disabling detectors: %s', detectors)

    # go through all of the detectors and disable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags[name] is not False:
                changed = True
                log.info("Disabling '%s'", name)
                flags._set(name, False)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags[name] is not False:
                    changed = True
                    log.info("Disabling '%s'", name)
                    flags._set(name, False)

    return changed

def modifyDetectorConfigFlags(flags):
    # disable problematic ATLAS flags by hand
    # flags.Detector.GeometryCSC = False
    # flags.Detector.GeometrysTGC = False
    # flags.Detector.GeometryMM = False

    # flags.Detector.EnableBCM = False
    # flags.Detector.EnableDBM = False
    # flags.Detector.EnablePixel = False
    # flags.Detector.EnableSCT = False
    # flags.Detector.EnableTRT = False

    # Upgrade ITk Inner Tracker is a separate and parallel detector
    # flags.Detector.EnableBCMPrime = False
    # flags.Detector.EnableITkPixel = False
    # flags.Detector.EnableITkStrip = False

    # flags.Detector.EnableHGTD = False

    # Calorimeters
    # flags.Detector.EnableLAr = False
    # flags.Detector.EnableTile = False

    # Muon Spectrometer
    # flags.Detector.EnableCSC = False 
    # flags.Detector.EnableMDT = False
    # flags.Detector.EnableRPC = False
    # flags.Detector.EnableTGC = False
    # flags.Detector.EnablesTGC = False
    # flags.Detector.EnableMM = False

    return flags