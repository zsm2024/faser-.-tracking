/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruthBase/FaserTrajectory.h"
#include "FaserMCTruthBase/FaserTruthStrategyManager.h"
#include "FaserMCTruth/FaserTruthController.h"
#include "FaserMCTruth/FaserTrackHelper.h"
#include "SimHelpers/TrackVisualizationHelper.h"

// Visualization stuff
#include "G4VVisManager.hh"
#include "G4Polyline.hh"
#include "G4Polymarker.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"


FaserTrajectory::FaserTrajectory(const G4Track* track, int subDetVolLevel)
  : G4Trajectory(track), m_subDetVolLevel(subDetVolLevel)
{}


void FaserTrajectory::AppendStep(const G4Step* aStep)
{
  // only use truth service if there are new any secondaries
  const int numSecondaries = aStep->GetSecondaryInCurrentStep()->size();
  // This method not available until G4 10.2
  //const int numSecondaries = aStep->GetNumberOfSecondariesInCurrentStep();
  if (numSecondaries) {
      // OK, there was an interation. look at the track, if it
      // is not a secondary (i.e. we have a connected tree) we
      // apply the MC truth machinery...
      FaserTrackHelper tHelper(aStep->GetTrack());
      if (!tHelper.IsSecondary())
        {
          FaserTruthStrategyManager* sManager =
            FaserTruthStrategyManager::GetStrategyManager();
          sManager->CreateTruthIncident(aStep, m_subDetVolLevel);
        }
    }

  // Call the base class
  G4Trajectory::AppendStep(aStep);
}

#if G4VERSION_NUMBER < 1010
void FaserTrajectory::DrawTrajectory(G4int i_mode=0) const
#else
void FaserTrajectory::DrawTrajectory(G4int i_mode) const
#endif
{
  // If i_mode>=0, draws a trajectory as a polyline (blue for
  // positive, red for negative, green for neutral) and, if i_mode!=0,
  // adds markers - yellow circles for step points and magenta squares
  // for auxiliary points, if any - whose screen size in pixels is
  // given by abs(i_mode)/1000.  E.g: i_mode = 5000 gives easily
  // visible markers.

  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (!pVVisManager) return;

  const G4double markerSize = abs(i_mode)*0.001;
  G4bool lineRequired (i_mode >= 0);
  G4bool markersRequired (markerSize > 0.);

  G4Polyline trajectoryLine;
  G4Polymarker stepPoints;
  G4Polymarker auxiliaryPoints;

  for (G4int i = 0; i < GetPointEntries() ; i++)
    {
      G4VTrajectoryPoint* aTrajectoryPoint = GetPoint(i);
      const std::vector<G4ThreeVector>* auxiliaries
        = aTrajectoryPoint->GetAuxiliaryPoints();
      if (auxiliaries)
        {
          for (size_t iAux = 0; iAux < auxiliaries->size(); ++iAux)
            {
              const G4ThreeVector pos((*auxiliaries)[iAux]);
              if (lineRequired)
                {
                  trajectoryLine.push_back(pos);
                }
              if (markersRequired)
                {
                  auxiliaryPoints.push_back(pos);
                }
            }
        }
      const G4ThreeVector pos(aTrajectoryPoint->GetPosition());
      if (lineRequired)
        {
          trajectoryLine.push_back(pos);
        }
      if (markersRequired)
        {
          stepPoints.push_back(pos);
        }
    }

  if (lineRequired)
    {
      int visScheme=FaserTruthController::getTruthController()->
        getVisualizationHelper()->trackVisualizationScheme();
      G4Colour colour;
      const G4double charge = GetCharge();
      if (visScheme==1)
        {
          // Geant3-like color code - neutrals are dashed
          const G4String pname=GetParticleName();
          if (pname=="e+" || pname=="e-") colour = G4Colour(1.,1.,0.);
          else if (pname=="mu+" || pname=="mu-") colour = G4Colour(0.,1.,0.);
          else if (pname=="gamma") colour = G4Colour(0.,0.,1.);
          else if (pname=="neutron") colour = G4Colour(0.,0.,0.);
          else if (charge) colour = G4Colour(1.,0.,0.);
        }
      else if (visScheme==2)
        {
          if(charge>0.)      colour = G4Colour(0.,0.,1.); // Blue = positive.
          else if(charge<0.) colour = G4Colour(1.,0.,0.); // Red = negative.
          else               colour = G4Colour(0.,1.,0.); // Green = neutral.
        }
      else if (visScheme==3)
        {
          const G4String pname=GetParticleName();
          if (pname=="e+" || pname=="e-") colour = G4Colour(1.,1.,0.);
          else if (pname=="mu+" || pname=="mu-") colour = G4Colour(0.,0.,1.);
          else if (pname=="gamma") colour = G4Colour(60./256.,79./256.,113./256.);
          else if (pname=="neutron") colour = G4Colour(1.,1.,1.);
          else if (pname=="pi-"|| pname=="pi+")
            colour = G4Colour(250/256.,128/256.,114/256.);
          else colour = G4Colour(176/256.,48/256.,96/256.);
        }
      G4VisAttributes trajectoryLineAttribs(colour);
      if(visScheme==1 && charge==0)
        {
          G4VisAttributes::LineStyle style=G4VisAttributes::dotted;
          trajectoryLineAttribs.SetLineStyle(style);
        }
      trajectoryLine.SetVisAttributes(&trajectoryLineAttribs);
      pVVisManager->Draw(trajectoryLine);
    }
  if (markersRequired)
    {
      auxiliaryPoints.SetMarkerType(G4Polymarker::squares);
      auxiliaryPoints.SetScreenSize(markerSize);
      auxiliaryPoints.SetFillStyle(G4VMarker::filled);
      G4VisAttributes auxiliaryPointsAttribs(G4Colour(0.,1.,1.));  // Magenta
      auxiliaryPoints.SetVisAttributes(&auxiliaryPointsAttribs);
      pVVisManager->Draw(auxiliaryPoints);

      stepPoints.SetMarkerType(G4Polymarker::circles);
      stepPoints.SetScreenSize(markerSize);
      stepPoints.SetFillStyle(G4VMarker::filled);
      G4VisAttributes stepPointsAttribs(G4Colour(1.,1.,0.));  // Yellow.
      stepPoints.SetVisAttributes(&stepPointsAttribs);
      pVVisManager->Draw(stepPoints);
    }
}
