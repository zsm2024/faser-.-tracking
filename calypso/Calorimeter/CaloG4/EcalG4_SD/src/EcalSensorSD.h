/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Ecal Sensitive Detector class
****************************************************************/

#ifndef ECALG4_SD_ECALSENSORSD_H
#define ECALG4_SD_ECALSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// For the hits
#include "FaserCaloSimEvent/CaloHitCollection.h"
#include "StoreGate/WriteHandle.h"

// G4 needed classes
class G4Step;
class G4TouchableHistory;

// G4 needed classes
#include "Geant4/G4EnergyLossTables.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4MaterialCutsCouple.hh"

class EcalSensorSD : public G4VSensitiveDetector
{
public:
  // Constructor
  EcalSensorSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~EcalSensorSD() { /* If all goes well we do not own myHitColl here */ }

  // Deal with each G4 hit
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

  // For setting up the hit collection
  void Initialize(G4HCofThisEvent*) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
      could get rather tricky, but the idea is to allow fast simulations to use the very
      same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

  /// Set the first coefficient of Birks's law   
  void birk_c1 (double c1) { m_birk_c1 = c1 ; }   
  /// Set the second coefficient of Birks's law   
  void birk_c2 (double c2) { m_birk_c2 = c2; }   
  /// Set the correction to the first coefficient of Birks's law   
  void birk_c1cor (double c1cor) { m_birk_c1correction = c1cor; }

  /// Amplitudes of the local non uniformity correction
  void local_non_uniformity (double local) { m_a_local_outer_ecal = local ; }
  /// Amplitudes of the global non uniformity correction
  void global_non_uniformity (double global) { m_a_global_outer_ecal = global ; }

  /// Correction for light reflection at the edges
  void reflection_height(double h) { m_a_reflection_height = h ; }
  void reflection_width(double w) { m_a_reflection_width = w ; }

private:
  void indexMethod(const G4TouchableHistory *myTouch, int &station, int &plate);
  static const uint32_t kModuleDepth { 3 };
  static const uint32_t kFiberDepth  { 5 };

protected:
  // The hits collection
  SG::WriteHandle<CaloHitCollection> m_HitColl;

private:
  // the first coefficient of Birk's law                    (c1)   
  double m_birk_c1             ;   
  // the second coefficient of Birk's law                   (c2)   
  double m_birk_c2             ;   
  // the correction to the first coefficient of Birk's law  (c1')   
  double m_birk_c1correction   ;
  
  // Amplitudes of the local non uniformity correction
  double m_a_local_outer_ecal  ;
  // Amplitudes of the global non uniformity correction
  double m_a_global_outer_ecal  ;
  // Correction for light reflection at the edges
  double  m_a_reflection_height ;
  double  m_a_reflection_width  ;

protected:
  /// the first coefficient of Birks's law   
  inline double birk_c1 () const { return m_birk_c1 ; }   
  /// the second coefficient of Birks's law   
  inline double birk_c2 () const { return m_birk_c2 ; }   
  /// the correction to the first coefficient of Birks's law   
  inline double birk_c1cor () const   { return m_birk_c1correction ; }
  
  /** Correction factor from Birk's Law
   *  @param step current G4step
   *  @return the correction factor
   */
  inline double birkCorrection ( const G4Step* step ) const ;
  
  /** Birk's correction for given particle with given kinetic energy
   *  for the given material
   *  @param  particle pointer to particle definition
   *  @param  Ekine    particle kinetic energy
   *  @param  maerial  pointer ot teh material
   */
  inline double  birkCorrection
  ( const G4ParticleDefinition* particle ,
    const double                eKine    ,
    const G4MaterialCutsCouple* material ) const ;
  
  /** evaluate the correction for Birk's law
   *  @param charge   the charge of the particle
   *  @param dEdX     the nominal dEdX in the material
   *  @param material the pointer ot teh material
   *  @return the correction coefficient
   */
  inline double  birkCorrection
  ( const double      charge   ,
    const double      dEdX     ,
    const G4Material* material ) const ;
  
  /** evaluate the correction for Birk's law
   *  @param charge   the charge of the particle
   *  @param dEdX     the nominal dEdX in the material
   *  @param density  the density ot the material
   *  @return the correction coefficient
   */
  inline double  birkCorrection
  ( const double      charge   ,
    const double      dEdX     ,
    const double      density  ) const ;
  
  /** Correction due to the local non uniformity due to the light
   *  collection efficiency in cell
   */
  inline double localNonUniformity (double x, double y) const ;

  /** The fractions of energy deposited in consequitive time-bins
   *  in the given Ecal/Hcal cell
   *  @see CaloSensDet
   *  @param time global time of energy deposition
   *  @param cell cellID of the cell
   *  @param slot (out) the first time bin
   *  @param fracs (out) the vector of fractions for subsequent time-slots;
   *  @return StatuscCode
   */
//   StatusCode timing
//   ( const double            time      ,
//     Fractions& fractions ) const;

  
};

// ============================================================================
/** evaluate the correction for Birk's law
 *  @param charge   the charge of the particle
 *  @param dEdX     the nominal dEdX in the material
 *  @param density
 *  @return the correction coefficient
 */
// ============================================================================
inline double  EcalSensorSD::birkCorrection
( const double      charge   ,
  const double      dEdX     ,
  const double      density  ) const
{
  const double C1 =
    fabs( charge )  < 1.5 ? birk_c1() : birk_c1() * birk_c1cor() ;
  const double C2 = birk_c2() ;

  const double alpha = dEdX/ density ;

  return 1.0 / ( 1.0 + C1 * alpha + C2 * alpha * alpha ) ;
}

// ============================================================================
/** evaluate the correction for Birk's law
 *  @param charge   the charge of the particle
 *  @param dEdX     the nominal dEdX in the material
 *  @param material the pointer ot teh material
 *  @return the correction coefficient
 */
// ============================================================================
inline double  EcalSensorSD::birkCorrection
( const double      charge   ,
  const double      dEdX     ,
  const G4Material* material ) const
{
  if ( 0 == material )
    { std::cerr << "birkCorrection(): invalid material " << std::endl; return 1. ; } // RETURN
  return birkCorrection( charge , dEdX , material->GetDensity() ) ;
}

// ============================================================================
/** Birk's correction for given particle with given kinetic energy
 *  for the given material
 *  @param  particle pointer to particle definition
 *  @param  Ekine    particle kinetic energy
 *  @param  maerial  pointer ot teh material
 */
// ============================================================================
inline double EcalSensorSD::birkCorrection
( const G4ParticleDefinition* particle ,
  const double                eKine    ,
  const G4MaterialCutsCouple* material ) const
{
  if (  0 == particle || 0 == material )
    { std::cerr << "birkCorrection(): invalid parameters " << std::endl; return 1.0 ; } // RETURN

  const double charge = particle -> GetPDGCharge() ;
  if ( fabs ( charge ) < 0.1                      ) { return 1.0 ; } // EEUTRN

  // get the nominal dEdX
  const double dEdX  =
    G4EnergyLossTables::GetDEDX ( particle , eKine , material ) ;

  // make an actual evaluation
  return birkCorrection
    ( charge ,
      dEdX   ,
      material->GetMaterial()->GetDensity() ) ;
}

// ============================================================================
// Birk's Law
// ============================================================================
/** Correction factor from Birk's Law
 *  Factor = 1/(1+C1*dEdx/rho+C2*(dEdx/rho)^2)
 *  Where :
 *      - C1 = 0.013 g/MeV/cm^2 [Ref NIM 80 (1970) 239]
 *      - C2 = 9.6.10^-6 g^2/MeV^2/cm^4
 *      - dEdx in MeV/cm
 *      - rho = density in g/cm^3
 */
// ============================================================================
inline double EcalSensorSD::birkCorrection ( const G4Step* step ) const
{
  if ( !step ) { return 1 ; }                               // RETURN

  // Track
  const G4Track* track  = step->GetTrack() ;
  const double charge   = track->GetDefinition()->GetPDGCharge()  ;

  // Only for charged tracks
  if ( fabs( charge ) < 0.1 ) { return 1 ; }                 // RETURN

  // make an actual evaluation
  return birkCorrection
    ( track->GetDefinition         () ,
      track->GetKineticEnergy      () ,
      track->GetMaterialCutsCouple () ) ;
}
// ============================================================================

inline double EcalSensorSD::localNonUniformity (double x, double y) const
{

  // Only for ECal for the moment
  double correction = 1. ;

  // Find the position of the step relative to the centre of the cell
  // Done before passing in so set centres x0 = y0 = 0
  double x0       = 0;
  double y0       = 0;


  // Distance between fibers
  // and correction amplitude
  double d        = 15.25 * CLHEP::mm ;
  double A_local  = m_a_local_outer_ecal ; 
  double A_global = m_a_global_outer_ecal ;

  // Cell size
  double cellSize = 121.9 * CLHEP::mm; 

  float lowTolerance = 1e-20;

  // Local uniformity is product of x and y sine-like functions
  // The Amplitude of the sin-like function is a function of x and y
  // Deion: changed LHCb's parametrization by adding the (- A_local) at the end in order to center the correction on zero
  if ( A_local > lowTolerance )
    correction += A_local / 2. * ( 1. - cos( 2. * CLHEP::pi * (x-x0)/d ) ) * ( 1. - cos( 2. * CLHEP::pi * (y-y0)/d ) ) - A_local ;

  double rX(0.) , rY(0.) , hCell(0.) ;

  // Global non uniformity
  if ( A_global > lowTolerance ) {
    rX = x - x0 ;
    rY = y - y0 ;
    hCell = cellSize / 2. ;
    correction +=
      A_global * ( hCell - rX ) * ( rX + hCell ) / ( hCell * hCell )
      * ( hCell - rY ) * ( rY + hCell ) / ( hCell * hCell ) ;
  }

  // Light Reflexion on the edges
  if ( m_a_reflection_height > lowTolerance ) {
    rX = rX / m_a_reflection_width ;
    rY = rY / m_a_reflection_width ;
    hCell = hCell / m_a_reflection_width ;
    correction += m_a_reflection_height *
      ( exp( - fabs ( rX + hCell ) ) + exp( - fabs ( rX - hCell ) )
        + exp( - fabs ( rY + hCell ) ) + exp( - fabs ( rY - hCell ) ) ) ;
  }

  return correction ;
}

#endif //ECALG4_SD_ECALSENSORSD_H
