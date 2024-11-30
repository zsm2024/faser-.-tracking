/*
  Copyright (C) 2022 CERN for the benefit of the FASER collaboration
*/

/**
 * @file CaloRecTool.cxx
 * Implementation file for the CaloRecTool.cxx
 * @ author Deion Fellers, 2022
 **/

#include "CaloRecTool.h"

// Constructor
CaloRecTool::CaloRecTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//--------------------------------------------------------------
StatusCode
CaloRecTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );

  // Set keys to read calibratiion variables from data base
  ATH_CHECK( m_PMT_HV_ReadKey.initialize() );
  ATH_CHECK( m_MIP_ref_ReadKey.initialize() );

  // access and store calo PMT HV gain curves
  HVgaincurves_rootFile = TFile::Open(m_PMT_HV_Gain_Curve_file.value().c_str(),"read");   // open root file that has TF1 gain curves stored

  chan0_HVgaincurve_pntr = HVgaincurves_rootFile->Get<TF1>(m_chan0_HVgaincurve_name.value().c_str()); // make pointers to the gain curves
  chan1_HVgaincurve_pntr = HVgaincurves_rootFile->Get<TF1>(m_chan1_HVgaincurve_name.value().c_str());
  chan2_HVgaincurve_pntr = HVgaincurves_rootFile->Get<TF1>(m_chan2_HVgaincurve_name.value().c_str());
  chan3_HVgaincurve_pntr = HVgaincurves_rootFile->Get<TF1>(m_chan3_HVgaincurve_name.value().c_str());

  m_HVgainCurveMap[0] = *chan0_HVgaincurve_pntr;  // store TF1 objects in an array mapped to the calo channel numbers
  m_HVgainCurveMap[1] = *chan1_HVgaincurve_pntr;
  m_HVgainCurveMap[2] = *chan2_HVgaincurve_pntr;
  m_HVgainCurveMap[3] = *chan3_HVgaincurve_pntr;

  HVgaincurves_rootFile->Close(); // close the root file

  // These should be in DB, but just hardcode this for now
  m_MIP_sim_Edep[0] = m_MIP_sim_Edep_calo.value();
  m_MIP_sim_Edep[1] = m_MIP_sim_Edep_calo.value();
  m_MIP_sim_Edep[2] = m_MIP_sim_Edep_calo.value();
  m_MIP_sim_Edep[3] = m_MIP_sim_Edep_calo.value();

  m_MIP_sim_Edep[12] = m_MIP_sim_Edep_preshower.value();
  m_MIP_sim_Edep[13] = m_MIP_sim_Edep_preshower.value();
  
  m_MIP_sim_Edep[16] = m_MIP_sim_Edep_calo2.value();
  m_MIP_sim_Edep[17] = m_MIP_sim_Edep_calo2.value();
  m_MIP_sim_Edep[18] = m_MIP_sim_Edep_calo2.value();
  m_MIP_sim_Edep[19] = m_MIP_sim_Edep_calo2.value();

  m_EM_mu_Map[0] = m_calo_EM_mu.value();
  m_EM_mu_Map[1] = m_calo_EM_mu.value();
  m_EM_mu_Map[2] = m_calo_EM_mu.value();
  m_EM_mu_Map[3] = m_calo_EM_mu.value();

  m_EM_mu_Map[16] = m_calo_EM_mu.value();
  m_EM_mu_Map[17] = m_calo_EM_mu.value();
  m_EM_mu_Map[18] = m_calo_EM_mu.value();
  m_EM_mu_Map[19] = m_calo_EM_mu.value();

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------
StatusCode
CaloRecTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------
TF1 CaloRecTool::get_PMT_HV_curve(int channel) const {
  if (channel <= 4) {
    return m_HVgainCurveMap[channel];
  } else {
    ATH_MSG_WARNING("channel "<<channel<<" is not <= 4 and thus not a calorimeter channel, so no HV gain-curve exists!");
  }
  return TF1("default", "1.0", 0.0, 2000.0);
}

//--------------------------------------------------------------
float CaloRecTool::getHV(const EventContext& ctx, int channel) const {

  ATH_MSG_DEBUG("in getHV("<<channel<<")");

  float HV=0.;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_PMT_HV_ReadKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return HV;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return HV;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  // Read offset for specific channel
  const CondAttrListCollection::AttributeList& payload{readCdo->attributeList(channel)};

  if (payload.exists("HV") and not payload["HV"].isNull()) {
    HV = payload["HV"].data<float>();
    ATH_MSG_DEBUG("Found digitizer channel " << channel << ", HV as " << HV);
  } else {
    ATH_MSG_WARNING("No valid HV found for channel "<<channel<<"!");
  }

  return HV;

}

float CaloRecTool::getHV(int channel) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return CaloRecTool::getHV(ctx, channel);
}

//----------------------------------------------------------------------
float CaloRecTool::getHV_ref(const EventContext& ctx, int channel) const {

  ATH_MSG_DEBUG("in getHV_ref("<<channel<<")");

  float HV_ref=0.;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_MIP_ref_ReadKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return HV_ref;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return HV_ref;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  // Read offset for specific channel
  const CondAttrListCollection::AttributeList& payload{readCdo->attributeList(channel)};

  if (payload.exists("HV_ref") and not payload["HV_ref"].isNull()) {
    HV_ref = payload["HV_ref"].data<float>();
    ATH_MSG_DEBUG("Found digitizer channel " << channel << ", HV_ref as " << HV_ref);
  } else {
    ATH_MSG_WARNING("No valid HV_ref found for channel "<<channel<<"!");
  }

  return HV_ref;

}

float CaloRecTool::getHV_ref(int channel) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return CaloRecTool::getHV_ref(ctx, channel);
}

//----------------------------------------------------------------------
float CaloRecTool::getMIPcharge_ref(const EventContext& ctx, int channel) const {

  ATH_MSG_DEBUG("in getMIPcharge_ref("<<channel<<")");

  float MIP_charge_ref =1.;  // Default for no calibration parameters

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_MIP_ref_ReadKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return MIP_charge_ref;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return MIP_charge_ref;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  // Read offset for specific channel
  const CondAttrListCollection::AttributeList& payload{readCdo->attributeList(channel)};

  if (payload.exists("charge_ref") and not payload["charge_ref"].isNull()) {
    MIP_charge_ref = payload["charge_ref"].data<float>();
    ATH_MSG_DEBUG("Found digitizer channel " << channel << ", MIP_charge_ref as " << MIP_charge_ref);
  } else {
    ATH_MSG_WARNING("No valid MIP_charge_ref found for channel "<<channel<<"!");
  }

  return MIP_charge_ref;

}

float CaloRecTool::getMIPcharge_ref(int channel) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return CaloRecTool::getMIPcharge_ref(ctx, channel);
}

//----------------------------------------------------------------------
float CaloRecTool::extrapolateHVgain(const EventContext& ctx, int channel) const {
  float PMT_hv = getHV(ctx, channel);
  float PMT_hv_ref = getHV_ref(ctx, channel);
  TF1 gaincurve = get_PMT_HV_curve(channel);

  float gaincurve_atHV = gaincurve.Eval(PMT_hv);
  float gaincurve_atHVref = gaincurve.Eval(PMT_hv_ref);

  return ( gaincurve_atHVref / gaincurve_atHV ) * pow( PMT_hv_ref / PMT_hv , 6.6);
}

//--------------------------------------------------------------

//----------------------------------------------------------------------
// Reconstruct one waveform hit 
//xAOD::CalorimeterHit*
void
CaloRecTool::reconstruct(const EventContext& ctx,
			 xAOD::CalorimeterHit* calo_hit,
			 bool correct_gain=false) const {

  // Get the waveform hit attached to this calo hit
  const xAOD::WaveformHit* wave_hit = calo_hit->Hit(0);
  ATH_MSG_DEBUG("calo_hit in channel " << wave_hit->channel() );
  
  float MIPcharge_ref = getMIPcharge_ref(ctx, wave_hit->channel()); // get reference MIP charge from database

  float charge = wave_hit->integral()/50.0; // divide by 50 ohms to get charge
  ATH_MSG_DEBUG("calo_hit filled has charge of " << charge << " pC");

  float gainRatio = 1.0;
  if (correct_gain) { // MC already has correct MIP charge stored in MIPcharge_ref, so only need to to HV extrapolation with reral data
    gainRatio = extrapolateHVgain(ctx, wave_hit->channel());
  }
  ATH_MSG_DEBUG("HV gain ratio = " << gainRatio );

  float Nmip = (charge * gainRatio) / MIPcharge_ref;
  ATH_MSG_DEBUG("Nmip = " << Nmip );
  calo_hit->set_Nmip(Nmip); // set Nmip value

  float E_dep = Nmip * m_MIP_sim_Edep[wave_hit->channel()];
  ATH_MSG_DEBUG("E_dep in MeV = " << E_dep );
  calo_hit->set_E_dep(E_dep);  // set calibrated E_dep value

  float E_EM = Nmip * m_EM_mu_Map[wave_hit->channel()];
  ATH_MSG_DEBUG("Em E in MeV = " << E_EM );
  calo_hit->set_E_EM(E_EM);  // set calibrated E_EM value

  float fit_to_raw_ratio = 1.0;
  if (wave_hit->integral() != 0.0) { // avoid possibility of division by zero error
    fit_to_raw_ratio = wave_hit->raw_integral() / wave_hit->integral();
  }
  calo_hit->set_fit_to_raw_ratio(fit_to_raw_ratio); // set fit-to-raw-ratio that can be used to take any of the calibrated values to what they would be if we used the raw integral instead of the fit integral

}




