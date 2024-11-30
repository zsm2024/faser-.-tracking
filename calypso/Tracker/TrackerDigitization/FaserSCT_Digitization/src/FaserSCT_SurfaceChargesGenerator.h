// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Header file for FaserSCT_SurfaceChargesGenerator Class
 * @brief Class to drift the charged hits to the sensor surface for SCT
 * version 1.0a Szymon Gadomski 31.05.2001
 * @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,    
 * tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch, Jorgen.Dalmau@cern.ch, 
 * This is based on the SCTtest3SurfaceChargesGenerator
 * compared to "test3", changes are all related to interfaces
 * this should work with the new SCT_BarrelModuleSideDesign
 * Compared to "test2" the following have been added in "test3":
 * - a possibility to work with long steps and to smear charge along
 *   the step in smaller steps
 * - Lorentz angle
 * 19/04/2001 -  change of hit class to SiTrackerHit
 * version 1.0 Szymon Gadomski 09/06/2001, compiles with framework
 * 26/07/2001 - changed to work with SiDigitization-00-02-00, tested
 * 20/10/2002 - Awatif Belymam
 * 29/10/2002 - the thickness of sensors is obtained from the objects
 *            - of SCT_ModuleSideDesign type instead from its jobOptions file.
 *            - Changes are done as well in jobOptions files.
 * 06/01/2004 - Everything is now in CLHEP units (mm, ns, MeV)
 * 23/08/2007 - Kondo.Gnanvo@cern.ch           
 *            - Conversion of the SCT_SurfaceChargesGenerator code Al gTool
 */

#ifndef FASERSCT_DIGITIZATION_FASERSCT_SURFACECHARGESGENERATOR_H
#define FASERSCT_DIGITIZATION_FASERSCT_SURFACECHARGESGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_Digitization/ISCT_SurfaceChargesGenerator.h"

#include "Identifier/IdentifierHash.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
// #include "FaserSCT_ConditionsTools/ISCT_RadDamageSummaryTool.h"
// #include "SCT_ModuleDistortions/ISCT_ModuleDistortionsTool.h"
#include "FaserSiPropertiesTool/IFaserSiPropertiesTool.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h" // for ITHistSvc
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"

#include <iostream>

// Charges and hits
class FaserSiHit;

// -- do charge trapping histos
class ITHistSvc;
class TH1F;
class TH2F;
class TProfile;

namespace TrackerDD {
  class SiDetectorElement;
  class SCT_ModuleSideDesign;
}

namespace CLHEP {
  class HepRandomEngine;
}

template <class HIT> class TimedHitPtr;

class FaserSCT_SurfaceChargesGenerator : public extends<AthAlgTool, ISCT_SurfaceChargesGenerator> {
 public:

  /**  constructor */
  FaserSCT_SurfaceChargesGenerator(const std::string& type, const std::string& name, const IInterface* parent);

  /** Destructor */
  virtual ~FaserSCT_SurfaceChargesGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

 private:

  virtual void setFixedTime(float fixedTime)                             {m_tfix = fixedTime;}

  /** create a list of surface charges from a hit */
  virtual void process(const TrackerDD::SiDetectorElement* element, const TimedHitPtr<FaserSiHit>& phit, const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine, const EventContext& ctx) const;
  void processSiHit(const TrackerDD::SiDetectorElement* element, const FaserSiHit& phit, const ISiSurfaceChargesInserter& inserter, float eventTime, unsigned short eventID, CLHEP::HepRandomEngine * rndmEngine, const EventContext& ctx) const;
  
  // some diagnostics methods are needed here too
  float driftTime(float zhit, const TrackerDD::SiDetectorElement* element, const EventContext& ctx) const; //!< calculate drift time perpandicular to the surface for a charge at distance zhit from mid gap
  float diffusionSigma(float zhit, const TrackerDD::SiDetectorElement* element, const EventContext& ctx) const; //!< calculate diffusion sigma from a gaussian dist scattered charge
  float surfaceDriftTime(float ysurf) const; //!< Calculate of the surface drift time
  float maxDriftTime(const TrackerDD::SiDetectorElement* element, const EventContext& ctx) const; //!< max drift charge equivalent to the detector thickness
  float maxDiffusionSigma(const TrackerDD::SiDetectorElement* element, const EventContext& ctx) const; //!< max sigma diffusion

  // trap_pos and drift_time are updated based on spess.
//   bool chargeIsTrapped(double spess, const TrackerDD::SiDetectorElement* element, double& trap_pos, double& drift_time) const;

  IntegerProperty m_numberOfCharges{this, "NumberOfCharges", 1, "number of charges"};
  FloatProperty m_smallStepLength{this, "SmallStepLength", 5, "max internal step along the larger G4 step"};

  /** related to the surface drift */
  FloatProperty m_tSurfaceDrift{this, "SurfaceDriftTime", 10, "max surface drift time"};

  FloatProperty m_tfix{this, "FixedTime", -999., "fixed time"};
  FloatProperty m_tsubtract{this, "SubtractTime", -999., "subtract drift time from mid gap"};

  BooleanProperty m_doDistortions{this, "doDistortions", false, "Simulation of module distortions"};
  BooleanProperty m_useSiCondDB{this, "UseSiCondDB", false, "Usage of SiConditions DB values can be disabled to use setable ones"};
  FloatProperty m_vdepl{this, "DepletionVoltage", 70., "depletion voltage, default 70V"};
  FloatProperty m_vbias{this, "BiasVoltage", 150., "bias voltage, default 150V"};
  BooleanProperty m_doTrapping{this, "doTrapping", false, "Flag to set Charge Trapping"};
//   BooleanProperty m_doHistoTrap{this, "doHistoTrap", false, "Histogram the charge trapping effect"};
//   BooleanProperty m_doRamo{this, "doRamo", false, "Ramo Potential for charge trapping effect"};
  BooleanProperty m_isOverlay{this, "isOverlay", false, "flag for overlay"};

  //ToolHandles
//   ToolHandle<ISCT_ModuleDistortionsTool> m_distortionsTool{this, "SCTDistortionsTool", "SCT_DistortionsTool", "Tool to retrieve SCT distortions"};
  ToolHandle<IFaserSiPropertiesTool> m_siPropertiesTool{this, "SiPropertiesTool", "FaserSCT_SiPropertiesTool", "Tool to retrieve SCT silicon properties"};
//   ToolHandle<ISCT_RadDamageSummaryTool> m_radDamageTool{this, "RadDamageSummaryTool", "SCT_RadDamageSummaryTool", "Tool to retrieve SCT radiation damages"};
  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "FaserSCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};
  ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/FaserSCTLorentzAngleTool", "Tool to retreive Lorentz angle"};

  // ServiceHandle<ITHistSvc> m_thistSvc{this, "THistSvc", "THistSvc"};

  float m_tHalfwayDrift{0.}; //!< Surface drift time
  float m_distInterStrip{1.0}; //!< Inter strip distance normalized to 1
  float m_distHalfInterStrip{0.}; //!< Half way distance inter strip

  bool m_SurfaceDriftFlag{false}; //!< surface drift ON/OFF

  // Keep track of TOF 
  // These are updated in processSiHit which is const, so make mutable
  // mutable int m_tofNum{0};
  // mutable float m_tofSum{0.};
  // mutable float m_tofSum2{0.};

  // -- Histograms
  // TProfile* m_h_efieldz{nullptr};
  // TH1F* m_h_efield{nullptr};
  // TH1F* m_h_spess{nullptr};
  // TH1F* m_h_depD{nullptr};
  // TH2F* m_h_drift_electrode{nullptr};
  // TH1F* m_h_ztrap{nullptr};
  // TH1F* m_h_drift_time{nullptr};
  // TH1F* m_h_t_electrode{nullptr};
  // TH1F* m_h_zhit{nullptr};
  // TH1F* m_h_ztrap_tot{nullptr};
  // TH1F* m_h_no_ztrap{nullptr};
  // TH1F* m_h_trap_drift_t{nullptr};
  // TH1F* m_h_notrap_drift_t{nullptr};
  // TProfile* m_h_mob_Char{nullptr};
  // TProfile* m_h_vel{nullptr};
  // TProfile* m_h_drift1{nullptr};
  // TProfile* m_h_gen{nullptr};
  // TProfile* m_h_gen1{nullptr};
  // TProfile* m_h_gen2{nullptr};
  // TProfile* m_h_velocity_trap{nullptr};
  // TProfile* m_h_mobility_trap{nullptr};
  // TH1F* m_h_trap_pos{nullptr};
};

#endif // FASERSCT_SURFACECHARGESGENERATOR_H
