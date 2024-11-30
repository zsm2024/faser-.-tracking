/*
  Copyright (C) 2002-2018 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERGEOMETRY_FASERMAGNETICFIELDWRAPPER_H
#define FASERGEOMETRY_FASERMAGNETICFIELDWRAPPER_H

#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/MagneticField/MagneticFieldProvider.hpp"

class FASERMagneticFieldWrapper final : public Acts::MagneticFieldProvider {

public:

  struct Cache {

    Cache(const Acts::MagneticFieldContext mctx)
    : m_faserField{mctx.get<const FaserFieldCacheCondObj*>()} {
      m_faserField->getInitializedCache(fieldCache);
    }

    Cache(const Cache& other) : m_faserField{other.m_faserField} {
      m_faserField->getInitializedCache(fieldCache);
    }

    Cache& operator=(const Cache& other) {
      m_faserField = other.m_faserField;
      fieldCache = {};
      m_faserField->getInitializedCache(fieldCache);
      return *this;
    }

    MagField::FaserFieldCache fieldCache;

    private:
      const FaserFieldCacheCondObj* m_faserField;
  };

  FASERMagneticFieldWrapper() = default;


  MagneticFieldProvider::Cache 
  makeCache(const Acts::MagneticFieldContext& mctx) const override {
    return Acts::MagneticFieldProvider::Cache(std::in_place_type<Cache>, mctx);
  }

  Acts::Result<Acts::Vector3>
  getField(const Acts::Vector3& position, Acts::MagneticFieldProvider::Cache& gcache) const override {
    Cache& cache = gcache.as<Cache>();
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];

    cache.fieldCache.getField(posXYZ, BField);

    // Magnetic field
    Acts::Vector3 bfield{BField[0],BField[1],BField[2]};

    bfield *= m_bFieldUnit; // kT -> T;

    return Acts::Result<Acts::Vector3>::success(bfield);
  }

  Acts::Result<Acts::Vector3>
  getFieldGradient(const Acts::Vector3& position,
                   Acts::ActsMatrix<3, 3>& gradient,
                   Acts::MagneticFieldProvider::Cache& gcache) const override
  {
    Cache& cache = gcache.as<Cache>();
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];
    double grad[9];

    cache.fieldCache.getField(posXYZ, BField, grad);

    // Magnetic field
    Acts::Vector3 bfield{BField[0], BField[1],BField[2]};
    Acts::ActsMatrix<3, 3> tempGrad;
    tempGrad << grad[0], grad[1], grad[2], grad[3], grad[4], grad[5], grad[6], grad[7], grad[8]; 
    gradient = tempGrad;


    bfield *= m_bFieldUnit; // kT -> T;
    gradient *= m_bFieldUnit;

    return Acts::Result<Acts::Vector3>::success(bfield);
  }

private:
  const double m_bFieldUnit = 1000.*Acts::UnitConstants::T;
};


#endif
