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


def CKF2_OutputCfg(flags):
    acc = ComponentAccumulator()
    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "TrackCollection#*",
                ]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc


def TI12CKF2Cfg(flags, **kwargs):
    # acc = ComponentAccumulator()
    acc = FaserSCT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    # acc.merge(FaserActsAlignmentCondAlgCfg(flags))
    acts_tracking_geometry_svc = ActsTrackingGeometrySvcCfg(flags)
    acc.merge(acts_tracking_geometry_svc )

    # track_seed_tool = CompFactory.ClusterTrackSeedTool()
    # track_seed_tool = CompFactory.ActsTrackSeedTool()
    # track_seed_tool = CompFactory.MyTrackSeedTool()
    # track_seed_tool = CompFactory.ThreeStationTrackSeedTool()
    track_seed_tool = CompFactory.CircleFitTrackSeedTool()
    sigma_loc0 = 1.9e-2
    sigma_loc1 = 9e-1
    sigma_phi = 3.3e-2
    sigma_theta = 2.9e-4
    p = 1000
    sigma_p = 0.1 * p
    sigma_qop = sigma_p / (p * p)
    # initial_variance_inflation = [1000, 1000, 100, 100, 10000]
    initial_variance_inflation = [100, 100, 100, 100, 1000]
    track_seed_tool.covLoc0 = initial_variance_inflation[0] * sigma_loc1 * sigma_loc1
    track_seed_tool.covLoc1 = initial_variance_inflation[1] * sigma_loc0 * sigma_loc0
    track_seed_tool.covPhi = initial_variance_inflation[2] * sigma_phi * sigma_phi
    track_seed_tool.covTheta = initial_variance_inflation[3] * sigma_theta * sigma_theta
    track_seed_tool.covQOverP = initial_variance_inflation[4] * sigma_qop * sigma_qop
    track_seed_tool.std_cluster = 0.0231
    track_seed_tool.TrackCollection = "Segments"

    trajectory_states_writer_tool = CompFactory.RootTrajectoryStatesWriterTool()
    trajectory_states_writer_tool.noDiagnostics = kwargs["noDiagnostics"]
    trajectory_states_writer_tool1 = CompFactory.RootTrajectoryStatesWriterTool()
    trajectory_states_writer_tool1.noDiagnostics = kwargs["noDiagnostics"]
    trajectory_states_writer_tool1.FilePath = "track_states_ckf1.root"

    trajectory_summary_writer_tool = CompFactory.RootTrajectorySummaryWriterTool()
    trajectory_summary_writer_tool.noDiagnostics = kwargs["noDiagnostics"]
    trajectory_summary_writer_tool1 = CompFactory.RootTrajectorySummaryWriterTool()
    trajectory_summary_writer_tool1.FilePath = "track_summary_ckf1.root"
    trajectory_summary_writer_tool1.noDiagnostics = kwargs["noDiagnostics"]

    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 1000
    actsExtrapolationTool.TrackingGeometryTool = CompFactory.FaserActsTrackingGeometryTool("TrackingGeometryTool")

    # trajectory_performance_writer_tool = CompFactory.PerformanceWriterTool("PerformanceWriterTool", **kwargs)
    # trajectory_performance_writer_tool.ExtrapolationTool = actsExtrapolationTool
    # trajectory_performance_writer_tool.noDiagnostics = kwargs["noDiagnostics"]


    ckf = CompFactory.CKF2(**kwargs)
    kalman_fitter1 = CompFactory.KalmanFitterTool(name="fitterTool1")
    kalman_fitter1.noDiagnostics = kwargs["noDiagnostics"]
    kalman_fitter1.ActsLogging = "INFO"
    kalman_fitter1.SummaryWriter = True
    kalman_fitter1.StatesWriter = True
    kalman_fitter1.SeedCovarianceScale = 10
    kalman_fitter1.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool1
    kalman_fitter1.RootTrajectorySummaryWriterTool = trajectory_summary_writer_tool1
    ckf.KalmanFitterTool1 = kalman_fitter1

    kalman_fitter2 = CompFactory.KalmanFitterTool(name="fitterTool2", **kwargs)
    kalman_fitter2.noDiagnostics = kwargs["noDiagnostics"]
    kalman_fitter2.ActsLogging = "INFO"
    kalman_fitter2.SummaryWriter = True
    kalman_fitter2.StatesWriter = True
    kalman_fitter2.SeedCovarianceScale = 1
    kalman_fitter2.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool2
    kalman_fitter2.RootTrajectorySummaryWriterTool = trajectory_summary_writer_tool2
    ckf.KalmanFitterTool2 = kalman_fitter2

    ckf.TrackSeed = track_seed_tool
    ckf.ActsLogging = "INFO"
    ckf.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool
    ckf.RootTrajectorySummaryWriterTool = trajectory_summary_writer_tool
    ckf.SummaryWriter = True
    ckf.StatesWriter = True
    ckf.PerformanceWriter = False
    # ckf.PerformanceWriterTool = trajectory_performance_writer_tool

    ckf.nMax = 10
    ckf.chi2Max = 100000
    acc.addEventAlgo(ckf)
    acc.merge(CKF2_OutputCfg(flags))
    return acc
