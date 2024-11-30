def getNEvents(fname, maxEvents):
    "Work out how many events are in the file"

    n = 0
    with open(fname) as f:
        n = sum(1 for l in f if l.startswith("E "))

    if maxEvents != -1 and n > maxEvents:
        n = maxEvents

    print ("Setting Maximum events to", n)
    return n


if __name__ == "__main__":

    import sys

    doShiftLOS = True
    
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    configFlags = initConfigFlags()
    configFlags.Input.RunNumber = [12345]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumber = [1]

    configFlags.Input.isMC = True
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Detector.EnableFaserSCT = True

    configFlags.Output.EVNTFileName = "myEVNT.pool.root"

    configFlags.Exec.MaxEvents= -1

    import sys
    configFlags.fillFromArgs(sys.argv[1:])


    configFlags.Exec.MaxEvents = getNEvents(configFlags.Input.Files[0], configFlags.Exec.MaxEvents)    

    configFlags.lock()

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(configFlags)

    from HEPMCReader.HepMCReaderConfig import HepMCReaderCfg

    if doShiftLOS:
        cfg.merge(HepMCReaderCfg(configFlags, McEventKey = "BeamTruthEvent_ATLASCoord"))
    else:
        cfg.merge(HepMCReaderCfg(configFlags))

    if doShiftLOS:
        import McParticleEvent.Pythonizations
        from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg
        cfg.merge(ShiftLOSCfg(configFlags, 
                              xcross = 0, 
                              ycross = -150e-6,
                              xshift = 0,
                              yshift = 12))

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(configFlags))    

    itemList = [ "EventInfo#McEventInfo", "McEventCollection#*" ]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(configFlags, "EVNT", itemList, disableEventTag = True))

    sc = cfg.run()
    sys.exit(not sc.isSuccess())
