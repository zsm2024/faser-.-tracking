#ifndef SIMWRITERTOOL_H
#define SIMWRITERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/Definitions/Algebra.hpp"
class TFile;
class TTree;


class SimWriterTool : public AthAlgTool {
 public:
  SimWriterTool(const std::string& type,
      const std::string& name, const IInterface* parent);

  virtual ~SimWriterTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  void writeout(Acts::Vector3 vertex, Acts::Vector3 momentum) const;

 private:
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "SimData.root", "Output root file for simulated data"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "tree", ""};
  TFile* m_file;
  TTree* m_tree;

  mutable int m_eventNumber;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
  mutable double m_px;
  mutable double m_py;
  mutable double m_pz;
};

#endif // SIMWRITERTOOL_H
