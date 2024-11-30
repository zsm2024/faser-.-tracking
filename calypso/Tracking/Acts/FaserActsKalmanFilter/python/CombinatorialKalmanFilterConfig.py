# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations

from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvcCfg


def FaserActsAlignmentCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.FaserActsAlignmentCondAlg(name="FaserActsAlignmentCondAlg", **kwargs))
    return acc


def CombinatorialKalmanFilter_OutputCfg(flags):
    acc = ComponentAccumulator()
    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "TrackCollection#*",
                ]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc


def CombinatorialKalmanFilterCfg(flags, **kwargs):
    # acc = ComponentAccumulator()
    acc = FaserSCT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    # acc.merge(FaserActsAlignmentCondAlgCfg(flags))
    acts_tracking_geometry_svc = ActsTrackingGeometrySvcCfg(flags)
    acc.merge(acts_tracking_geometry_svc )

    # track_seed_tool = CompFactory.ClusterTrackSeedTool()
    track_seed_tool = CompFactory.ThreeStationTrackSeedTool()
    # track_seed_tool = CompFactory.ActsTrackSeedTool()
    # track_seed_tool = CompFactory.MyTrackSeedTool()
    sigma_loc0 = 1.9e-2
    sigma_loc1 = 9e-1
    sigma_phi = 3.3e-2
    sigma_theta = 2.9e-4
    p = 1000
    sigma_p = 0.1 * p
    sigma_qop = sigma_p / (p * p)
    initial_variance_inflation = [100, 100, 100, 100, 1000]
    track_seed_tool.covLoc0 = initial_variance_inflation[0] * sigma_loc0 * sigma_loc0
    track_seed_tool.covLoc1 = initial_variance_inflation[1] * sigma_loc1 * sigma_loc1
    track_seed_tool.covPhi = initial_variance_inflation[2] * sigma_phi * sigma_phi
    track_seed_tool.covTheta = initial_variance_inflation[3] * sigma_theta * sigma_theta
    track_seed_tool.covQOverP = initial_variance_inflation[4] * sigma_qop * sigma_qop
    track_seed_tool.std_cluster = 0.023
    track_seed_tool.origin = 0

    #seed_writer_tool = CompFactory.RootSeedWriterTool()
    #seed_writer_tool.noDiagnostics = kwargs["noDiagnostics"]
    #seed_writer_tool.MC = True

    trajectory_states_writer_tool = CompFactory.RootTrajectoryStatesWriterTool()
    trajectory_states_writer_tool.noDiagnostics = kwargs["noDiagnostics"]
    trajectory_states_writer_tool.MC = True

    trajectory_summary_writer_tool = CompFactory.RootTrajectorySummaryWriterTool()
    trajectory_summary_writer_tool.noDiagnostics = kwargs["noDiagnostics"]

    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 1000
    actsExtrapolationTool.TrackingGeometryTool = CompFactory.FaserActsTrackingGeometryTool("TrackingGeometryTool")
    performance_writer_tool = CompFactory.PerformanceWriterTool("PerformanceWriterTool")
    performance_writer_tool.noDiagnostics = kwargs["noDiagnostics"]
    performance_writer_tool.ExtrapolationTool = actsExtrapolationTool

    ckf = CompFactory.CombinatorialKalmanFilterAlg(**kwargs)
    ckf.TrackSeed = track_seed_tool
    ckf.ActsLogging = "INFO"
    ckf.noDiagnostics = kwargs["noDiagnostics"]
    #ckf.RootSeedWriterTool = seed_writer_tool
    ckf.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool
    ckf.RootTrajectorySummaryWriterTool = trajectory_summary_writer_tool
    ckf.PerformanceWriterTool = performance_writer_tool

    ckf.nMax = 10
    ckf.chi2Max = 25
    acc.addEventAlgo(ckf)
    acc.merge(CombinatorialKalmanFilter_OutputCfg(flags))
    return acc
