#include "SummaryPlotTool.h"
#include <iostream>

void SummaryPlotTool::book(SummaryPlotTool::SummaryPlotCache &trackSummaryPlotCache) const {
  PlotHelpers::Binning bEta = m_varBinning.at("Eta");
  PlotHelpers::Binning bPt = m_varBinning.at("Pt");
  PlotHelpers::Binning bNum = m_varBinning.at("Num");
  // number of track states versus eta
  trackSummaryPlotCache.nStates_vs_eta = PlotHelpers::bookProf(
      "nStates_vs_eta", "Number of total states vs. #eta", bEta, bNum);
  // number of measurements versus eta
  trackSummaryPlotCache.nMeasurements_vs_eta = PlotHelpers::bookProf(
      "nMeasurements_vs_eta", "Number of measurements vs. #eta", bEta, bNum);
  // number of holes versus eta
  trackSummaryPlotCache.nHoles_vs_eta = PlotHelpers::bookProf(
      "nHoles_vs_eta", "Number of holes vs. #eta", bEta, bNum);
  // number of outliers versus eta
  trackSummaryPlotCache.nOutliers_vs_eta = PlotHelpers::bookProf(
      "nOutliers_vs_eta", "Number of outliers vs. #eta", bEta, bNum);
  // number of Shared Hits versus eta
  trackSummaryPlotCache.nSharedHits_vs_eta = PlotHelpers::bookProf(
      "nSharedHits_vs_eta", "Number of Shared Hits vs. #eta", bEta, bNum);
  // number of track states versus pt
  trackSummaryPlotCache.nStates_vs_pt = PlotHelpers::bookProf(
      "nStates_vs_pT", "Number of total states vs. pT", bPt, bNum);
  // number of measurements versus pt
  trackSummaryPlotCache.nMeasurements_vs_pt = PlotHelpers::bookProf(
      "nMeasurements_vs_pT", "Number of measurements vs. pT", bPt, bNum);
  // number of holes versus pt
  trackSummaryPlotCache.nHoles_vs_pt = PlotHelpers::bookProf(
      "nHoles_vs_pT", "Number of holes vs. pT", bPt, bNum);
  // number of outliers versus pt
  trackSummaryPlotCache.nOutliers_vs_pt = PlotHelpers::bookProf(
      "nOutliers_vs_pT", "Number of outliers vs. pT", bPt, bNum);
  // number of Shared Hits versus pt
  trackSummaryPlotCache.nSharedHits_vs_pt = PlotHelpers::bookProf(
      "nSharedHits_vs_pT", "Number of Shared Hits vs. pT", bPt, bNum);
}

void SummaryPlotTool::fill(SummaryPlotTool::SummaryPlotCache &trackSummaryPlotCache,
                           const Acts::BoundTrackParameters &fittedParameters, size_t nStates, size_t nMeasurements,
                           size_t nOutliers, size_t nHoles, size_t nSharedHits) const {
  using Acts::VectorHelpers::eta;
  using Acts::VectorHelpers::perp;
  const auto& momentum = fittedParameters.momentum();
  const double fit_eta = eta(momentum);
  const double fit_pT = perp(momentum);

  PlotHelpers::fillProf(trackSummaryPlotCache.nStates_vs_eta, fit_eta, nStates);
  PlotHelpers::fillProf(trackSummaryPlotCache.nMeasurements_vs_eta, fit_eta, nMeasurements);
  PlotHelpers::fillProf(trackSummaryPlotCache.nOutliers_vs_eta, fit_eta, nOutliers);
  PlotHelpers::fillProf(trackSummaryPlotCache.nHoles_vs_eta, fit_eta, nHoles);
  PlotHelpers::fillProf(trackSummaryPlotCache.nSharedHits_vs_eta, fit_eta, nSharedHits);

  PlotHelpers::fillProf(trackSummaryPlotCache.nStates_vs_pt, fit_pT, nStates);
  PlotHelpers::fillProf(trackSummaryPlotCache.nMeasurements_vs_pt, fit_pT, nMeasurements);
  PlotHelpers::fillProf(trackSummaryPlotCache.nOutliers_vs_pt, fit_pT, nOutliers);
  PlotHelpers::fillProf(trackSummaryPlotCache.nHoles_vs_pt, fit_pT, nHoles);
  PlotHelpers::fillProf(trackSummaryPlotCache.nSharedHits_vs_pt, fit_pT, nSharedHits);
}

void SummaryPlotTool::write(const SummaryPlotTool::SummaryPlotCache &trackSummaryPlotCache) const {
  trackSummaryPlotCache.nStates_vs_eta->Write();
  trackSummaryPlotCache.nMeasurements_vs_eta->Write();
  trackSummaryPlotCache.nOutliers_vs_eta->Write();
  trackSummaryPlotCache.nHoles_vs_eta->Write();
  trackSummaryPlotCache.nSharedHits_vs_eta->Write();
  trackSummaryPlotCache.nStates_vs_pt->Write();
  trackSummaryPlotCache.nMeasurements_vs_pt->Write();
  trackSummaryPlotCache.nOutliers_vs_pt->Write();
  trackSummaryPlotCache.nHoles_vs_pt->Write();
  trackSummaryPlotCache.nSharedHits_vs_pt->Write();
}

void SummaryPlotTool::clear(SummaryPlotTool::SummaryPlotCache &trackSummaryPlotCache) const {
  delete trackSummaryPlotCache.nStates_vs_eta;
  delete trackSummaryPlotCache.nMeasurements_vs_eta;
  delete trackSummaryPlotCache.nOutliers_vs_eta;
  delete trackSummaryPlotCache.nHoles_vs_eta;
  delete trackSummaryPlotCache.nSharedHits_vs_eta;
  delete trackSummaryPlotCache.nStates_vs_pt;
  delete trackSummaryPlotCache.nMeasurements_vs_pt;
  delete trackSummaryPlotCache.nOutliers_vs_pt;
  delete trackSummaryPlotCache.nHoles_vs_pt;
  delete trackSummaryPlotCache.nSharedHits_vs_pt;
}
