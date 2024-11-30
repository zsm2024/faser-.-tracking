# Declare the package name:
atlas_subdir(FaserActsKalmanFilter)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Boost )
#set( Acts_DIR /home/tboeckh/Documents/acts/run )
#find_package( Acts REQUIRED COMPONENTS Core PATHS /home/tboeckh/Documents/acts/run NO_DEFAULT_PATH )
find_package( Acts COMPONENTS Core )


# Component(s) in the package:
atlas_add_library( FaserActsKalmanFilterLib
    PUBLIC_HEADERS FaserActsKalmanFilter
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
    AthenaKernel
    ActsCore
    TrackerIdentifier
    TrackerReadoutGeometry
    ActsInteropLib
    FaserActsGeometryLib
    FaserActsGeometryInterfacesLib
)

atlas_add_component(FaserActsKalmanFilter
    ActsTrackSeedTool.h
    CircleFit.h
    CircleFitTrackSeedTool.h
    CKF2.h
    # CKF2Alignment.h
    CombinatorialKalmanFilterAlg.h
    EffPlotTool.h
    FASERSourceLink.h
    TrackFitterFunction.h
    TrackFinderFunction.h
    FaserActsKalmanFilter/FaserActsGeometryContainers.h
    FaserActsKalmanFilterAlg.h
    FaserActsRecMultiTrajectory.h
    GhostBusters.h
    MyTrackSeedTool.h
    FaserActsKalmanFilter/IdentifierLink.h
    FaserActsKalmanFilter/IndexSourceLink.h
    FaserActsKalmanFilter/IFiducialParticleTool.h
    FaserActsKalmanFilter/ITrackFinderTool.h
    FaserActsKalmanFilter/ITrackSeedTool.h
    FaserActsKalmanFilter/ITrackTruthMatchingTool.h
    KalmanFitterTool.h
    LinearFit.h
#    ClusterTrackSeedTool.h
#    TruthTrackFinderTool.h
    FaserActsKalmanFilter/Measurement.h
#    MultiTrackFinderTool.h
    PerformanceWriterTool.h
    PlotHelpers.h
    ResPlotTool.h
    RootSeedWriteTool.h
    RootTrajectoryStatesWriterTool.h
    RootTrajectorySummaryWriterTool.h
    SeedingAlg.h
#    SegmentFitClusterTrackFinderTool.h
#    SegmentFitTrackFinderTool.h
    SimWriterTool.h
    SPSeedBasedInitialParameterTool.h
    SPSimpleInitialParameterTool.h
    SummaryPlotTool.h
    TrackClassification.h
    TrackSeedWriterTool.h
    TrackSelection.h
#todo    TrajectoryWriterTool.h
#    ProtoTrackWriterTool.h
    TruthBasedInitialParameterTool.h
#    TruthSeededTrackFinderTool.h
    ThreeStationTrackSeedTool.h
    src/ActsTrackSeedTool.cxx
    src/CircleFit.cxx
    src/CircleFitTrackSeedTool.cxx
    src/CKF2.cxx
    # src/CKF2Alignment.cxx
    src/CreateTrkTrackTool.h
    src/CreateTrkTrackTool.cxx
#    src/ClusterTrackSeedTool.cxx
#todo    src/CombinatorialKalmanFilterAlg.cxx
#todo    src/EffPlotTool.cxx
    src/TrackFitterFunction.cxx
    src/TrackFinderFunction.cxx
    src/FaserActsKalmanFilterAlg.cxx
    src/FiducialParticleTool.h
    src/FiducialParticleTool.cxx
    src/GhostBusters.cxx
    src/MyTrackSeedTool.cxx
    src/KalmanFitterTool.cxx
#    src/MultiTrackFinderTool.cxx
#todo    src/PerformanceWriterTool.cxx
    src/PlotHelpers.cxx
#todo    src/ResPlotTool.cxx
    src/SeedingAlg.cxx
    src/RootSeedWriteTool.cxx
    src/RootTrajectoryStatesWriterTool.cxx
    src/RootTrajectorySummaryWriterTool.cxx
#    src/SegmentFitClusterTrackFinderTool.cxx
#    src/SegmentFitTrackFinderTool.cxx
    src/SimWriterTool.cxx
#todo    src/SPSeedBasedInitialParameterTool.cxx
#todo    src/SPSimpleInitialParameterTool.cxx
#    src/ProtoTrackWriterTool.cxx
#todo    src/TrajectoryWriterTool.cxx
    src/TruthBasedInitialParameterTool.cxx
    src/SummaryPlotTool.cxx
    src/TrackClassification.cxx
    src/TrackSeedWriterTool.cxx
    src/TrackSelection.cxx
    src/TrackTruthMatchingTool.h
    src/TrackTruthMatchingTool.cxx
#    src/TruthTrackFinderTool.cxx
#    src/TruthSeededTrackFinderTool.cxx
    src/ThreeStationTrackSeedTool.cxx
    src/components/FaserActsKalmanFilter_entries.cxx
    PUBLIC_HEADERS FaserActsKalmanFilter
    INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
    LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
    ActsCore
    EventInfo
    FaserActsKalmanFilterLib
    ActsInteropLib
    FaserActsGeometryLib
    FaserActsGeometryInterfacesLib
    MagFieldInterfaces
    MagFieldElements
    MagFieldConditions
    TrkPrepRawData
    TrkTrack
    TrackerPrepRawData
    TrackerRIO_OnTrack
    TrkRIO_OnTrack
    TrackerSpacePoint
    GeoModelFaserUtilities
    GeneratorObjects
    TrackerIdentifier
    TrackerReadoutGeometry
    Identifier
    TrackerSimEvent
    TrackerSimData
    TrackerSeedFinderLib
)

# Install files from the package:
# atlas_install_headers(FaserActsKalmanFilter)
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
