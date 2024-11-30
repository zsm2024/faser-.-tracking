/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRENCHDETECTORTOOL_H
#define TRENCHDETECTORTOOL_H

#include "GeoModelFaserUtilities/GeoModelTool.h"
class TrenchDetectorManager;

class TrenchDetectorTool final : public GeoModelTool 
{
 public:

  // Standard Constructor
  TrenchDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~TrenchDetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  
 private:
  const TrenchDetectorManager* m_manager;
};

#endif 
