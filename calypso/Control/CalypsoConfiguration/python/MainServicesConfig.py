# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# from __future__ import print_function
# from AthenaConfiguration.ComponentFactory import CompFactory

# from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
# AthSequencer=CompFactory.AthSequencer

# def MainServicesMiniCfg(loopMgr='AthenaEventLoopMgr', masterSequence='AthAlgSeq'):
#     #Mininmal basic config, just good enough for HelloWorld and alike
#     cfg=ComponentAccumulator(CompFactory.AthSequencer(masterSequence,Sequential=True))
#     cfg.setAsTopLevel()
#     cfg.setAppProperty('TopAlg',['AthSequencer/'+masterSequence])
#     cfg.setAppProperty('MessageSvcType', 'MessageSvc')
#     cfg.setAppProperty('EventLoop', loopMgr)
#     cfg.setAppProperty('ExtSvcCreates', 'False')
#     cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
#     cfg.setAppProperty('JobOptionsType', 'NONE')
#     cfg.setAppProperty('JobOptionsPostAction', '')
#     cfg.setAppProperty('JobOptionsPreAction', '')
#     cfg.setAppProperty('PrintAlgsSequence', True)
#     return cfg
# Copyright (C) 2002-2024 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import INFO


def MainServicesMiniCfg(flags, loopMgr='AthenaEventLoopMgr', masterSequence='AthAlgSeq'):
    """Mininmal basic config, just good enough for HelloWorld and alike"""
    cfg = ComponentAccumulator(CompFactory.AthSequencer(masterSequence,Sequential=True))
    cfg.setAsTopLevel()
    cfg.setAppProperty('TopAlg',['AthSequencer/'+masterSequence])
    cfg.setAppProperty('MessageSvcType', 'MessageSvc')
    cfg.setAppProperty('EventLoop', loopMgr)
    cfg.setAppProperty('ExtSvcCreates', 'False')
    cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
    cfg.setAppProperty('JobOptionsType', 'NONE')
    cfg.setAppProperty('JobOptionsPostAction', '')
    cfg.setAppProperty('JobOptionsPreAction', '')
    cfg.setAppProperty('PrintAlgsSequence', flags.Exec.PrintAlgsSequence)
    return cfg

def AthenaEventLoopMgrCfg(flags):
    cfg = ComponentAccumulator()
    elmgr = CompFactory.AthenaEventLoopMgr()
    if flags.Input.OverrideRunNumber:
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        elmgr.EvtIdModifierSvc = cfg.getPrimaryAndMerge( EvtIdModifierSvcCfg(flags) ).name

    if flags.Common.isOverlay:
        if not flags.Overlay.DataOverlay:
            elmgr.RequireInputAttributeList = True
            elmgr.UseSecondaryEventNumber = True

    cfg.addService( elmgr )

    return cfg

def MessageSvcCfg(flags):
    cfg = ComponentAccumulator()
    msgsvc = CompFactory.MessageSvc()
    msgsvc.OutputLevel = flags.Exec.OutputLevel
    msgsvc.Format = "% F%{:d}W%C%7W%R%T %0W%M".format(flags.Common.MsgSourceLength)
    msgsvc.enableSuppression = flags.Common.MsgSuppression
    if flags.Common.ShowMsgStats:
        msgsvc.showStats = True
        from AthenaCommon.Constants import WARNING
        msgsvc.statLevel = WARNING

    # from AthenaConfiguration.Enums import ProductionStep
    # if flags.Common.ProductionStep not in [ProductionStep.Default, ProductionStep.Reconstruction, ProductionStep.Derivation]:
    #     msgsvc.Format = "% F%18W%S%7W%R%T %0W%M" # Temporary to match legacy configuration for serial simulation/digitization/overlay jobs
    if flags.Concurrency.NumThreads>0:
        msgsvc.Format = "% F%{:d}W%C%6W%R%e%s%8W%R%T %0W%M".format(flags.Common.MsgSourceLength)
    if flags.Exec.VerboseMessageComponents:
        msgsvc.verboseLimit=0
    if flags.Exec.DebugMessageComponents:
        msgsvc.debugLimit=0
    if flags.Exec.InfoMessageComponents:
        msgsvc.infoLimit=0
    if flags.Exec.WarningMessageComponents:
        msgsvc.warningLimit=0
    if flags.Exec.ErrorMessageComponents:
        msgsvc.errorLimit=0

    cfg.addService(msgsvc)
    return cfg

def addMainSequences(flags, cfg):
    """Add the standard sequences to cfg"""

    # Build standard sequences:
    AthSequencer = CompFactory.AthSequencer
    cfg.addSequence(AthSequencer('AthAlgEvtSeq', Sequential=True, StopOverride=True), parentName='AthMasterSeq')
    cfg.addSequence(AthSequencer('AthOutSeq', StopOverride=True), parentName='AthMasterSeq')

    cfg.addSequence(AthSequencer('AthBeginSeq', Sequential=True), parentName='AthAlgEvtSeq')
    cfg.addSequence(AthSequencer('AthAllAlgSeq', StopOverride=True), parentName='AthAlgEvtSeq')

    athAlgSeq = AthSequencer('AthAlgSeq', IgnoreFilterPassed=True, StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[])
    athCondSeq = AthSequencer('AthCondSeq',StopOverride=True)

    if flags.Concurrency.NumThreads==0:
        # For serial execution, we need the CondAlgs to execute first.
        cfg.addSequence(athCondSeq, parentName='AthAllAlgSeq')
        cfg.addSequence(athAlgSeq, parentName='AthAllAlgSeq')
    else:
        # In MT, the order of execution is irrelevant (determined by data deps).
        # We add the conditions sequence later such that the CondInputLoader gets
        # initialized after all other user Algorithms for MT, so the base classes
        # of data deps can be correctly determined.
        cfg.addSequence(athAlgSeq, parentName='AthAllAlgSeq')
        cfg.addSequence(athCondSeq, parentName='AthAllAlgSeq')

    cfg.addSequence(AthSequencer('AthEndSeq', Sequential=True), parentName='AthAlgEvtSeq')

    # Set up incident firing:
    AthIncFirerAlg = CompFactory.AthIncFirerAlg
    IncidentProcAlg = CompFactory.IncidentProcAlg

    previousPerfmonDomain = cfg.getCurrentPerfmonDomain()
    cfg.flagPerfmonDomain('Incidents')

    cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg", FireSerial=False, Incidents=['BeginEvent']),
                     sequenceName='AthBeginSeq')

    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),
                     sequenceName='AthBeginSeq')

    cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg', FireSerial=False, Incidents=['EndEvent']),
                     sequenceName="AthEndSeq")

    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),
                     sequenceName="AthEndSeq")

    # Should be after all other algorithms:
    cfg.addEventAlgo(AthIncFirerAlg('EndAlgorithmsFiringAlg', FireSerial=False, Incidents=['EndAlgorithms']),
                     sequenceName="AthMasterSeq")

    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg3'),
                     sequenceName="AthMasterSeq")

    cfg.flagPerfmonDomain(previousPerfmonDomain)

def MainServicesCfg(flags, LoopMgr='AthenaEventLoopMgr'):
    # Set the Python OutputLevel on the root logger
    from AthenaCommon.Logging import log
    log.setLevel(flags.Exec.OutputLevel)

    if flags.Exec.Interactive == "run":
        LoopMgr="PyAthenaEventLoopMgr"
        log.info("Interactive mode, switching to %s", LoopMgr)
    else:
        # Run a serial job for threads=0
        if flags.Concurrency.NumThreads > 0:
            if flags.Concurrency.NumConcurrentEvents==0:
                raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, "
                                "which will not process events!")
            if flags.Exec.MTEventService:
                LoopMgr = "AthenaMtesEventLoopMgr"
            else:
                LoopMgr = "AthenaHiveEventLoopMgr"

        if flags.Concurrency.NumProcs > 0:
            LoopMgr = "AthMpEvtLoopMgr"

    # Core components needed for serial and threaded jobs:
    cfg = MainServicesMiniCfg(flags, loopMgr=LoopMgr, masterSequence='AthMasterSeq')

    # Main sequences and incident handling:
    addMainSequences(flags, cfg)

    # Basic services:
    cfg.addService(CompFactory.ClassIDSvc(CLIDDBFiles = ['clid.db','Gaudi_clid.db']))

    cfg.addService(CompFactory.AlgContextSvc(BypassIncidents=True))
    cfg.addAuditor(CompFactory.AlgContextAuditor())

    cfg.addService(CompFactory.StoreGateSvc(Dump=flags.Debug.DumpEvtStore))
    cfg.addService(CompFactory.StoreGateSvc("DetectorStore",Dump=flags.Debug.DumpDetStore))
    cfg.addService(CompFactory.StoreGateSvc("HistoryStore"))
    cfg.addService(CompFactory.StoreGateSvc("ConditionStore",Dump=flags.Debug.DumpCondStore))

    cfg.merge(MessageSvcCfg(flags))

    from AthenaConfiguration.FPEAndCoreDumpConfig import FPEAndCoreDumpCfg
    cfg.merge(FPEAndCoreDumpCfg(flags))

    # Avoid stack traces to the exception handler. These traces
    # aren't very useful since they just point to the handler, not
    # the original bug.
    cfg.addService(CompFactory.ExceptionSvc(Catch="NONE"))

    # ApplicationMgr properties:
    cfg.setAppProperty('AuditAlgorithms', True)
    cfg.setAppProperty('InitializationLoopCheck', False)
    cfg.setAppProperty('EvtMax', flags.Exec.MaxEvents)
    if flags.Exec.OutputLevel > INFO:
        # this turns off the appMgr spalsh
        cfg.setAppProperty('AppName', '')
    cfg.setAppProperty('OutputLevel', flags.Exec.OutputLevel)

    if flags.Exec.DebugStage != "":
        cfg.setDebugStage(flags.Exec.DebugStage)

    cfg.interactive=flags.Exec.Interactive

    # if flags.Concurrency.NumProcs > 0:
    #     cfg.merge(AthenaMpEventLoopMgrCfg(flags))

    # Additional components needed for threaded jobs only:
    if flags.Concurrency.NumThreads > 0:
        # if flags.Exec.MTEventService:
        #     cfg.merge(AthenaMtesEventLoopMgrCfg(flags,True,flags.Exec.MTEventServiceChannel))
        # else:
        #     cfg.merge(AthenaHiveEventLoopMgrCfg(flags))
        # # Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
        cfg.addAuditor( CompFactory.SGCommitAuditor() )
    elif LoopMgr == 'AthenaEventLoopMgr':
        cfg.merge(AthenaEventLoopMgrCfg(flags))

    # # Performance monitoring and profiling:
    # if flags.PerfMon.doFastMonMT or flags.PerfMon.doFullMonMT:
    #     from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
    #     cfg.merge(PerfMonMTSvcCfg(flags))

    # if flags.PerfMon.doGPerfProf:
    #     from PerfMonGPerfTools.GPT_ProfilerServiceConfig import GPT_ProfilerServiceCfg
    #     cfg.merge(GPT_ProfilerServiceCfg(flags))

    # if len(flags.PerfMon.Valgrind.ProfiledAlgs)>0:
    #     from Valkyrie.ValkyrieConfig import ValgrindServiceCfg
    #     cfg.merge(ValgrindServiceCfg(flags))

    return cfg

# def MainServicesCfg(cfgFlags):
#     # Run a serial job for threads=0
#     LoopMgr = 'AthenaEventLoopMgr'
#     if cfgFlags.Concurrency.NumThreads>0:
#         if cfgFlags.Concurrency.NumConcurrentEvents==0:
#             # In a threaded job this will mess you up because no events will be processed
#             raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, which will not process events!")
#         LoopMgr = "AthenaHiveEventLoopMgr"

#     ########################################################################
#     # Core components needed for serial and threaded jobs
#     cfg=MainServicesMiniCfg(loopMgr=LoopMgr, masterSequence='AthMasterSeq')
#     cfg.setAppProperty('OutStreamType', 'AthenaOutputStream')

#     #Build standard sequences:
#     cfg.addSequence(AthSequencer('AthAlgEvtSeq',Sequential=True, StopOverride=True),parentName="AthMasterSeq") 
#     cfg.addSequence(AthSequencer('AthOutSeq',StopOverride=True),parentName="AthMasterSeq")

#     cfg.addSequence(AthSequencer('AthBeginSeq',Sequential=True),parentName='AthAlgEvtSeq')
#     cfg.addSequence(AthSequencer('AthAllAlgSeq',StopOverride=True),parentName='AthAlgEvtSeq') 

#     if cfgFlags.Concurrency.NumThreads==0:
#         # For serial execution, we need the CondAlgs to execute first.
#         cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')
#         cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[] ),parentName='AthAllAlgSeq')
#     else:
#         # In MT, the order of execution is irrelevant (determined by data deps).
#         # We add the conditions sequence later such that the CondInputLoader gets
#         # initialized after all other user Algorithms for MT, so the base classes
#         # of data deps can be correctly determined. 
#         cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True),parentName='AthAllAlgSeq')
#         cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[]),parentName='AthAllAlgSeq')

#     cfg.addSequence(AthSequencer('AthEndSeq',Sequential=True),parentName='AthAlgEvtSeq') 
#     cfg.setAppProperty('PrintAlgsSequence', True)
    
#     #Set up incident firing:
#     AthIncFirerAlg=CompFactory.AthIncFirerAlg
#     IncidentProcAlg=CompFactory.IncidentProcAlg

#     cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg",FireSerial=False,Incidents=['BeginEvent']),sequenceName='AthBeginSeq')
#     cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),sequenceName='AthBeginSeq')

#     cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg',FireSerial=False,Incidents=['EndEvent']), sequenceName="AthEndSeq")
#     cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),sequenceName="AthEndSeq")

#     #Basic services:
#     ClassIDSvc=CompFactory.ClassIDSvc
#     cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))

#     StoreGateSvc=CompFactory.StoreGateSvc
#     cfg.addService(StoreGateSvc())
#     cfg.addService(StoreGateSvc("DetectorStore"))
#     cfg.addService(StoreGateSvc("HistoryStore"))
#     cfg.addService(StoreGateSvc("ConditionStore"))

#     from FaserGeoModel.GeoModelConfig import GeoModelCfg
#     cfg.merge( GeoModelCfg(cfgFlags) )
#     cfg.addService(CompFactory.DetDescrCnvSvc(), create=True)
#     cfg.addService(CompFactory.CoreDumpSvc(), create=True)

#     cfg.setAppProperty('InitializationLoopCheck',False)

#     cfg.setAppProperty('EvtMax',cfgFlags.Exec.MaxEvents)

#     msgsvc=CompFactory.MessageSvc()
#     msgsvc.OutputLevel=cfgFlags.Exec.OutputLevel
#     cfg.addService(msgsvc)

#     if cfgFlags.Exec.DebugStage != "":
#         cfg.setDebugStage(cfgFlags.Exec.DebugStage)


#     ########################################################################
#     # Additional components needed for threaded jobs only
#     if cfgFlags.Concurrency.NumThreads>0:

#         # Migrated code from AtlasThreadedJob.py
#         AuditorSvc=CompFactory.AuditorSvc
#         msgsvc.defaultLimit = 0
#         msgsvc.Format = "% F%40W%S%4W%R%e%s%8W%R%T %0W%M"

#         SG__HiveMgrSvc=CompFactory.SG.HiveMgrSvc
#         hivesvc = SG__HiveMgrSvc("EventDataSvc")
#         hivesvc.NSlots = cfgFlags.Concurrency.NumConcurrentEvents
#         cfg.addService( hivesvc )

#         AlgResourcePool=CompFactory.AlgResourcePool
#         from AthenaCommon.Constants import INFO
#         arp=AlgResourcePool( OutputLevel = INFO )
#         arp.TopAlg=["AthMasterSeq"] #this should enable control flow
#         cfg.addService( arp )

#         AvalancheSchedulerSvc=CompFactory.AvalancheSchedulerSvc
#         scheduler = AvalancheSchedulerSvc()
#         scheduler.CheckDependencies    = cfgFlags.Scheduler.CheckDependencies
#         scheduler.ShowDataDependencies = cfgFlags.Scheduler.ShowDataDeps
#         scheduler.ShowDataFlow         = cfgFlags.Scheduler.ShowDataFlow
#         scheduler.ShowControlFlow      = cfgFlags.Scheduler.ShowControlFlow
#         scheduler.ThreadPoolSize       = cfgFlags.Concurrency.NumThreads
#         cfg.addService(scheduler)

#         SGInputLoader=CompFactory.SGInputLoader
#         # FailIfNoProxy=False makes it a warning, not an error, if unmet data
#         # dependencies are not found in the store.  It should probably be changed
#         # to True eventually.
#         inputloader = SGInputLoader (FailIfNoProxy = False)
#         cfg.addEventAlgo( inputloader, "AthAlgSeq" )
#         scheduler.DataLoaderAlg = inputloader.getName()

#         AthenaHiveEventLoopMgr=CompFactory.AthenaHiveEventLoopMgr

#         elmgr = AthenaHiveEventLoopMgr()
#         elmgr.WhiteboardSvc = "EventDataSvc"
#         elmgr.SchedulerSvc = scheduler.getName()
#         cfg.addService( elmgr )

#         # enable timeline recording
#         TimelineSvc=CompFactory.TimelineSvc
#         cfg.addService( TimelineSvc( RecordTimeline = True, Partial = False ) )

#         #
#         ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
#         #
#         SGCommitAuditor=CompFactory.SGCommitAuditor
#         cfg.addService( AuditorSvc(Auditors=[SGCommitAuditor().getFullJobOptName(),]))
#         cfg.setAppProperty("AuditAlgorithms", True)

#     return cfg
    
