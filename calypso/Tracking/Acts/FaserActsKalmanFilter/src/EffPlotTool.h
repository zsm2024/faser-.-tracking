#ifndef FASERACTSKALMANFILTER_EFFPLOTTOOL_H
#define FASERACTSKALMANFILTER_EFFPLOTTOOL_H

#include "PlotHelpers.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "HepMC3/GenParticle.h"
#include "TEfficiency.h"
#include "TProfile.h"
#include <map>
#include <string>

class EffPlotTool {
public:
  std::map<std::string, PlotHelpers::Binning> m_varBinning = {
      {"Eta", PlotHelpers::Binning("#eta", 40, 4, 12)},
      {"Phi", PlotHelpers::Binning("#phi", 100, -3.15, 3.15)},
      {"Pt", PlotHelpers::Binning("pT [GeV/c]", 40, 0, 20)},
      {"DeltaR", PlotHelpers::Binning("#Delta R", 100, 0, 0.3)}
  };

  /// @brief Nested Cache struct
  struct EffPlotCache {
    TEfficiency* trackEff_vs_pT{nullptr};   ///< Tracking efficiency vs pT
    TEfficiency* trackEff_vs_eta{nullptr};  ///< Tracking efficiency vs eta
    TEfficiency* trackEff_vs_phi{nullptr};  ///< Tracking efficiency vs phi
    TEfficiency* trackEff_vs_DeltaR{
        nullptr};  ///< Tracking efficiency vs distance to the closest truth
                   ///< particle
  };

  /// Constructor
  ///
  EffPlotTool() = default;

  /// @brief book the efficiency plots
  ///
  /// @param effPlotCache the cache for efficiency plots
  void book(EffPlotCache& effPlotCache) const;

  /// @brief fill efficiency plots
  ///
  /// @param effPlotCache cache object for efficiency plots
  /// @param truthParticle the truth Particle
  /// @param status the reconstruction status
  void fill(EffPlotCache& effPlotCache, const HepMC3::GenParticle* truthParticle, bool status) const;

  /// @brief write the efficiency plots to file
  ///
  /// @param effPlotCache cache object for efficiency plots
  void write(const EffPlotCache& effPlotCache) const;

  /// @brief delete the efficiency plots
  ///
  /// @param effPlotCache cache object for efficiency plots
  void clear(EffPlotCache& effPlotCache) const;

private:
  const double m_MeV2GeV = 0.001;
};

#endif // FASERACTSKALMANFILTER_EFFPLOTTOOL_H
