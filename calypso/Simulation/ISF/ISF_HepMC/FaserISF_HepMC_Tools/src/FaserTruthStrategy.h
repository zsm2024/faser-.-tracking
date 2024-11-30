/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_TOOLS_FASERTRUTHSTRATEGY_H
#define FASERISF_TOOLS_FASERTRUTHSTRATEGY_H 1

// stl includes
#include <set>
#include <vector>

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserDetDescr/FaserRegion.h"

// ISF includes
#include "FaserISF_HepMC_Interfaces/IFaserTruthStrategy.h"

namespace ISF {
  typedef std::vector<int>              VertexTypesVector;
  typedef std::set<int>                 VertexTypesSet;
  typedef std::vector<int>              PDGCodesVector;
  typedef std::set<int>                 PDGCodesSet;

  /** @class FaserTruthStrategy

      A multi-purpose implementation of an ISF TruthStrategy.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class FaserTruthStrategy final : public extends<AthAlgTool, IFaserTruthStrategy> {
      
    public: 
     /** Constructor with parameters */
     FaserTruthStrategy( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~FaserTruthStrategy();

     // Athena algtool's Hooks
     virtual StatusCode  initialize() override;
     virtual StatusCode  finalize() override;

     /** true if the ITruthStrategy implementation applies to the given IFaserTruthIncident */
     virtual bool pass( IFaserTruthIncident& incident) const override;

     virtual bool appliesToRegion(unsigned short geoID) const override;
	  private:
     /** parent kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
    //  bool                                   m_useParentPt;         //!< use pT or Ekin cuts?
    //  double                                 m_parentPt2;           //!< parent particle
     double                                 m_parentEkin;          //!< parent particle

     /** child particle kinetic energy / transverse momentum cuts
         (pT is stored as pT^2 which allows for faster comparisons) */
    //  bool                                   m_useChildPt;          //!< use pT or Ekin cuts?
    //  double                                 m_childPt2;            //!< pT momentum cut
     double                                 m_childEkin;           //!< Ekin cut
     bool                                   m_allowChildrenOrParentPass; //!< pass cuts if parent did not

     /** vertex type (physics code) checks */
     VertexTypesVector                      m_vertexTypesVector;  //!< Python property
     VertexTypesSet                         m_vertexTypes;        //!< optimized for search
     bool                                   m_doVertexRangeCheck;
     int                                    m_vertexTypeRangeLow;
     int                                    m_vertexTypeRangeHigh;
     unsigned                               m_vertexTypeRangeLength;

     /** PDG code checks */
     PDGCodesVector                         m_parentPdgCodesVector;  //!< Python property
     PDGCodesSet                            m_parentPdgCodes;        //!< optimized for search

    IntegerArrayProperty            m_regionListProperty;
   }; 
  
}


#endif //> !FASERISF_TOOLS_FASERTRUTHSTRATEGY_H
