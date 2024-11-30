/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS and FASER
  collaborations
*/

#ifndef FASERACTSKALMANFILTER_IFIDUCIALEVENTSELECTIONTOOL_H
#define FASERACTSKALMANFILTER_IFIDUCIALEVENTSELECTIONTOOL_H

#include "CLHEP/Geometry/Point3D.h"

class IFiducialParticleTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IFiducialParticleTool, 1, 0);

  /** Check if truth hits for the given barcode exist and are in the
   * fiducial volume (r < 100mm) in stations 1 to 3.
   * @param barcode of a xAOD::TruthParticle
   */
  virtual bool isFiducial(int barcode) const = 0;

  /** Return average truth position in each station.
   * @param barcode of a xAOD::TruthParticle
   */
  virtual std::array<HepGeom::Point3D<double>, 4>
  getTruthPositions(int barcode) const = 0;

  /** Return average truth momentum in each station.
   * @param barcode of a xAOD::TruthParticle
   */
  virtual std::array<HepGeom::Point3D<double>, 4>
  getTruthMomenta(int barcode) const = 0;
};

#endif // FASERACTSKALMANFILTER_IFIDUCIALEVENTSELECTIONTOOL_H
