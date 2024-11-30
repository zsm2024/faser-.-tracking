/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserTruthController.h"
#include "SimHelpers/TrackVisualizationHelper.h"

FaserTruthController* FaserTruthController::s_thePointer=FaserTruthController::getTruthController() ;

FaserTruthController* FaserTruthController::getTruthController()
{
  if (!s_thePointer) s_thePointer = new FaserTruthController;
  return s_thePointer;
}

FaserTruthController::FaserTruthController()
{
  m_theVisHelper=new TrackVisualizationHelper;
}

FaserTruthController::~FaserTruthController()
{
  if (m_theVisHelper) delete m_theVisHelper;
}
void FaserTruthController::setVisualizationHelper(TrackVisualizationHelper *h)
{
  if (m_theVisHelper==h) return;
  delete m_theVisHelper;
  m_theVisHelper=h;
}
