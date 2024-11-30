atlas_subdir(FaserActsVertexing)

find_package( Eigen )
find_package( Acts COMPONENTS Core )

atlas_add_library( FaserActsmanVertexingLib
  PUBLIC_HEADERS FaserActsVertexing
  INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
  LINK_LIBRARIES ${EIGEN_LIBRARIES}
  ActsCore
  AthenaBaseComps
  FaserActsGeometryLib
  FaserActsKalmanFilterLib
  MagFieldConditions
  StoreGateLib
  TrackerPrepRawData
  TrackerRIO_OnTrack
  TrackerSimEvent
  TrackerSpacePoint
  TrkTrack
  xAODTruth
)

atlas_add_component(
        FaserActsVertexing
        FaserActsVertexing/IVertexingTool.h
        src/PointOfClosestApproachSearchTool.h
        src/PointOfClosestApproachSearchTool.cxx
        src/components/FaserActsVertexing_entries.cxx
        INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
        LINK_LIBRARIES ${EIGEN_LIBRARIES}
        ActsCore
        AthenaBaseComps
        FaserActsGeometryLib
        FaserActsKalmanFilterLib
        MagFieldConditions
        StoreGateLib
        TrackerPrepRawData
        TrackerRIO_OnTrack
        TrackerSimEvent
        TrackerSpacePoint
        TrkTrack
        xAODTruth
)
