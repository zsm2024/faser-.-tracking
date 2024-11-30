#ifndef FASERACTSKALMANFILTER_FIDUCIALEVENTSELECTIONTOOL_H
#define FASERACTSKALMANFILTER_FIDUCIALEVENTSELECTIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserActsKalmanFilter/IFiducialParticleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}

class FiducialParticleTool : public extends<AthAlgTool, IFiducialParticleTool> {
public:
  FiducialParticleTool(const std::string &type, const std::string &name,
                       const IInterface *parent);
  virtual ~FiducialParticleTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  bool isFiducial(int barcode) const override;

  std::array<HepGeom::Point3D<double>, 4>
  getTruthPositions(int barcode) const override;
  std::array<HepGeom::Point3D<double>, 4>
  getTruthMomenta(int barcode) const override;
  HepGeom::Point3D<double>
  getMomentum(const FaserSiHit &hit) const;

private:
  HepGeom::Point3D<double> getGlobalPosition(const FaserSiHit &hit) const;

  SG::ReadHandleKey<FaserSiHitCollection> m_siHitCollectionKey{
      this, "FaserSiHitCollection", "SCT_Hits"};

  const FaserSCT_ID *m_sctHelper{nullptr};
  const TrackerDD::SCT_DetectorManager *m_detMgr{nullptr};
};

#endif // FASERACTSKALMANFILTER_FIDUCIALEVENTSELECTIONTOOL_H
