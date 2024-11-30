/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKEREVENTCNVTOOL_H
#define TRACKEREVENTCNVTOOL_H

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include <utility>

class FaserDetectorID;
class Identifier;
class IdentifierHash;
class IdDictManager;
class FaserSCT_ID;

namespace Trk {
  class TrkDetElementBase;
  class PrepRawData;
}


namespace Tracker {
  /**Helper tool uses to convert Tracker objects in generic tracking custom convertor TrkEventAthenaPool.

     See "mainpage" for discussion of jobOpts.
  */
class TrackerEventCnvTool :  public extends<AthAlgTool, Trk::ITrkEventCnvTool>
{
  public:
  
    enum TrackerConcreteType { SCT, Unknown };

    TrackerEventCnvTool(const std::string&,const std::string&,const IInterface*);
  
    virtual ~TrackerEventCnvTool() = default;
  
    virtual StatusCode initialize() override;
  
    virtual void checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const override;
  
    /** use the passed identifier to recreate the detector element and PRD links on the passed RIO_OnTrack
        @param[in] rioOnTrack The RIO_OnTrack we're interested in
        @return  std::pair of the pointers to the two corresponding objects*/
    virtual std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>
      getLinks( Trk::RIO_OnTrack& rioOnTrack ) const override;
      
    /** @copydoc Trk::ITrkEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack* rot)*/
    virtual void prepareRIO_OnTrack( Trk::RIO_OnTrack* rot) const override;
  
    /** Similar, but just return the EL components rather then
        changing ROT. */
    virtual void prepareRIO_OnTrackLink( const Trk::RIO_OnTrack* rot,
                                         ELKey_t& key,
                                         ELIndex_t& index ) const override;

    /** @copydoc Trk::ITrkEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack* rot)*/
    virtual void recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const override;
  
    /** Return the detectorElement associated with this Identifier*/
    virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const override;

    /** Return the detectorElement associated with this Identifier*/
    virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id) const override;

  
  private:

    /** use the passed identifier to recreate the SCT cluster link on the passed RIO_OnTrack*/
    virtual const Trk::PrepRawData* sctClusterLink( const Identifier& id,  const IdentifierHash& idHash ) const;
  
    /** use the passed IdentifierHash to get SiDetectorElement for SCT*/
    const TrackerDD::SiDetectorElement* getSCTDetectorElement(const IdentifierHash& waferHash) const;

    bool m_setPrepRawDataLink;                     //!< if true, attempt to recreate link to PRD


    //various id helpers
    const FaserDetectorID* m_IDHelper;
    const FaserSCT_ID* m_SCTHelper;
  
    // added to check TRT existence (SLHC geo check)
    const IdDictManager* m_idDictMgr;

    SG::ReadHandleKey<FaserSCT_ClusterContainer>      m_sctClusContName       {this, "SCT_ClusterContainer",     "SCT_ClusterContainer",             "SCT Cluster container name"};   //!< location of container of sct clusters

    SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  };

}
#endif // MOORETOTRACKTOOL_H
