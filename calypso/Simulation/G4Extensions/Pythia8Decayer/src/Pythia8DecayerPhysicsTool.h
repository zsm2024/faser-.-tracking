/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS and FASER collaborations
*/

#ifndef PYTHIA8DECAYER_PYTHIA8DECAYERPHYSICSTOOL_H
#define PYTHIA8DECAYER_PYTHIA8DECAYERPHYSICSTOOL_H

// Include files
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IPhysicsOptionTool.h"
#include "G4VPhysicsConstructor.hh"

/** @class Pythia8DecayerPhysicsTool Pythia8DecayerPhysicsTool.h "Pythia8Decayer/Pythia8DecayerPhysicsTool.h"
 *
 *
 *
 *  @author Edoardo Farina (modified for FASER by D. Casper)
 *  @date  2015-05-14
 */
class Pythia8DecayerPhysicsTool :  public G4VPhysicsConstructor, public extends<AthAlgTool, IPhysicsOptionTool>
{
public:
  /// Standard constructor
  Pythia8DecayerPhysicsTool( const std::string& type , const std::string& name,
                             const IInterface* parent ) ;

  virtual ~Pythia8DecayerPhysicsTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  virtual void ConstructParticle();
  virtual void ConstructProcess();

  /** Implements
   */

  virtual Pythia8DecayerPhysicsTool* GetPhysicsOption();

};



#endif
