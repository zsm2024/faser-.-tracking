/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserTruthStrategy.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "FaserTruthStrategy.h"

// ISF includes
#include "FaserISF_Event/IFaserTruthIncident.h"
#include "FaserISF_Event/FaserISFParticle.h"

/** Constructor **/
ISF::FaserTruthStrategy::FaserTruthStrategy(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p),
  m_parentEkin(-1.),
  m_childEkin(-1.),
  m_allowChildrenOrParentPass(false),
  m_vertexTypesVector(0),
  m_vertexTypes(),
  m_doVertexRangeCheck(false),
  m_vertexTypeRangeLow(0),
  m_vertexTypeRangeHigh(0),
  m_vertexTypeRangeLength(0),
  m_parentPdgCodesVector(0),
  m_parentPdgCodes()
{
    // provide either a Ekin cut for the parent and child particles respectively.
    // if none are given, it will not use Ekin cuts
    declareProperty("ParentMinEkin"             , m_parentEkin           );
    declareProperty("ChildMinEkin"              , m_childEkin        );
    // if set to true, kinetic cuts are passed even if only child particles pass them
    // (used for special cases such as de-excitation)
    declareProperty("AllowChildrenOrParentPassKineticCuts" , m_allowChildrenOrParentPass );
    declareProperty("VertexTypes"                , m_vertexTypesVector   );
    declareProperty("VertexTypeRangeLow"         , m_vertexTypeRangeLow  );
    declareProperty("VertexTypeRangeHigh"        , m_vertexTypeRangeHigh );
    declareProperty("ParentPDGCodes"            , m_parentPdgCodesVector  );
    declareProperty("Regions"                   , m_regionListProperty );
}

/** Destructor **/
ISF::FaserTruthStrategy::~FaserTruthStrategy()
{
}

// Athena algtool's Hooks
StatusCode  ISF::FaserTruthStrategy::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");

    // (*) setup parent particle cuts
    // -----
    //     (compute and store the squared cut parameters (faster comparisons))
    // check whether the user input makes sense (error case)
    if ( m_parentEkin<0. ) m_parentEkin  = 0.; 

    // (*) setup child particle cuts
    // check whether the user input makes sense (error case)
    if ( m_childEkin<0. ) m_childEkin  = 0.;

    // VertexTypeRanges:
    //
    // check whether user input makes sense:
    if ( m_vertexTypeRangeHigh < m_vertexTypeRangeLow) {
      ATH_MSG_ERROR("The given parameter VertexTypeRangeLow is bigger than VertexTypeRangeHigh. ABORT");
      return StatusCode::FAILURE;
    }
    // the length of the given vertex type range
    m_vertexTypeRangeLength = unsigned(m_vertexTypeRangeHigh - m_vertexTypeRangeLow) + 1;
    // if neither lower now upper range limit given, disable range check
    m_doVertexRangeCheck    = m_vertexTypeRangeLow && m_vertexTypeRangeHigh;


    // fill PDG code std::set used for optimized search
    m_parentPdgCodes.insert( m_parentPdgCodesVector.begin(), m_parentPdgCodesVector.end());

    // fill vertex type std::set used for optimized search
    m_vertexTypes.insert( m_vertexTypesVector.begin(), m_vertexTypesVector.end());

    for(auto region : m_regionListProperty.value()) {
      if(region < FaserDetDescr::fFirstFaserRegion || region >= FaserDetDescr::fNumFaserRegions) {
        ATH_MSG_ERROR("Unknown Region (" << region << ") specified. Please check your configuration.");
        return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
}

StatusCode  ISF::FaserTruthStrategy::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

bool ISF::FaserTruthStrategy::pass( IFaserTruthIncident& ti) const {

  // (1.) energy check
  // ----
  //
  {
    // check whether parent particle passes cut or not
    bool primFail = (ti.parentEkin() < m_parentEkin) ;

    // if parent particle failed and strategy does not
    // allow for child-only pass -> failed
    if ( ( primFail && (!m_allowChildrenOrParentPass) ) ) {
      return false;
    }

    // check child particles
    bool secPass =  ti.childrenEkinPass(m_childEkin);

    // if child particles do not pass cuts
    if (!secPass) {
      if (!m_allowChildrenOrParentPass) {
        // child particles were required to pass cuts but did not
        return false;
      } else if (primFail) {
        // neither parent nor child particles passed cuts
        return false;
      }
    }
  }


  // (2.) parent particle PDG code check
  // ----
  // check whether parent PDG code matches with any of the given ones
  if (  m_parentPdgCodes.size() &&
       (m_parentPdgCodes.find(ti.parentPdgCode()) == m_parentPdgCodes.end()) ) {

    // parent particle PDG code not found
    return false;
  }


  // (3.) vertex type check
  // ----
  if ( m_doVertexRangeCheck || m_vertexTypes.size()) {
    int vxtype = ti.physicsProcessCode();

    // (3.1) vxtype in given range?: this is a small performance trick (only one comparison operator to check if within range)
    //  -> exactly equivalent to:  m_doVertexRangeCheck  && (m_vertexTypeLow<=vxtype) && (vxtype<=m_vertexTypeRangeHigh)
    bool vtxTypeRangePassed = m_doVertexRangeCheck && ( unsigned(vxtype-m_vertexTypeRangeLow) < m_vertexTypeRangeLength );
    // (3.2) if not in range or range check disabled, check whether vxtype
    //       std::set contains the given vertex type
    if ( (!vtxTypeRangePassed) && (m_vertexTypes.find(vxtype)==m_vertexTypes.end()) ) {
        // vxtype not registered -> not passed
        return false;
    }
  }

  // all cuts passed
  return true;
}

bool ISF::FaserTruthStrategy::appliesToRegion(unsigned short geoID) const
{
  return std::find( m_regionListProperty.begin(),
                    m_regionListProperty.end(),
                    geoID ) != m_regionListProperty.end();
}
