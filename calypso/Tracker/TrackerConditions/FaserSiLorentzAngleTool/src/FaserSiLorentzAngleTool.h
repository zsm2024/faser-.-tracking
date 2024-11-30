/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FaserSiLorentzAngleTool.h
 * @author Susumu.Oda@cern.ch
**/
#ifndef FaserSiLorentzAngleTool_h
#define FaserSiLorentzAngleTool_h

#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "FaserSiLorentzAngleTool/FaserSiLorentzAngleCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

//Gaudi Includes
#include "GaudiKernel/ServiceHandle.h"

class IdentifierHash;
namespace TrackerDD {
  class SiDetectorElement;
}

/**
 * @class FaserSiLorentzAngleTool
 * Concrete class for tool providing Lorentz angle (and the corresponding correction for the shift of the measurement) 
 * for each detector element.
**/

class FaserSiLorentzAngleTool: public extends<AthAlgTool, ISiLorentzAngleTool>
{
public:

  FaserSiLorentzAngleTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FaserSiLorentzAngleTool() = default;

  virtual StatusCode initialize() override;          //!< Service init
  virtual StatusCode finalize() override;            //!< Service finalize

  /** Get the Lorentz shift correction in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShift(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /**Get the Lorentz shift correction in the local y (etaDist) direction
     Assumes the center of the detector and is generally cached. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get tan af the Lorentz angle in the local x (phiDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get tan af the Lorentz angle in the local y (etaDist) direction
      Assumes the center of the detector and is generally cached. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash) const override;

  /** As above, but provide the local position. 
      More accurate but slower. */
  virtual double getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const override;

  /** Get bias voltage */
  virtual double getBiasVoltage(const IdentifierHash& elementHash) const override;

  /** Get temperature */
  virtual double getTemperature(const IdentifierHash& elementHash) const override;

  /** Get depletion voltage */
  virtual double getDepletionVoltage(const IdentifierHash& elementHash) const override;

private:
  enum Variable {LorentzShift, LorentzShiftEta, TanLorentzAngle, TanLorentzAngleEta};

  double getValue(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, Variable variable) const;
  double getCorrectionFactor() const;
  Amg::Vector3D getMagneticField(const Amg::Vector3D& pointvec) const;
  const FaserSiLorentzAngleCondData* getCondData() const;
  const TrackerDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash) const;

  // Properties
  std::string              m_detectorName;
  double                   m_nominalField;
  bool                     m_useMagFieldSvc;
  bool                     m_ignoreLocalPos;   // Makes methods using localPosition behave as method without passing localPosition.
  SG::ReadCondHandleKey<FaserSiLorentzAngleCondData> m_condData;
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_detEleCollKey{this, "DetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  // needed services
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  static const double s_invalidValue;
};

#endif // FaserSiLorentzAngleTool_h
