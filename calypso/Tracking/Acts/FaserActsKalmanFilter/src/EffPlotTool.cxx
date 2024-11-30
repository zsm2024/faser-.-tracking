#include "EffPlotTool.h"
#include "Acts/Utilities/Helpers.hpp"

void EffPlotTool::book(EffPlotTool::EffPlotCache &effPlotCache) const {
  PlotHelpers::Binning bPhi = m_varBinning.at("Phi");
  PlotHelpers::Binning bEta = m_varBinning.at("Eta");
  PlotHelpers::Binning bPt = m_varBinning.at("Pt");
  // efficiency vs pT
  effPlotCache.trackEff_vs_pT = PlotHelpers::bookEff(
      "trackeff_vs_pT", "Tracking efficiency;Truth pT [GeV/c];Efficiency", bPt);
  // efficiency vs eta
  effPlotCache.trackEff_vs_eta = PlotHelpers::bookEff(
      "trackeff_vs_eta", "Tracking efficiency;Truth #eta;Efficiency", bEta);
  // efficiency vs phi
  effPlotCache.trackEff_vs_phi = PlotHelpers::bookEff(
      "trackeff_vs_phi", "Tracking efficiency;Truth #phi;Efficiency", bPhi);
}

void EffPlotTool::fill(EffPlotTool::EffPlotCache &effPlotCache,
                       const HepMC3::GenParticle* truthParticle, bool status) const {
  const auto t_phi = truthParticle->momentum().phi();
  const auto t_eta = truthParticle->momentum().eta();
  const auto t_pT = truthParticle->momentum().perp() * m_MeV2GeV;

  PlotHelpers::fillEff(effPlotCache.trackEff_vs_pT, t_pT, status);
  PlotHelpers::fillEff(effPlotCache.trackEff_vs_eta, t_eta, status);
  PlotHelpers::fillEff(effPlotCache.trackEff_vs_phi, t_phi, status);
}

void EffPlotTool::write(const EffPlotTool::EffPlotCache &effPlotCache) const {
  effPlotCache.trackEff_vs_pT->Write();
  effPlotCache.trackEff_vs_eta->Write();
  effPlotCache.trackEff_vs_phi->Write();
}

void EffPlotTool::clear(EffPlotTool::EffPlotCache &effPlotCache) const {
  delete effPlotCache.trackEff_vs_pT;
  delete effPlotCache.trackEff_vs_eta;
  delete effPlotCache.trackEff_vs_phi;
}
