/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file CaloRecTool.h
 *  Header file for CaloRecTool.h
 *
 */
#ifndef CALORECTOOLS_CALORECTOOL_H
#define CALORECTOOLS_CALORECTOOL_H

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloRecTools/ICaloRecTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"

// Data Classes
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserCalorimeter/CalorimeterHit.h"

// Include ROOT classes
#include "TF1.h"
#include "TFile.h"

#include <string>

class CaloRecTool: public extends<AthAlgTool, ICaloRecTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
  CaloRecTool(const std::string& type, const std::string& name, const IInterface* parent);

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // methods for returning calibration data
  // Channels indexed by digitizer channel number
  // HV is in Volts and MIPcharge is in pC
  //
  virtual float getHV(const EventContext& ctx, int channel) const override;
  virtual float getHV(int channel) const override;

  virtual float getHV_ref(const EventContext& ctx, int channel) const override;
  virtual float getHV_ref(int channel) const override;

  virtual float getMIPcharge_ref(const EventContext& ctx, int channel) const override;
  virtual float getMIPcharge_ref(int channel) const override;

  // method for returning PMT HV calibration curves from root file
  virtual TF1 get_PMT_HV_curve(int channel) const override;

  // Reconstruct one waveform hit and put the resulting Calorimeter hit in the container
  virtual void reconstruct(const EventContext& ctx,
			   xAOD::CalorimeterHit* hit,
			   bool correct_gain) const override;

  TFile* HVgaincurves_rootFile;

  TF1* chan0_HVgaincurve_pntr;
  TF1* chan1_HVgaincurve_pntr;
  TF1* chan2_HVgaincurve_pntr;
  TF1* chan3_HVgaincurve_pntr;

  TF1 m_HVgainCurveMap[4];

  private:
  // Propert that points to the location of the root file which contains the HV gain curves for the calorimeter PMTs
  StringProperty m_PMT_HV_Gain_Curve_file{this, "PMT_HV_Gain_Curve_file", "/cvmfs/faser.cern.ch/repo/sw/database/DBRelease/current/pmtgain/CaloGainCurves.root"};

  // properties that map channel name to PMT HV gain curves. PMt names found at https://twiki.cern.ch/twiki/bin/viewauth/FASER/CaloScintillator
  StringProperty m_chan0_HVgaincurve_name{this, "Chan0HVgaincurve", "pol5_HV_Gain_Curve_PMT_LB8770"};
  StringProperty m_chan1_HVgaincurve_name{this, "Chan1HVgaincurve", "pol5_HV_Gain_Curve_PMT_LB8733"};
  StringProperty m_chan2_HVgaincurve_name{this, "Chan2HVgaincurve", "pol5_HV_Gain_Curve_PMT_LB8786"};
  StringProperty m_chan3_HVgaincurve_name{this, "Chan3HVgaincurve", "pol5_HV_Gain_Curve_PMT_LB8732"};

  // Read Cond Handle
  SG::ReadCondHandleKey<CondAttrListCollection> m_PMT_HV_ReadKey{this, "PMT_HV_ReadKey", "/WAVE/Calibration/HV", "Key of folder for PMT HV reading"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_MIP_ref_ReadKey{this, "MIP_ref_ReadKey", "/WAVE/Calibration/MIP_ref", "Key of folder for MIP charge calibration measurment, also stores PMT HV used to measure the reference MIP charge"};

  // Could also put these in DB, but just hardcode them for now
  FloatProperty m_MIP_sim_Edep_calo {this, "MIP_sim_Edep_calo", 58.5}; // MIP deposits 5.85 MeV of energy in calo
  FloatProperty m_MIP_sim_Edep_calo2 {this, "MIP_sim_Edep_calo", 58.5}; // MIP deposits 5.85 MeV of energy in calo
  FloatProperty m_MIP_sim_Edep_preshower {this, "MIP_sim_Edep_preshower", 4.894}; // MIP deposits 4.894 MeV of energy in a preshower layer

  FloatProperty m_calo_EM_mu {this, "m_calo_EM_mu", 330.0}; // factor used to do rough calibration of calo to EM energy: 0.33 GeV or 330 MeV

  float m_MIP_sim_Edep[32]; // vector that holds Edep factors for calo and preshower
  float m_EM_mu_Map[32]; // vector that holds EM_mu calibration factors for calo channels

  float extrapolateHVgain(const EventContext& ctx, int channel) const;

};

#endif // CALORECTOOLS_CALORECTOOL_H
