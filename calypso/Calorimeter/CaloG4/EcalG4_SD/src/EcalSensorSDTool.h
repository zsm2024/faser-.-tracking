/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Ecal Sensitive Detector Tool
 ****************************************************************/

#ifndef ECALG4_SD_ECALSENSORSDTOOL_H
#define ECALG4_SD_ECALSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class EcalSensorSDTool : public SensitiveDetectorBase
{

public:
  // Constructor
  EcalSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~EcalSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;

private:
  // the first coefficient of Birk's law                    (c1)   
  double m_birk_c1             ;   
  // the second coefficient of Birk's law                   (c2)   
  double m_birk_c2             ;   
  // the correction to the first coefficient of Birk's law  (c1')   
  double m_birk_c1correction   ;
  
  // Amplitudes of the local non uniformity correction
  double m_a_local_outer_ecal  ;
  // Amplitudes of the global non uniformity correction
  double m_a_global_outer_ecal  ;
  // Correction for light reflection at the edges
  double  m_a_reflection_height ;
  double  m_a_reflection_width  ;
};

#endif //ECALG4_SD_ECALSENSORSDTOOL_H
