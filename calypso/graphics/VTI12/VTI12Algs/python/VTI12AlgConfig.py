# Copyright (C) 2002-2024 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from Campaigns.Utils import Campaign
from AthenaConfiguration.Enums import BeamType

# from AthenaConfiguration.Enums import Format


def configureGeometry(flags, cfg):
    if flags.Detector.GeometryEmulsion:
        from EmulsionGeoModel.EmulsionGeoModelConfig import EmulsionGeometryCfg
        cfg.merge(EmulsionGeometryCfg(flags))

    if flags.Detector.GeometryDipole:
        from DipoleGeoModel.DipoleGeoModelConfig import DipoleGeometryCfg
        cfg.merge(DipoleGeometryCfg(flags))

    if flags.Detector.GeometryVetoNu:
        from VetoNuGeoModel.VetoNuGeoModelConfig import VetoNuGeometryCfg
        cfg.merge(VetoNuGeometryCfg(flags))

    if flags.Detector.GeometryVeto:
        from VetoGeoModel.VetoGeoModelConfig import VetoGeometryCfg
        cfg.merge(VetoGeometryCfg(flags))

    if flags.Detector.GeometryTrigger:
        from TriggerGeoModel.TriggerGeoModelConfig import TriggerGeometryCfg
        cfg.merge(TriggerGeometryCfg(flags))

    if flags.Detector.GeometryPreshower:
        from PreshowerGeoModel.PreshowerGeoModelConfig import PreshowerGeometryCfg
        cfg.merge(PreshowerGeometryCfg(flags))

    if flags.Detector.GeometryFaserSCT:
        from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
        cfg.merge(FaserSCT_GeometryCfg(flags))

    if flags.Detector.GeometryEcal:
        from EcalGeoModel.EcalGeoModelConfig import EcalGeometryCfg
        cfg.merge(EcalGeometryCfg(flags))

    if flags.Detector.GeometryTrench:
        from FaserGeoModel.TrenchGMConfig import TrenchGeometryCfg
        cfg.merge(TrenchGeometryCfg(flags))

def getFASERVersion():
    import os

    if "FaserVersion" in os.environ:
        return os.environ["FaserVersion"]
    if "FaserBaseVersion" in os.environ:
        return os.environ["FaserBaseVersion"]
    return "Unknown"


def VTI12AlgCfg(flags, name="VTI12AlgCA", **kwargs):
    # This is based on a few old-style configuation files:
    # JiveXML_RecEx_config.py
    # JiveXML_jobOptionBase.py
    result = ComponentAccumulator()

    # kwargs.setdefault("FaserRelease", getFASERVersion())
    the_alg = CompFactory.VTI12Alg(name="VTI12EventDisplayAlg", **kwargs)
    result.addEventAlgo(the_alg, primary=True)
    return result


def SetupVTI12():
    from AthenaConfiguration.Enums import Format
    from AthenaCommon.Logging import logging
    from AthenaCommon.Constants import VERBOSE

    # ++++ Firstly we setup flags ++++
    _logger = logging.getLogger("VTI12")
    _logger.setLevel(VERBOSE)

    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    flags = initConfigFlags()
    flags.Concurrency.NumThreads = 0
    flags.Input.MCCampaign = Campaign.Unknown
    flags.Input.TypedCollections = []
    flags.Input.MetadataItems = []
    flags.Beam.Type = BeamType.Collisions
    flags.GeoModel.FaserVersion = "FASERNU-04"
    flags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

    # ^ VP1 will not work with the scheduler, since its condition/data dependencies are not known in advance
    # More in details: the scheduler needs to know BEFORE the event, what the dependencies of each Alg are.
    # So for VP1, no dependencies are declared, which means the conditions data is not there.
    # So when I load tracks, the geometry is missing and it crashes.
    # Turning off the scheduler (with NumThreads=0) fixes this.

    parser = flags.getArgumentParser()
    parser.prog = "vti12"
    parser.description = """
    VTI12, or Virtual TI 12, is the interactive 3D event display for the FASER experiment at CERN.
    It is based on the ATLAS event display VP1
    Detailed documentation can be found at the webpage: https://atlas-vp1.web.cern.ch/atlas-vp1
    but below are the flags that can be used to configure VP1 (most are standard Athena flags, but some are VP1-specific).
    """
    parser.description = """
    So for example, to run VTI12 on a file: vti12 [options] myESD.pool.root"""
    # Add VTI12-specific arguments here, but remember you can also directly pass flags in form <flagName>=<value>.
    group = parser.add_argument_group("VTI12 specific")
    group.add_argument(
        "Filename",
        nargs='?',
        help="Input file to pass to VTI12 (i.e. vti12 myESD.pool.root as an alternative to vti12 --filesInput=[])",
        metavar="File name",
    )
    group.add_argument(
        "--config",
        nargs="*",
        help="Config file to use for VTI12. If not specified, the default configuration will be used.",
    )
    group.add_argument(
        "--verboseAthena",
        action="store_true",
        help="If false, tell Athena to suppress INFO messages and below.",
    )
    # group.add_argument(
    #     "--online", action="store_true", help="Use this flag for running VP1 at P1."
    # )
    group.add_argument(
        "--cruise",
        type=int,
        help="Start in cruise mode, changing events after N seconds.",
    )
    # Batch
    # group.add_argument(
    #     "--batch",
    #     action="store_true",
    #     help="Run VTI12 in 'batch' mode with a given configuration file.",
    # )
    # group.add_argument(
    #     "--batch-all-events",
    #     action="store_true",
    #     help="Process all events in the input data file in '-batch' mode. Use this together with '-batch'.",
    # )
    # group.add_argument(
    #     "--batch-n-events",
    #     type=int,
    #     help="Process 'N' events in the input data file in '-batch' mode. Use this together with '-batch'.",
    # )
    # group.add_argument(
    #     "--batch-output-folder",
    #     help="Specify an output folder to store the event displays produced with the '-batch' option.",
    # )
    # group.add_argument(
    #     "--batch-random-config",
    #     action="store_true",
    #     help="Run VTI12 in 'batch' mode; for each single event a configuration file will be randomly picked out of the configuration files provided by the user. Use this together with '-batch'.",
    # )
    # Live / Livelocal
    # group.add_argument(
    #     "--live", action="store_true", help="Run on live events from point 1."
    # )
    # group.add_argument(
    #     "--livelocal",
    #     action="store_true",
    #     help="Run on live events from point 1 in local directory.",
    # )
    # group.add_argument(
    #     "--eventsrc",
    #     help="Directory to take single event files from (do not specify input files in this case). To get files from a web server (i.e. live events), put instead the url to the file residing in the same directory (most users should just use the --live option instead).",
    # )
    # group.add_argument(
    #     "--extraevtsrcdir",
    #     help="Directory to take single event files from (do not specify input files in this case). To get files from a web server (i.e. live events), put instead the url to the file residing in the same directory (most users should just use the -live option instead).",
    # )
    # group.add_argument(
    #     "--eventcpy",
    #     help="Directory to keep local copies of processed event files. If --eventsrc is set, then -eventcpy will default to /tmp/emoyse/vp1events/6897 .",
    # )
    # group.add_argument('--nocleanupeventcpy', action='store_true', help="Prevent removal of eventcpy directory after athena process finishes.")
    # Commented, because I'm not sure how to implement this safely.

    args = flags.fillFromArgs(parser=parser)

    if "help" in args:
        # No point doing more here, since we just want to print the help.
        import sys
        sys.exit()

    # Support the positional version of passing file name e.g. vp1 myESD.pool.root
    if args.Filename and (
        flags.Input.Files == []
        or flags.Input.Files == ["_CALYPSO_GENERIC_INPUTFILE_NAME_"]
    ):
        flags.Input.Files = [args.Filename]

    _logger.verbose("+ About to set flags related to the input")




    # Empty input is not normal for Athena, so we will need to check
    # this repeatedly below
    vti12_empty_input = False
    # This covers the use case where we launch VP1
    # without input files; e.g., to check the detector description
    if flags.Input.Files == [] or flags.Input.Files == [
        "_CALYPSO_GENERIC_INPUTFILE_NAME_"
    ]:
        from AthenaConfiguration.TestDefaults import defaultGeometryTags

        vti12_empty_input = True
        # NB Must set e.g. ConfigFlags.Input.Runparse_args() Number and
        # ConfigFlags.Input.TimeStamp before calling the
        # MainServicesCfg to avoid it attempting auto-configuration
        # from an input file, which is empty in this use case.
        # If you don't have it, it (and/or other Cfg routines) complains and crashes.
        # See also:
        # https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/InnerDetector/InDetConditions/SCT_ConditionsAlgorithms/python/SCT_DCSConditionsTestAlgConfig.py#0023
        flags.Input.ProjectName = "mc20_13TeV"
        flags.Input.RunNumbers = [330000]
        flags.Input.TimeStamps = [1]
        flags.Input.TypedCollections = []
        flags.Input.Format = Format.POOL

        # set default CondDB and Geometry version
        flags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
        flags.Input.isMC = True
        flags.Input.MCCampaign = Campaign.Unknown
        flags.GeoModel.FaserVersion = "FASERNU-04"
    else:
        # Now just check file exists, or fail gracefully
        from os import path

        for file in flags.Input.Files:
            if not path.exists(flags.Input.Files[0]):
                _logger.warning("Input file", file, "does not exist")
                import sys
                sys.exit(1)
    
        # Set the online flag if we are running at P1
    # if args.online:
    #     flags.Common.isOnline = args.online
    # elif "HLTP" in flags.IOVDb.GlobalTag:
    #     print(
    #         "HLTP detected in the global tag, but --online mode is not enabled. Enabling it now."
    #     )
    #     flags.Common.isOnline = True
    
    _logger.verbose("+ ... Input flags done")

    _logger.verbose("+ About to set the detector flags")
    # So we can now set up the geometry flags from the input
    from CalypsoConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(flags, toggle_geometry = True)
    # setupDetectorFlags(
    #     flags,
    #     None,
    #     use_metadata=not vp1_empty_input,
    #     toggle_geometry=True,
    #     keep_beampipe=True,
    # )
    _logger.verbose("+ ... Detector flags done")

    # finalize setting flags: lock them.
    flags.lock()

    # DEBUG -- inspect the flags
    flags.dump()
    # flags._loadDynaFlags('GeoModel')
    # flags._loadDynaFlags('Detector')
    # flags.dump('Detector.(Geometry|Enable)', True)

    # ++++ Now we setup the actual configuration ++++

    # NB Must have set ConfigFlags.Input.RunNumber and
    # ConfigFlags.Input.TimeStamp before calling to avoid
    # attempted auto-configuration from an input file.
    _logger.verbose("+ Setup main services, and input file reading")

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg

    cfg = MainServicesCfg(flags)

    if not vti12_empty_input:
        # Only do this if we have input files, otherwise flags will try to read metadata
        # Check if we are reading from POOL and setupo convertors if so
        if flags.Input.Format is Format.POOL:
            from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
            cfg.merge(PoolReadCfg(flags))
            # Check if running on legacy inputs
            if "EventInfo" not in flags.Input.Collections:
                from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
                cfg.merge(EventInfoCnvAlgCfg(flags))

            # from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
            # cfg.merge(TrackCollectionReadCfg(flags, "Tracks"))

            # from TrkConfig.AtlasExtrapolationEngineConfig import AtlasExtrapolationEngineCfg
            # AtlasExtrapolationEngine = cfg.getPrimaryAndMerge(AtlasExtrapolationEngineCfg(flags))
            # cfg.addPublicTool(AtlasExtrapolationEngine)

        if flags.Input.isMC:
            # AOD2xAOD Truth conversion
            from xAODTruthCnv.xAODTruthCnvConfig import GEN_AOD2xAODCfg

            cfg.merge(GEN_AOD2xAODCfg(flags))
    _logger.verbose("+ ... Main services done")

    _logger.verbose("+ About to setup geometry")
    configureGeometry(flags, cfg)
    _logger.verbose("+ ... Geometry done")

    # Setup some VP1 specific stuff
    vti12config = {}
    if not args.verboseAthena:
        # Suppress the output from Athena
        print("Suppressing most messages from Athena.")
        print("To see more, set the --verboseAthena flag to true.")
        msgService = cfg.getService("MessageSvc")
        msgService.OutputLevel = 4

    if args.cruise:
        vti12config.setdefault("InitialCruiseMode", "EVENT")
        vti12config.setdefault("InitialCruiseModePeriod", args.cruise)

    if args.config:
        print("Using config file", args.config)
        vti12config.setdefault("InitialInputVTI12Files", args.config)

    # Batch mode
    # if args.batch:
    #     setup_batch_mode(args)

    # Event copying and live
    # if args.eventsrc:
    #     vti12config.setdefault("MultipleFilesON", True)

    # if args.live or args.livelocal:
    #     setup_live_mode(args, vp1config)

    # configure VTI12
    cfg.merge(VTI12AlgCfg(flags, **vti12config))
    cfg.run()


# def setup_live_mode(args, vp1config):
#     vp1config.setdefault("MultipleFilesON", True)
#     if args.eventcpy:
#         vp1config.setdefault("vp1Alg.MFLocalCopyDir", args.eventcpy)
#     if args.extraevtsrcdir:
#         vp1config.setdefault("MFAvailableLocalInputDirectories", args.extraevtsrcdir)
#     if args.live:
#         vp1config.setdefault(
#             "MFSourceDir",
#             "https://atlas-live.cern.ch/event_files/L1MinBias/vp1fileinfo.txt",
#         )
#     elif args.livelocal:
#         vp1config.setdefault("MFSourceDir", "/VP1_events/")


# def setup_batch_mode(args):
#     # BATCH-MODE
#     # If "--batch" is True, then set the corresponding env var.
#     # The GUI of VP1 will not be shown, but the config file will be taken
#     # and in the end a render of the 3D window will be saved as PNG file.
#     import os

#     if args.batch:
#         os.putenv("VP1_BATCHMODE", "1")
#     if args.batch_all_events:
#         os.putenv("VP1_BATCHMODE_ALLEVENTS", "1")
#     if args.batch_n_events:
#         os.putenv("VP1_BATCHMODE_NEVENTS", str(args.batch_n_events))
#     if args.batch_output_folder:
#         os.putenv("VP1_BATCHMODE_OUT_FOLDER", args.batch_output_folder)
#     if args.batch_random_config:
#         os.putenv("VP1_BATCHMODE_RANDOMCONFIG", "1")


if __name__ == "__main__":
    # Run with e.g. 
    # python -m VTI12Algs.VTI12AlgConfig --filesInput=myESD_ca.pool.root
    SetupVTI12()
    import sys

    sys.exit()
