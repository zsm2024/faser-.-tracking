/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Ecal Sensitive Detector Tool.
//

// class header
#include "EcalSensorSDTool.h"

// package includes
#include "EcalSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EcalSensorSDTool::EcalSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
  , m_birk_c1 ( 0.013  * CLHEP::g/CLHEP::MeV/CLHEP::cm2 ) // 1st coef. of Birk's   
  , m_birk_c2 ( 9.6E-6 * CLHEP::g*CLHEP::g/CLHEP::MeV/CLHEP::MeV/CLHEP::cm2/CLHEP::cm2 ) // 2nd coef. of Birk's law 
  , m_birk_c1correction  ( 0.57142857)  //correction of c1 for 2e charged part.  
  , m_a_local_outer_ecal ( 0.05 )  // sinusoidal non uniformity amplitude
  , m_a_global_outer_ecal  ( 0.03  ) // global non uniformity amplitude
  , m_a_reflection_height ( 0.09 ) // reflection on the edges - height
  , m_a_reflection_width  ( 6. * CLHEP::mm ) // reflection on the edges - width
{

  declareProperty ( "BirkC1"               ,  m_birk_c1               ) ;   
  declareProperty ( "BirkC1cor"            ,  m_birk_c1correction     ) ;   
  declareProperty ( "BirkC2"               ,  m_birk_c2               ) ;
  declareProperty ( "LocalNonUnifomity"    ,  m_a_local_outer_ecal  ) ;
  declareProperty ( "GlobalNonUnifomity"   ,  m_a_global_outer_ecal ) ;
  declareProperty ( "ReflectionHeight"     ,  m_a_reflection_height   ) ;  
  declareProperty ( "ReflectionWidth"      ,  m_a_reflection_width    ) ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* EcalSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating Ecal SD: " << name() );

  EcalSensorSD* ecsd = new EcalSensorSD(name(), m_outputCollectionNames[0]);

  ecsd->birk_c1(m_birk_c1);
  ecsd->birk_c1cor(m_birk_c1correction);
  ecsd->birk_c2(m_birk_c2);
  ecsd->local_non_uniformity(m_a_local_outer_ecal);
  ecsd->global_non_uniformity(m_a_global_outer_ecal);
  ecsd->reflection_height(m_a_reflection_height);
  ecsd->reflection_width(m_a_reflection_width);

  return ecsd;
}

