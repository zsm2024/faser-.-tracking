/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */


/**
 * @file FaserCacheCreator.h
 */

#ifndef TRACKERPREPRAWDATAFORMATION_FASERCACHECREATOR
#define TRACKERPREPRAWDATAFORMATION_FASERCACHECREATOR

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
// #include "FaserTrkSpacePoint/FaserSpacePointContainer.h"
#include "TrkSpacePoint/SpacePointContainer.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <atomic>

class FaserSCT_ID;

namespace Tracker 
{
    class FaserCacheCreator : public AthReentrantAlgorithm
    {
      public:

        FaserCacheCreator(const std::string &name,ISvcLocator *pSvcLocator);
        virtual ~FaserCacheCreator()  ;
        virtual StatusCode initialize () override;
        virtual StatusCode execute (const EventContext& ctx) const override;
        //No need for finalize
      protected:
        const FaserSCT_ID*  m_sct_idHelper;
        SG::WriteHandleKey<Tracker::FaserSCT_ClusterContainerCache>            m_SCTclusterContainerCacheKey
        {this, "SCT_ClusterKey", ""};
        SG::WriteHandleKey<SpacePointCache>                                    m_SCTSpacePointCacheKey
        {this, "SpacePointCacheSCT", ""};
        SG::WriteHandleKey<FaserSCT_RDO_Cache>                                 m_SCTRDOCacheKey
        {this, "SCTRDOCacheKey", ""};

        BooleanProperty m_disableWarning{this, "DisableViewWarning", false};
        mutable std::atomic_bool m_disableWarningCheck;
        //Temporary workarounds for problem in scheduler - remove later
        bool isInsideView(const EventContext&) const;
        template<typename T>
          StatusCode createContainer(const SG::WriteHandleKey<T>& , long unsigned int , const EventContext& ) const;

        // template<typename T, typename X>
        //   StatusCode createValueContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx, const X& defaultValue) const;
    };

    template<typename T>
    StatusCode FaserCacheCreator::createContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx) const{
      if(containerKey.key().empty()){
        ATH_MSG_DEBUG( "Creation of container "<< containerKey.key() << " is disabled (no name specified)");
        return StatusCode::SUCCESS;
      }
      SG::WriteHandle<T> ContainerCacheKey(containerKey, ctx);
      ATH_CHECK( ContainerCacheKey.recordNonConst ( std::make_unique<T>(IdentifierHash(size), nullptr) ));
      ATH_MSG_DEBUG( "Container "<< containerKey.key() << " created to hold " << size );
      return StatusCode::SUCCESS;
    }

    // template<typename T, typename X>
    // StatusCode FaserCacheCreator::createValueContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx, const X& defaultValue) const{
    //     static_assert(std::is_base_of<IdentifiableValueCache<X>, T>::value, "Expects a IdentifiableValueCache Class" );
    //     if(containerKey.key().empty()){
    //         ATH_MSG_DEBUG( "Creation of container "<< containerKey.key() << " is disabled (no name specified)");
    //         return StatusCode::SUCCESS;
    //     }
    //     SG::WriteHandle<T> ContainerCacheKey(containerKey, ctx);
    //     ATH_CHECK( ContainerCacheKey.recordNonConst ( std::make_unique<T>(size, defaultValue) ));
    //     ATH_MSG_DEBUG( "ValueContainer "<< containerKey.key() << " created to hold " << size );
    //     return StatusCode::SUCCESS;
    // }

}
#endif //TRACKERPREPRAWDATAFORMATION_FASERCACHECREATOR
