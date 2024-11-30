#pragma once

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include <cassert>
#include <boost/container/flat_map.hpp>
#include "FaserActsKalmanFilter/FaserActsGeometryContainers.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"

using Index = uint32_t;

class FASERSourceLink final {
public:
  /// Construct from geometry identifier and index.
  constexpr FASERSourceLink(Acts::GeometryIdentifier gid, Index idx, Tracker::FaserSCT_Cluster hit)
      : m_geometryId(gid), m_index(idx), m_hit(hit) {}

  // Construct an invalid source link. Must be default constructible to
  /// satisfy SourceLinkConcept.
  FASERSourceLink() = default;
  FASERSourceLink(const FASERSourceLink&) = default;
  FASERSourceLink(FASERSourceLink&&) = default;
  FASERSourceLink& operator=(const FASERSourceLink&) = default;
  FASERSourceLink& operator=(FASERSourceLink&&) = default;

  /// Access the geometry identifier.
  constexpr Acts::GeometryIdentifier geometryId() const { return m_geometryId; }
  /// Access the index.
  constexpr Index index() const { return m_index; }
  /// Access the Tracker::FaserSCT_Cluster hit
  constexpr Tracker::FaserSCT_Cluster hit() const { return m_hit; }

private:
  Acts::GeometryIdentifier m_geometryId;
  Index m_index;
  Tracker::FaserSCT_Cluster m_hit;

  friend constexpr bool operator==(const FASERSourceLink& lhs,
                                   const FASERSourceLink& rhs) {
    return (lhs.m_geometryId == rhs.m_geometryId) and
           (lhs.m_index == rhs.m_index);
  }
  friend constexpr bool operator!=(const FASERSourceLink& lhs,
                                   const FASERSourceLink& rhs) {
    return not(lhs == rhs);
  }
};

/// Container of index source links.
///
/// Since the source links provide a `.geometryId()` accessor, they can be
/// stored in an ordered geometry container.
using FASERSourceLinkContainer = GeometryIdMultiset<FASERSourceLink>;
/// Accessor for the above source link container
///
/// It wraps up a few lookup methods to be used in the Combinatorial Kalman
/// Filter
using FASERSourceLinkAccessor = GeometryIdMultisetAccessor<FASERSourceLink>;
