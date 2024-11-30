/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */


#include "FaserCacheCreator.h"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include "Identifier/IdentifierHash.h"
#include "AthViews/View.h"

namespace Tracker
{

  FaserCacheCreator::FaserCacheCreator(const std::string &name, ISvcLocator *pSvcLocator) :
    AthReentrantAlgorithm(name,pSvcLocator),
    m_sct_idHelper(nullptr),
    m_disableWarningCheck(false)
  {
  }


  StatusCode FaserCacheCreator::initialize(){
    ATH_CHECK( m_SCTclusterContainerCacheKey.initialize(!m_SCTclusterContainerCacheKey.key().empty()) );
    //ATH_CHECK( m_SCTSpacePointCacheKey.initialize(!m_SCTSpacePointCacheKey.key().empty()) );
    ATH_CHECK( m_SCTRDOCacheKey.initialize(!m_SCTRDOCacheKey.key().empty()) );
    ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "FaserSCT_ID"));
    return StatusCode::SUCCESS;
  }

  FaserCacheCreator::~FaserCacheCreator() {}

  bool FaserCacheCreator::isInsideView(const EventContext& context) const
  {
    const IProxyDict* proxy = Atlas::getExtendedEventContext(context).proxy();
    const SG::View* view = dynamic_cast<const SG::View*>(proxy);
    return view != nullptr;
  }

  StatusCode FaserCacheCreator::execute (const EventContext& ctx) const
  {

    if (!m_disableWarningCheck and !m_disableWarning.value()){
      if (isInsideView(ctx)){
        ATH_MSG_ERROR("FaserCacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
      }
      m_disableWarningCheck = true; //only check once
    }

    ATH_CHECK(createContainer(m_SCTclusterContainerCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

    //ATH_CHECK(createContainer(m_SCTSpacePointCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));

    ATH_CHECK(createContainer(m_SCTRDOCacheKey, m_sct_idHelper->wafer_hash_max(), ctx));


    return StatusCode::SUCCESS;
  }

}