#ifndef FASERACTSKALMANFILTER_SUMMARYPLOTTOOL_H
#define FASERACTSKALMANFILTER_SUMMARYPLOTTOOL_H

#include "PlotHelpers.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "TProfile.h"
#include <map>
#include <string>

class SummaryPlotTool {
public:
  std::map<std::string, PlotHelpers::Binning> m_varBinning {
      {"Eta", PlotHelpers::Binning("#eta", 40, 4, 12)},
      {"Phi", PlotHelpers::Binning("#phi", 100, -3.15, 3.15)},
      {"Pt", PlotHelpers::Binning("pT [GeV/c]", 40, 0, 20)},
      {"Num", PlotHelpers::Binning("N", 30, -0.5, 29.5)}
  };

  /// @brief Nested Cache struct
  struct SummaryPlotCache {
    TProfile* nStates_vs_eta;        ///< Number of total states vs eta
    TProfile* nMeasurements_vs_eta;  ///< Number of non-outlier measurements vs eta
    TProfile* nHoles_vs_eta;         ///< Number of holes vs eta
    TProfile* nOutliers_vs_eta;      ///< Number of outliers vs eta
    TProfile* nSharedHits_vs_eta;    ///< Number of Shared Hits vs eta
    TProfile* nStates_vs_pt;         ///< Number of total states vs pt
    TProfile* nMeasurements_vs_pt;   ///< Number of non-outlier measurements vs pt
    TProfile* nHoles_vs_pt;          ///< Number of holes vs pt
    TProfile* nOutliers_vs_pt;       ///< Number of outliers vs pt
    TProfile* nSharedHits_vs_pt;     ///< Number of Shared Hits vs pt
  };

  /// Constructor
  ///
  SummaryPlotTool() = default;

  /// @brief book the track info plots
  ///
  /// @param trackSummaryPlotCache the cache for track info plots
  void book(SummaryPlotCache& trackSummaryPlotCache) const;

  /// @brief fill reco track info w.r.t. fitted track parameters
  ///
  /// @param trackSummaryPlotCache cache object for track info plots
  /// @param fittedParameters fitted track parameters of this track
  /// @param nStates number of track states
  /// @param nMeasurements number of measurements
  /// @param nOutliers number of outliers
  /// @param nHoles number of holes
  /// @param nSharedHits number of shared hits
  void fill(SummaryPlotCache& trackSummaryPlotCache,
            const Acts::BoundTrackParameters& fittedParameters, size_t nStates,
            size_t nMeasurements, size_t nOutliers, size_t nHoles,
            size_t nSharedHits) const;

  /// @brief write the track info plots to file
  ///
  /// @param trackSummaryPlotCache cache object for track info plots
  void write(const SummaryPlotCache& trackSummaryPlotCache) const;

  /// @brief delete the track info plots
  ///
  /// @param trackSummaryPlotCache cache object for track info plots
  void clear(SummaryPlotCache& trackSummaryPlotCache) const;
};

#endif // FASERACTSKALMANFILTER_SUMMARYPLOTTOOL_H
