"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
# from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

# FaserActsKalmanFilterAlg,FaserActsTrackingGeometrySvc, FaserActsTrackingGeometryTool, FaserActsExtrapolationTool,FaserActsAlignmentCondAlg, THistSvc = CompFactory.getComps("FaserActsKalmanFilterAlg","FaserActsTrackingGeometrySvc", "FaserActsTrackingGeometryTool", "FaserActsExtrapolationTool","FaserActsAlignmentCondAlg", "THistSvc")
#
# def FaserActsTrackingGeometrySvcCfg(flags, **kwargs):
#     acc = ComponentAccumulator()
#     acc.addService(FaserActsTrackingGeometrySvc(name = "FaserActsTrackingGeometrySvc", **kwargs))
#     return acc
#
# def FaserActsAlignmentCondAlgCfg(flags, **kwargs):
#     acc = ComponentAccumulator()
#     acc.addCondAlgo(CompFactory.FaserActsAlignmentCondAlg(name = "FaserActsAlignmentCondAlg", **kwargs))
#     #acc.addCondAlgo(CompFactory.NominalAlignmentCondAlg(name = "NominalAlignmentCondAlg", **kwargs))
#     return acc
#
# def FaserActsKalmanFilterCfg(flags, **kwargs):
#     acc = ComponentAccumulator()
#     # Initialize field service
#     acc.merge(MagneticFieldSvcCfg(flags))
#
#     acc.merge(FaserActsTrackingGeometrySvcCfg(flags, **kwargs))
#     acc.merge(FaserActsAlignmentCondAlgCfg(flags, **kwargs))
#
#     kwargs.setdefault("FaserSpacePointsSeedsName", "Seeds_SpacePointContainer")
#     actsKalmanFilterAlg = FaserActsKalmanFilterAlg(**kwargs)
#     #actsKalmanFilterAlg.TrackingGeometryTool = FaserActsTrackingGeometryTool("TrackingGeometryTool")
#     actsExtrapolationTool=FaserActsExtrapolationTool("FaserActsExtrapolationTool")
#     actsExtrapolationTool.FieldMode="FASER"
#     #actsExtrapolationTool.FieldMode="Constant"
#     actsExtrapolationTool.TrackingGeometryTool=FaserActsTrackingGeometryTool("TrackingGeometryTool")
#     actsKalmanFilterAlg.ExtrapolationTool=actsExtrapolationTool
#     acc.addEventAlgo(actsKalmanFilterAlg)
#     histSvc= THistSvc()
#     histSvc.Output += [ "KalmanTracks DATAFILE='KalmanTracks.root' OPT='RECREATE'" ]
#     acc.addService(histSvc)
#     acc.merge(FaserActsKalmanFilter_OutputCfg(flags))
#     return  acc
#
# def FaserActsKalmanFilter_OutputCfg(flags):
#     """Return ComponentAccumulator with Output for SCT. Not standalone."""
#     acc = ComponentAccumulator()
#     acc.merge(OutputStreamCfg(flags, "ESD"))
#     ostream = acc.getEventAlgo("OutputStreamESD")
#     ostream.TakeItemsFromInput = True
#     return acc

def FaserActsKalmanFilter_OutputCfg(flags):
    acc = ComponentAccumulator()
    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "TrackCollection#*",
                ]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc

def FaserActsKalmanFilter_OutputAODCfg(flags):
    acc = ComponentAccumulator()
    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "FaserSCT_RDO_Container#*",
                "xAOD::FaserTriggerData#*",
                "xAOD::FaserTriggerDataAux#*",
                "ScintWaveformContainer#*",
                "TrackCollection#*",
                "xAOD::WaveformHitContainer#*",
                "xAOD::WaveformHitAuxContainer#*",
                "xAOD::WaveformClock#*",
                "xAOD::WaveformClockAuxInfo#*"
                ]
    acc.merge(OutputStreamCfg(flags, "AOD",itemList))
    ostream = acc.getEventAlgo("OutputStreamAOD")
    ostream.TakeItemsFromInput = True
    return acc


def FaserActsKalmanFilterCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    track_finder_tool = CompFactory.MultiTrackFinderTool()
    # track_finder_tool = CompFactory.SegmentFitClusterTrackFinderTool()
    # track_finder_tool = CompFactory.SegmentFitTrackFinderTool()
    sigma_loc0 = 1.9e-2
    sigma_loc1 = 9e-1
    sigma_phi = 3.3e-2
    sigma_theta = 2.9e-4
    # sigma_p_rel = 0.1
    p = 1
    sigma_qop = 0.1 * p / (p * p)
    initial_variance_inflation = [100, 100, 1000, 1000, 1000]
    track_finder_tool.covLoc0 = initial_variance_inflation[0] * sigma_loc1 * sigma_loc1
    track_finder_tool.covLoc1 = initial_variance_inflation[1] * sigma_loc0 * sigma_loc0
    track_finder_tool.covPhi = initial_variance_inflation[2] * sigma_phi * sigma_phi
    track_finder_tool.covTheta = initial_variance_inflation[3] * sigma_theta * sigma_theta
    track_finder_tool.covQOverP = initial_variance_inflation[4] * sigma_qop * sigma_qop
    # track_finder_tool.sigmaCluster = 0.04
    # track_finder_tool.covLoc0 = 1e-1
    # track_finder_tool.covLoc1 = 1e-1
    # track_finder_tool.covPhi = 1e-1
    # track_finder_tool.covTheta = 1e-1
    # track_finder_tool.covQOverP = 1e-1

    # track_finder_tool = CompFactory.TruthSeededTrackFinderTool()
    # track_finder_tool.useTrueInitialParameters = False
    # track_finder_tool.covMeas00 = 0.0004
    # track_finder_tool.covMeas01 = 0.01
    # track_finder_tool.covMeas10 = 0.01
    # track_finder_tool.covMeas11 = 0.64
    # track_finder_tool.covLoc0 = 1e-4
    # track_finder_tool.covLoc1 = 1e-4
    # track_finder_tool.covPhi = 1e-4
    # track_finder_tool.covTheta = 1e-4
    # track_finder_tool.covQOverP = 1e-4

    # track_finder_tool.covLoc0 = 1e-1
    # track_finder_tool.covLoc1 = 1e-1
    # track_finder_tool.covPhi = 1e-1
    # track_finder_tool.covTheta = 1e-1
    # track_finder_tool.covQOverP = 1e-1

    # track_finder_tool.sigmaLoc0 = 0.02
    # track_finder_tool.sigmaLoc1 = 0.8
    # track_finder_tool.sigmaPhi = 2 * pi/180
    # track_finder_tool.sigmaTheta = 2 * pi/180
    # track_finder_tool.sigmaP = 0.1

    # track_finder_tool = CompFactory.TruthTrackFinderTool()
    # track_finder_tool.first_side = 1
    # track_finder_tool.sigmaMeasLoc0 = 0.02
    # track_finder_tool.sigmaMeasLoc1 = 0.8
    # track_finder_tool.covMeasLoc0 = 0.02 * 0.02 * 10
    # track_finder_tool.covMeasLoc1 = 0.8 * 0.8 * 10
    # track_finder_tool.sigmaLoc0 = 0
    # track_finder_tool.sigmaLoc1 = 0
    # track_finder_tool.sigmaPhi = 0
    # track_finder_tool.sigmaTheta = 0
    # track_finder_tool.sigmaP = 0
    # track_finder_tool.covLoc0 = 1e-3
    # track_finder_tool.covLoc1 = 1e-3
    # track_finder_tool.covPhi = 1e-3
    # track_finder_tool.covTheta = 1e-3
    # track_finder_tool.covQOverP = 1e-3
    trajectory_writer_tool = CompFactory.TrajectoryWriterTool()
    trajectory_writer_tool.FilePath = "KalmanFilterTrajectories.root"
    # trajectory_states_writer_tool = CompFactory.RootTrajectoryStatesWriterTool()
    # trajectory_states_writer_tool.FilePath = "TrajectoryStates.root"
    kalman_filter = CompFactory.FaserActsKalmanFilterAlg(**kwargs)
    # kalman_filter.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool
    kalman_filter.OutputTool = trajectory_writer_tool
    kalman_filter.TrackFinderTool = track_finder_tool
    kalman_filter.ActsLogging = "VERBOSE"
    acc.addEventAlgo(kalman_filter)
    acc.merge(FaserActsKalmanFilter_OutputCfg(flags))
    # acc.merge(FaserActsKalmanFilter_OutputAODCfg(flags))
    return acc
