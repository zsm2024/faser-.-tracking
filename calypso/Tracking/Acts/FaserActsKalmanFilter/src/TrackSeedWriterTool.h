#ifndef FASERACTSKALMANFILTER_TRACKSEEDWRITERTOOL_H
#define FASERACTSKALMANFILTER_TRACKSEEDWRITERTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"

class TFile;
class TTree;

class TrackSeedWriterTool : public  AthAlgTool {
 public:
  TrackSeedWriterTool(const std::string& type,
                      const std::string& name, const IInterface* parent);
  virtual ~TrackSeedWriterTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  void writeout(const Acts::GeometryContext& gctx,
                const std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>>& initialParameters) const;

private:
  void initializeTree();
  void clearVariables() const;
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "TrackSeeds.root", ""};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "tree", ""};
  TFile* m_file;
  TTree* m_tree;

  mutable int m_runNumber;
  mutable int m_eventNumber;
  mutable std::vector<float> m_eLOC0;
  mutable std::vector<float> m_eLOC1;
  mutable std::vector<float> m_ePHI;
  mutable std::vector<float> m_eTHETA;
  mutable std::vector<float> m_eQOP;
  mutable std::vector<float> m_err_eLOC0;
  mutable std::vector<float> m_err_eLOC1;
  mutable std::vector<float> m_err_ePHI;
  mutable std::vector<float> m_err_eTHETA;
  mutable std::vector<float> m_err_eQOP;
  mutable std::vector<float> m_x;
  mutable std::vector<float> m_y;
  mutable std::vector<float> m_z;
  mutable std::vector<float> m_px;
  mutable std::vector<float> m_py;
  mutable std::vector<float> m_pz;
};


#endif // FASERACTSKALMANFILTER_TRACKSEEDWRITERTOOL_H
