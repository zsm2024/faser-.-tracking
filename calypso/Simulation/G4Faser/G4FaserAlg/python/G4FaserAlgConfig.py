# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS and FASER collaborations
from G4FaserServices.G4FaserServicesConfig import DetectorGeometrySvcCfg, FaserPhysicsListSvcCfg
from FaserISF_Services.FaserISF_ServicesConfigNew import FaserTruthServiceCfg, FaserInputConverterCfg
from FaserISF_Services.FaserISF_ServicesCoreConfigNew import FaserGeoIDSvcCfg
from G4FaserTools.G4FaserToolsConfigNew import SensitiveDetectorMasterToolCfg
# from G4FaserTools.G4FaserToolsConfigNew import FastSimulationMasterToolCfg
from G4FaserServices.G4FaserUserActionConfig import FaserUserActionSvcCfg
# from G4FaserApps.G4Faser_MetadataNew import writeSimulationParametersMetadata
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg, outputStreamName


def G4FaserAlgBasicCfg(ConfigFlags, name="G4FaserAlg", **kwargs):
    """Return ComponentAccumulator configured for Faser G4 simulation, without output"""
    # wihout output
    result = DetectorGeometrySvcCfg(ConfigFlags)
    kwargs.setdefault("DetGeoSvc", result.getService("DetectorGeometrySvc"))

    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent") #tocheck -are these string inputs?
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos

    ## Don"t drop the GeoModel
    kwargs.setdefault("ReleaseGeoModel", ConfigFlags.Sim.ReleaseGeoModel)

    ## Record the particle flux during the simulation
    kwargs.setdefault("RecordFlux", ConfigFlags.Sim.RecordFlux)

    if ConfigFlags.Sim.FlagAbortedEvents:
        ## default false
        kwargs.setdefault("FlagAbortedEvents", ConfigFlags.Sim.FlagAbortedEvents)
        if ConfigFlags.Sim.FlagAbortedEvents and ConfigFlags.Sim.KillAbortedEvents:
            print("WARNING When G4FaserAlg.FlagAbortedEvents is True G4FaserAlg.KillAbortedEvents should be False. Setting G4FaserAlg.KillAbortedEvents = False now.")
            kwargs.setdefault("KillAbortedEvents", False)

    ## default true
    kwargs.setdefault("KillAbortedEvents", ConfigFlags.Sim.KillAbortedEvents)

    from RngComps.RngCompsConfig import AthRNGSvcCfg
    kwargs.setdefault("AtRndmGenSvc",
                      result.getPrimaryAndMerge(AthRNGSvcCfg(ConfigFlags)).name)

    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    #is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    is_hive = ConfigFlags.Concurrency.NumThreads > 0
    kwargs.setdefault("MultiThreading", is_hive)

    accMCTruth = FaserTruthServiceCfg(ConfigFlags)
    result.merge(accMCTruth)
    kwargs.setdefault("TruthRecordService", result.getService("FaserISF_TruthService"))
    #kwargs.setdefault("TruthRecordService", ConfigFlags.Sim.TruthStrategy) # TODO need to have manual override (simFlags.TruthStrategy.TruthServiceName())

    # accGeoID = FaserGeoIDSvcCfg(ConfigFlags)
    # result.merge(accGeoID)
    # kwargs.setdefault("GeoIDSvc", result.getService("ISF_FaserGeoIDSvc"))

    #input converter
    accInputConverter = FaserInputConverterCfg(ConfigFlags)
    result.merge(accInputConverter)
    kwargs.setdefault("InputConverter", result.getService("ISF_FaserInputConverter"))

    #sensitive detector master tool
    SensitiveDetector = result.popToolsAndMerge(SensitiveDetectorMasterToolCfg(ConfigFlags))
    result.addPublicTool(SensitiveDetector)
    kwargs.setdefault("SenDetMasterTool", result.getPublicTool(SensitiveDetector.name))

    #fast simulation master tool
    # FastSimulation = result.popToolsAndMerge(FastSimulationMasterToolCfg(ConfigFlags))
    # result.addPublicTool(FastSimulation)
    # kwargs.setdefault("FastSimMasterTool", result.getPublicTool(FastSimulation.name))

    #Write MetaData container
    # result.merge(writeSimulationParametersMetadata(ConfigFlags))

    #User action services (Slow...)
    result.merge( FaserUserActionSvcCfg(ConfigFlags) )
    kwargs.setdefault("UserActionSvc", result.getService( "G4UA::FaserUserActionSvc") )

    #PhysicsListSvc
    result.merge( FaserPhysicsListSvcCfg(ConfigFlags) )
    kwargs.setdefault("PhysicsListSvc", result.getService( "FaserPhysicsListSvc") )

    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    # FIXME GaudiConfig2 seems to fail to distinguish an empty dict {} from None
    verbosities=dict(foo="bar")
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]="1"
    #    print verbosities
    kwargs.setdefault("Verbosities", verbosities)

    # Configure GDML output; crashes if file already exists
    # kwargs.setdefault("GDMLfile", "Faser.gdml")

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", ConfigFlags.Sim.G4Commands)
    kwargs.setdefault("ExtraOutputs", SimHitContainerListCfg(ConfigFlags))

    result.addEventAlgo(CompFactory.G4FaserAlg(name, **kwargs))

    return result


def SimHitContainerListCfg(flags):
    extraOutput = []
    if flags.Detector.EnableEmulsion:
        extraOutput += [("NeutrinoHitCollection","EmulsionHits")]

    if flags.Detector.EnableFaserSCT:
        extraOutput += [("FaserSiHitCollection","SCT_Hits")]

    if flags.Detector.EnableEcal:
        extraOutput += [("CaloHitCollection","EcalHits")]

    if flags.Detector.EnableVeto:
        extraOutput += [("ScintHitCollection","VetoHits")]

    if flags.Detector.EnableVetoNu:
        extraOutput += [("ScintHitCollection","VetoNuHits")]

    if flags.Detector.EnableTrigger:
        extraOutput += [("ScintHitCollection","TriggerHits")]

    if flags.Detector.EnablePreshower:
        extraOutput += [("ScintHitCollection","PreshowerHits")]
    return extraOutput



def getG4FaserAlgItemList(ConfigFlags):
    """
    Return ComponentAccumulator with output for G4 simulation. Not standalone.

    follows G4Atlas.flat.configuration.py
    """
    ItemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#EventInfoAux.",
                "McEventCollection#TruthEvent"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#BeamTruthEvent"]
        ItemList += ["McEventCollection#GEN_EVENT"]

    if ConfigFlags.Detector.EnableEmulsion:
        ItemList += ["NeutrinoHitCollection#*"]

    if ConfigFlags.Detector.EnableFaserSCT:
        ItemList += ["FaserSiHitCollection#*"]

    if ConfigFlags.Detector.EnableEcal:
        ItemList += ["CaloHitCollection#*"]

    if ConfigFlags.Detector.EnableVeto:
        ItemList += ["ScintHitCollection#VetoHits"]

    if ConfigFlags.Detector.EnableVetoNu:
        ItemList += ["ScintHitCollection#VetoNuHits"]

    if ConfigFlags.Detector.EnableTrigger:
        ItemList += ["ScintHitCollection#TriggerHits"]

    if ConfigFlags.Detector.EnablePreshower:
        ItemList += ["ScintHitCollection#PreshowerHits"]

    # TimingAlg
    # ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

    # acc = OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList, disableEventTag=True)

    # # Make stream aware of aborted events
    # OutputStreamHITS = acc.getEventAlgo(outputStreamName("HITS"))
    # OutputStreamHITS.AcceptAlgs += ["G4FaserAlg"]

    # G4Atlas.flat.configuration.py#0333 onwards
    # FIXME unifyset now fails
    #PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    #PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    #acc.addService(CompFactory.AthenaPoolCnvSvc(PoolAttributes=PoolAttributes))

    # return acc
    return ItemList


def G4FaserAlgCfg(ConfigFlags, name="G4FaserAlg", **kwargs):
    """Return ComponentAccumulator configured for Faser G4 simulation, with output"""
    acc = G4FaserAlgBasicCfg(ConfigFlags, **kwargs)
    return acc
