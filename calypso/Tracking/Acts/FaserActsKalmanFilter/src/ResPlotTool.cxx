#include "ResPlotTool.h"
#include "Acts/Utilities/Helpers.hpp"
#include "HepMC3/GenVertex.h"

void ResPlotTool::book(ResPlotTool::ResPlotCache& resPlotCache) const {
  PlotHelpers::Binning bEta = m_varBinning.at("Eta");
  PlotHelpers::Binning bPt = m_varBinning.at("Pt");
  PlotHelpers::Binning bPull = m_varBinning.at("Pull");
  std::cout << "DEBUG: Initialize the histograms for residual and pull plots" << std::endl;
  for (unsigned int parID = 0; parID < Acts::eBoundSize; parID++) {
    std::string parName = m_paramNames.at(parID);
    std::string parResidual = "Residual_" + parName;
    // Binning for residual is parameter dependent
    PlotHelpers::Binning bResidual = m_varBinning.at(parResidual);


    // residual distributions
    resPlotCache.res[parName] = PlotHelpers::bookHisto(
        Form("res_%s", parName.c_str()),
        Form("Residual of %s", parName.c_str()), bResidual);
    // residual vs eta scatter plots
    resPlotCache.res_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("res_%s_vs_eta", parName.c_str()),
        Form("Residual of %s vs eta", parName.c_str()), bEta, bResidual);
    // residual mean in each eta bin
    resPlotCache.resMean_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("resmean_%s_vs_eta", parName.c_str()),
        Form("Residual mean of %s", parName.c_str()), bEta);
    // residual width in each eta bin
    resPlotCache.resWidth_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("reswidth_%s_vs_eta", parName.c_str()),
        Form("Residual width of %s", parName.c_str()), bEta);
    // residual vs pT scatter plots
    resPlotCache.res_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("res_%s_vs_pT", parName.c_str()),
        Form("Residual of %s vs pT", parName.c_str()), bPt, bResidual);
    // residual mean in each pT bin
    resPlotCache.resMean_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("resmean_%s_vs_pT", parName.c_str()),
        Form("Residual mean of %s", parName.c_str()), bPt);
    // residual width in each pT bin
    resPlotCache.resWidth_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("reswidth_%s_vs_pT", parName.c_str()),
        Form("Residual width of %s", parName.c_str()), bPt);

    // pull distritutions
    resPlotCache.pull[parName] = PlotHelpers::bookHisto(
        Form("pull_%s", parName.c_str()),
        Form("Pull of %s", parName.c_str()), bPull);
    // pull vs eta scatter plots
    resPlotCache.pull_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("pull_%s_vs_eta", parName.c_str()),
        Form("Pull of %s vs eta", parName.c_str()), bEta, bPull);
    // pull mean in each eta bin
    resPlotCache.pullMean_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("pullmean_%s_vs_eta", parName.c_str()),
        Form("Pull mean of %s", parName.c_str()), bEta);
    // pull width in each eta bin
    resPlotCache.pullWidth_vs_eta[parName] = PlotHelpers::bookHisto(
        Form("pullwidth_%s_vs_eta", parName.c_str()),
        Form("Pull width of %s", parName.c_str()), bEta);
    // pull vs pT scatter plots
    resPlotCache.pull_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("pull_%s_vs_pT", parName.c_str()),
        Form("Pull of %s vs pT", parName.c_str()), bPt, bPull);
    // pull mean in each pT bin
    resPlotCache.pullMean_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("pullmean_%s_vs_pT", parName.c_str()),
        Form("Pull mean of %s", parName.c_str()), bPt);
    // pull width in each pT bin
    resPlotCache.pullWidth_vs_pT[parName] = PlotHelpers::bookHisto(
        Form("pullwidth_%s_vs_pT", parName.c_str()),
        Form("Pull width of %s", parName.c_str()), bPt);
    }
}

void ResPlotTool::write(const ResPlotCache &resPlotCache) const {
  for (unsigned int parID = 0; parID < Acts::eBoundSize; parID++) {
    std::string parName = m_paramNames.at(parID);
    if (resPlotCache.res.count(parName)) {
      resPlotCache.res.at(parName)->Write();
      resPlotCache.res_vs_eta.at(parName)->Write();
      resPlotCache.resMean_vs_eta.at(parName)->Write();
      resPlotCache.resWidth_vs_eta.at(parName)->Write();
      resPlotCache.res_vs_pT.at(parName)->Write();
      resPlotCache.resMean_vs_pT.at(parName)->Write();
      resPlotCache.resWidth_vs_pT.at(parName)->Write();
      resPlotCache.pull.at(parName)->Write();
      resPlotCache.pull_vs_eta.at(parName)->Write();
      resPlotCache.pullMean_vs_eta.at(parName)->Write();
      resPlotCache.pullWidth_vs_eta.at(parName)->Write();
      resPlotCache.pull_vs_pT.at(parName)->Write();
      resPlotCache.pullMean_vs_pT.at(parName)->Write();
      resPlotCache.pullWidth_vs_pT.at(parName)->Write();
    }
  }
}

void ResPlotTool::clear(ResPlotCache &resPlotCache) const {
  for (unsigned int parID = 0; parID < Acts::eBoundSize; parID++) {
    std::string parName = m_paramNames.at(parID);
    if (resPlotCache.res.count(parName)) {
      delete resPlotCache.res.at(parName);
      delete resPlotCache.res_vs_eta.at(parName);
      delete resPlotCache.resMean_vs_eta.at(parName);
      delete resPlotCache.resWidth_vs_eta.at(parName);
      delete resPlotCache.res_vs_pT.at(parName);
      delete resPlotCache.resMean_vs_pT.at(parName);
      delete resPlotCache.resWidth_vs_pT.at(parName);
      delete resPlotCache.pull.at(parName);
      delete resPlotCache.pull_vs_eta.at(parName);
      delete resPlotCache.pullMean_vs_eta.at(parName);
      delete resPlotCache.pullWidth_vs_eta.at(parName);
      delete resPlotCache.pull_vs_pT.at(parName);
      delete resPlotCache.pullMean_vs_pT.at(parName);
      delete resPlotCache.pullWidth_vs_pT.at(parName);
    }
  }
}

void ResPlotTool::fill(
    ResPlotCache& resPlotCache, const Acts::GeometryContext& /*gctx*/,
    std::unique_ptr<const Acts::BoundTrackParameters> truthParameters,
    const Acts::BoundTrackParameters& fittedParameters) const {
  using ParametersVector = Acts::BoundTrackParameters::ParametersVector;
  using Acts::VectorHelpers::eta;
  using Acts::VectorHelpers::perp;
  // using Acts::VectorHelpers::phi;
  // using Acts::VectorHelpers::theta;

  // get the fitted parameter (at perigee surface) and its error
  auto trackParameter = fittedParameters.parameters();

  // get the truth position and momentum
  ParametersVector truthParameter = truthParameters->parameters();

  // get the truth eta and pT
  const auto truthEta = eta(truthParameters->momentum().normalized());
  const auto truthPt = truthParameters->absoluteMomentum() * perp(truthParameters->momentum().normalized());

  // fill the histograms for residual and pull
  for (unsigned int parID = 0; parID < Acts::eBoundSize; parID++) {
    std::string parName = m_paramNames.at(parID);
    float residual = trackParameter[parID] - truthParameter[parID];
    PlotHelpers::fillHisto(resPlotCache.res.at(parName), residual);
    PlotHelpers::fillHisto(resPlotCache.res_vs_eta.at(parName), truthEta, residual);
    PlotHelpers::fillHisto(resPlotCache.res_vs_pT.at(parName), truthPt, residual);

    if (fittedParameters.covariance().has_value()) {
      auto covariance = *fittedParameters.covariance();
      if (covariance(parID, parID) > 0) {
        float pull = residual / sqrt(covariance(parID, parID));
        PlotHelpers::fillHisto(resPlotCache.pull[parName], pull);
        PlotHelpers::fillHisto(resPlotCache.pull_vs_eta.at(parName), truthEta, pull);
        PlotHelpers::fillHisto(resPlotCache.pull_vs_pT.at(parName), truthPt, pull);
      } else {
        std::cout << "WARNING: Fitted track parameter :" << parName << " has negative covariance = " << covariance(parID, parID) << std::endl;
      }
    } else {
      std::cout << "WARNING: Fitted track parameter :" << parName << " has no covariance" << std::endl;
    }
  }
}


// get the mean and width of residual/pull in each eta/pT bin and fill them into histograms
void ResPlotTool::refinement(ResPlotTool::ResPlotCache& resPlotCache) const {
  PlotHelpers::Binning bEta = m_varBinning.at("Eta");
  PlotHelpers::Binning bPt = m_varBinning.at("Pt");
  for (unsigned int parID = 0; parID < Acts::eBoundSize; parID++) {
    std::string parName = m_paramNames.at(parID);
    // refine the plots vs eta
    for (int j = 1; j <= bEta.nBins; j++) {
      TH1D* temp_res = resPlotCache.res_vs_eta.at(parName)->ProjectionY(
          Form("%s_projy_bin%d", "Residual_vs_eta_Histo", j), j, j);
      PlotHelpers::anaHisto(temp_res, j,
                            resPlotCache.resMean_vs_eta.at(parName),
                            resPlotCache.resWidth_vs_eta.at(parName));

      TH1D* temp_pull = resPlotCache.pull_vs_eta.at(parName)->ProjectionY(
          Form("%s_projy_bin%d", "Pull_vs_eta_Histo", j), j, j);
      PlotHelpers::anaHisto(temp_pull, j,
                            resPlotCache.pullMean_vs_eta.at(parName),
                            resPlotCache.pullWidth_vs_eta.at(parName));
    }

    // refine the plots vs pT
    for (int j = 1; j <= bPt.nBins; j++) {
      TH1D* temp_res = resPlotCache.res_vs_pT.at(parName)->ProjectionY(
          Form("%s_projy_bin%d", "Residual_vs_pT_Histo", j), j, j);
      PlotHelpers::anaHisto(temp_res, j, resPlotCache.resMean_vs_pT.at(parName),
                            resPlotCache.resWidth_vs_pT.at(parName));

      TH1D* temp_pull = resPlotCache.pull_vs_pT.at(parName)->ProjectionY(
          Form("%s_projy_bin%d", "Pull_vs_pT_Histo", j), j, j);
      PlotHelpers::anaHisto(temp_pull, j,
                            resPlotCache.pullMean_vs_pT.at(parName),
                            resPlotCache.pullWidth_vs_pT.at(parName));
    }
  }
}
