/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_VP1TRACKSYSTEMS_MEASUREMENTTOSONODE_H
#define TRK_VP1TRACKSYSTEMS_MEASUREMENTTOSONODE_H
#include <vector>
#include <map>
#include <string>
#include "Identifier/Identifier.h"
#include "VTI12Utils/HitsSoNodeManager.h"
#include "VTI12TrackSystems/TrkObjToString.h"

class SoNode;
class SoCube;
class SoTubs;
class SoSeparator;
class SoTransform;
class SoMaterial;

// namespace InDet {
//     class PixelCluster;
//     class SCT_Cluster;
//     class TRT_DriftCircle;
// }

// namespace InDetDD {
//     class SiDetectorElement;
// }
namespace ScintDD {
    class ScintDetectorElement;
}

namespace Trk {
//    class LocalPosition;
    class TrkDetElementBase;
    class PrepRawData;
    class RIO_OnTrack;
    class CompetingRIOsOnTrack;
    class PseudoMeasurementOnTrack;
    class MeasurementBase;

    class MeasurementToSoNode {
    public:

        enum ClusterDrawMode { SingleStrip=0, MultipleStrips=1, Errors=2 };

        MeasurementToSoNode();

        void initialize( );

        /** This is the most general and most complete method - it returns a separator with nodes and transform. The material is not
        added here as it should be at the top of the tree (in general) */
        std::pair<SoSeparator*,  std::vector<SoNode*> >              \
                                createMeasSep(  const Trk::MeasurementBase& meas);

        /** Returns a vector of SoNodes to represent the RIO_OnTrack. The cluster mode determines how the clusters are drawn -
        i.e. as single central strip or the strips contributing to the cluster*/
        std::vector<SoNode*>    toSoNodes(      const Trk::RIO_OnTrack& prd);

        std::vector<SoNode*>    toSoNodes(      const Trk::CompetingRIOsOnTrack& crot);

        std::vector<SoNode*>    toSoNodes(      const Trk::PseudoMeasurementOnTrack& crot);

        // create basic shapes

        // SoNode*                 createPointMarker();
        SoNode*                 createLineMarker(double halfLength, bool isTube=false );

        /** Generic routine which creates SoNode from passed detector element, plus measurement type*/
        SoNode*                 toSoNode(const Trk::TrkDetElementBase* baseDetEl, Identifier id, TrkObjToString::MeasurementType prdType, Amg::Vector2D locPos);

        //specific types
        // SoNode*                 toSoNode(const InDetDD::SiDetectorElement* detElement, Identifier id, TrkObjToString::MeasurementType detType, Amg::Vector2D& locPos);

        void                    setClusterDrawMode    (ClusterDrawMode mode);
        ClusterDrawMode         clusterDrawMode       ();

        void                    setSimpleView(      TrkObjToString::MeasurementType type, bool value );
        void                    setSimpleViewAll(   bool value );
        bool                    isSimpleView(       TrkObjToString::MeasurementType type );

        /** Uses the local coords & the surface of the passed PRD to create a transform for the strip*/
        // SoTransform*            createTransform(    const Trk::PrepRawData& prd,
        //                                             MeasurementType detType);

        /** Uses the local coords & the surface of the passed Trk::MeasurementBase to create a transform for the strip*/
        SoTransform*            createTransform(    const Trk::MeasurementBase& rot,
                                                    TrkObjToString::MeasurementType detType);

    private:
        std::vector<bool>       m_simpleView;
        double                  m_stripThickness; //!< Depth/Thickness of strips in z (local coords)
        std::vector< SoMaterial* > m_materials;

        ClusterDrawMode         m_clusDrawMode;
        float m_dtLength; //!< Only used when in ShortMode
        
        HitsSoNodeManager       m_nodeManager;
        TrkObjToString          m_objToType;
    };
}

inline void
Trk::MeasurementToSoNode::setClusterDrawMode(Trk::MeasurementToSoNode::ClusterDrawMode mode)
{
    m_clusDrawMode=mode;
}

inline Trk::MeasurementToSoNode::ClusterDrawMode
Trk::MeasurementToSoNode::clusterDrawMode()
{
    return m_clusDrawMode;
}

inline void
Trk::MeasurementToSoNode::setSimpleView(TrkObjToString::MeasurementType type, bool value)
{
    m_simpleView[type]=value;
}

inline void
Trk::MeasurementToSoNode::setSimpleViewAll( bool value)
{
    unsigned int end = static_cast<TrkObjToString::MeasurementType>(TrkObjToString::Unknown);
    for (unsigned int i=0; i<end; ++i) m_simpleView[i]=value;
}

inline bool
Trk::MeasurementToSoNode::isSimpleView(TrkObjToString::MeasurementType type)
{
    return m_simpleView[type];
}


#endif

