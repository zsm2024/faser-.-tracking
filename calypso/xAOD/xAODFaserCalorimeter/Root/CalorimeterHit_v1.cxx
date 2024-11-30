/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODFaserCalorimeter/versions/CalorimeterHit_v1.h"

namespace xAOD {

  CalorimeterHit_v1::CalorimeterHit_v1() : SG::AuxElement() {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CalorimeterHit_v1, unsigned int, channel, set_channel )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CalorimeterHit_v1, float, Nmip, set_Nmip )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CalorimeterHit_v1, float, E_dep, set_E_dep )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CalorimeterHit_v1, float, E_EM, set_E_EM )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CalorimeterHit_v1, float, fit_to_raw_ratio, set_fit_to_raw_ratio )

  // setters and getters for the Calo WaveformHit links

  AUXSTORE_OBJECT_SETTER_AND_GETTER( CalorimeterHit_v1,
				     CalorimeterHit_v1::WaveformHitLinks_t,
				     WaveformLinks,
				     setWaveformLinks )

  static const SG::AuxElement::Accessor< CalorimeterHit_v1::WaveformHitLinks_t > HitAcc( "WaveformLinks" );

  const WaveformHit* CalorimeterHit_v1::Hit( size_t i ) const {
    return ( *HitAcc( *this )[ i ] );
  }

  size_t CalorimeterHit_v1::nHits() const {
    return HitAcc( *this ).size();
  }

  void CalorimeterHit_v1::addHit( const xAOD::WaveformHitContainer* pWaveformHitContainer, 
				      const xAOD::WaveformHit* pWaveformHit) {
    ElementLink< xAOD::WaveformHitContainer > linkToWaveformHit;
    linkToWaveformHit.toContainedElement(*pWaveformHitContainer, pWaveformHit);

    HitAcc( *this ).push_back( linkToWaveformHit );

    return;
  }

  void CalorimeterHit_v1::clearWaveformLinks() {
    HitAcc( *this ).clear();
    return;
  }

} // namespace xAOD

namespace xAOD {

  std::ostream& operator<<(std::ostream& s, const xAOD::CalorimeterHit_v1& hit) {
    s << "xAODCalorimeterHit:"
      << " channel = " << hit.channel()
      << ", E_dep = " << hit.E_dep()
      << std::endl;

    return s;
  }

} // namespace xAOD
