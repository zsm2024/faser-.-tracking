#ifndef FASERACTSKALMANFILTER_RESPLOTTOOL_H
#define FASERACTSKALMANFILTER_RESPLOTTOOL_H

#include "PlotHelpers.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "HepMC3/GenParticle.h"
#include "TH1F.h"
#include "TH2F.h"
#include <map>
#include <string>
#include <vector>

class ResPlotTool {
public:
  std::vector<std::string> m_paramNames = {"x0", "y0", "phi", "theta", "qop", "t"};
  std::map<std::string, PlotHelpers::Binning> m_varBinning {
      {"Eta", PlotHelpers::Binning("#eta", 40, 4, 12)},
      {"Pt", PlotHelpers::Binning("pT [GeV/c]", 40, 0, 20)},
      {"Pull", PlotHelpers::Binning("pull", 100, -5, 5)},
      {"Residual_x0", PlotHelpers::Binning("r_{x0} [mm]", 100, -0.1, 0.1)},
      {"Residual_y0", PlotHelpers::Binning("r_{y0} [mm]", 100, -4, 4)},
      {"Residual_phi", PlotHelpers::Binning("r_{#phi} [rad]", 100, -0.3, 0.3)},
      {"Residual_theta", PlotHelpers::Binning("r_{#theta} [rad]", 100, -0.002, 0.002)},
      {"Residual_qop", PlotHelpers::Binning("r_{q/p} [c/GeV]", 100, -0.002, 0.002)},
      {"Residual_t", PlotHelpers::Binning("r_{t} [s]", 100, -1000, 1000)},
  };

  /// @brief Nested Cache struct
  struct ResPlotCache {
    std::map<std::string, TH1F*> res;               ///< Residual distribution
    std::map<std::string, TH2F*> res_vs_eta;        ///< Residual vs eta scatter plot
    std::map<std::string, TH1F*> resMean_vs_eta;    ///< Residual mean vs eta distribution
    std::map<std::string, TH1F*> resWidth_vs_eta;   ///< Residual width vs eta distribution
    std::map<std::string, TH2F*> res_vs_pT;         ///< Residual vs pT scatter plot
    std::map<std::string, TH1F*> resMean_vs_pT;     ///< Residual mean vs pT distribution
    std::map<std::string, TH1F*> resWidth_vs_pT;    ///< Residual width vs pT distribution

    std::map<std::string, TH1F*> pull;              ///< Pull distribution
    std::map<std::string, TH2F*> pull_vs_eta;       ///< Pull vs eta scatter plot
    std::map<std::string, TH1F*> pullMean_vs_eta;   ///< Pull mean vs eta distribution
    std::map<std::string, TH1F*> pullWidth_vs_eta;  ///< Pull width vs eta distribution
    std::map<std::string, TH2F*> pull_vs_pT;        ///< Pull vs pT scatter plot
    std::map<std::string, TH1F*> pullMean_vs_pT;    ///< Pull mean vs pT distribution
    std::map<std::string, TH1F*> pullWidth_vs_pT;   ///< Pull width vs pT distribution
  };

  /// Constructor
  ///
  ResPlotTool() = default;

  /// @brief book the histograms
  ///
  /// @param resPlotCache the cache for residual/pull histograms
  void book(ResPlotCache& resPlotCache) const;

  /// @brief fill the histograms
  ///
  /// @param resPlotCache the cache for residual/pull histograms
  /// @param gctx the geometry context
  /// @param truthParticle the truth particle
  /// @param fittedParamters the fitted parameters at perigee surface
  void fill(ResPlotCache& resPlotCache, const Acts::GeometryContext& gctx,
            std::unique_ptr<const Acts::BoundTrackParameters> truthParameters,
            const Acts::BoundTrackParameters& fittedParameters) const;

  /// @brief extract the details of the residual/pull plots and fill details
  ///
  /// into separate histograms
  /// @param resPlotCache the cache object for residual/pull histograms
  void refinement(ResPlotCache& resPlotCache) const;

  /// @brief write the histograms to output file
  ///
  /// @param resPlotCache the cache object for residual/pull histograms
  void write(const ResPlotCache& resPlotCache) const;

  /// @brief delele the histograms
  ///
  /// @param resPlotCache the cache object for residual/pull histograms
  void clear(ResPlotCache& resPlotCache) const;

private:
  const double m_MeV2GeV = 0.001;
};

#endif // FASERACTSKALMANFILTER_RESPLOTTOOL_H
