//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file library/src/G4eLukeScattering.cc
/// \brief Implementation of the G4eLukeScattering class
//
// $Id$
//
// 20140325  Move time-step calculation to G4CMPProcessUtils
// 20140331  Add required process subtype code
// 20140415  Add run-time flag to select valley vs. H-V kinematics

#include "G4CMPeLukeScattering.hh"
#include "G4CMPDriftElectron.hh"
#include "G4CMPValleyTrackMap.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4PhysicalConstants.hh"
#include "G4RandomDirection.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4VParticleChange.hh"
#include "Randomize.hh"
#include <fstream>
#include <iostream>


G4CMPeLukeScattering::G4CMPeLukeScattering(G4VProcess* sLim)
  : G4CMPVDriftProcess("eLukeScattering", fLukeScattering),
    stepLimiter(sLim) {;}

G4CMPeLukeScattering::~G4CMPeLukeScattering() {;}


G4double G4CMPeLukeScattering::GetMeanFreePath(const G4Track& aTrack,
					       G4double,
					       G4ForceCondition* condition) {
  *condition = Forced;

  G4int iv = GetValleyIndex(aTrack);
  G4ThreeVector v = theLattice->MapPtoV_el(iv,GetLocalMomentum(aTrack));
  G4ThreeVector k_HV = theLattice->MapPtoK_HV(iv,GetLocalMomentum(aTrack));
  G4double kmag = k_HV.mag();

#ifdef G4CMP_DEBUG
  G4cout << "eLuke v = " << v.mag()/m*s << " kmag = " << kmag*m
	 << "\nv = " << v << "\nk_HV = " << k_HV
	 << "\nk_valley = "
	 << theLattice->MapPtoK_valley(iv,GetLocalMomentum(aTrack))
	 << G4endl;
#endif

  if (kmag<=ksound_e) return DBL_MAX;
 
  // Time step corresponding to Mach number (avg. time between radiations)
  G4double dtau = ChargeCarrierTimeStep(kmag/ksound_e, l0_e);
  
  G4double mfp = dtau * v.mag();
#ifdef G4CMP_DEBUG
  G4cout << "eLuke MFP = " << mfp/m << G4endl;
#endif
  return mfp;
}

// TEMPORARY:  Run-time flag to switch HV vs. Valley kinematics
const G4bool eLuke_valley_kinematics = (getenv("ELUKE_VALLEY_KINEMATICS")!=0);

G4VParticleChange* G4CMPeLukeScattering::PostStepDoIt(const G4Track& aTrack,
						      const G4Step& aStep) {
  aParticleChange.Initialize(aTrack); 
  G4StepPoint* postStepPoint = aStep.GetPostStepPoint();
  
  // Do nothing other than re-calculate mfp when step limit reached or leaving
  // volume
#ifdef G4CMP_DEBUG
  G4cout << GetProcessName() << "::PostStepDoIt: Step limited by process "
	 << postStepPoint->GetProcessDefinedStep()->GetProcessName()
	 << G4endl;
#endif

  if ( (postStepPoint->GetProcessDefinedStep()==stepLimiter)
       || (postStepPoint->GetStepStatus()==fGeomBoundary) ) {
    return &aParticleChange;
  }

  G4int iv = GetValleyIndex(aTrack);
  G4ThreeVector p = GetLocalDirection(postStepPoint->GetMomentum());
  G4ThreeVector k_HV = theLattice->MapPtoK_HV(iv, p);
  G4double kmag = k_HV.mag();

  // Construct phonon pseudovector in Henning-Vogt space
  G4double theta_phonon = MakePhononTheta(kmag, ksound_e);
  G4double phi_phonon   = G4UniformRand()*twopi;
  G4double q = 2*(kmag*cos(theta_phonon)-ksound_e);

  // Sanity check for phonon production: should be forward going, like Cherenkov
  if (theta_phonon>acos(ksound_e/kmag) || theta_phonon>halfpi) {
    G4cerr << "ERROR: Phonon production theta_phonon " << theta_phonon
	   << " exceeds cone angle " << acos(ksound_e/kmag) << G4endl;
  }

  // Compute kinematics in either Herring-Vogt or physical (valley) space
  G4ThreeVector kdir, qvec, k_recoil, p_new;

  if (eLuke_valley_kinematics) {	// Use vectors in valley frame
#ifdef G4CMP_DEBUG
    G4cout << "Using kinematics in valley frame, not H-V" << G4endl;
#endif

    G4ThreeVector k_valley = theLattice->MapPtoK_valley(iv, p);
    kdir = k_valley.unit();
    qvec = q*kdir;
    qvec.rotate(kdir.orthogonal(), theta_phonon);
    qvec.rotate(kdir, phi_phonon);
    k_recoil = k_valley - qvec;
    p_new = theLattice->MapK_valleyToP(iv, k_recoil);
  } else {				// Use H-V vectors for everything
    kdir = k_HV.unit();
    qvec = q*kdir;
    qvec.rotate(kdir.orthogonal(), theta_phonon);
    qvec.rotate(kdir, phi_phonon);
    k_recoil = k_HV - qvec;
    p_new = theLattice->MapK_HVtoP(iv, k_recoil);
  }

  RotateToGlobalDirection(p_new);	// Put into global frame for tracks

  /*** NOT READY UNTIL EFFECTIVE MASS IS BEING USED EVERYWHERE
  // Electron kinetic energy computed in valley frame using mass tensor
  G4ThreeVector p_v = theLattice->GetValley(iv) * p_new;
  G4ThreeVector psq(p_v.x()*p_v.x(), p_v.y()*p_v.y(), p_v.z()*p_v.z());
  psq *= theLattice->GetMInvTensor();
  G4double Enew = psq.mag() / 2.;		// 1/2 p^2/m, non-relativistic
  G4double Mnew = p_new.mag2() / (2.*Enew);	// Effective scalar mass
  ***/
  G4double Enew = 0.5*p_new.mag2()/(mc_e*c_squared);
  G4double Etrack = postStepPoint->GetKineticEnergy();
  if (Enew > Etrack) {
    G4cerr << "ERROR:  Energy is not conserved!  Etrack = " << Etrack
	   << " less than Enew = " << Enew << G4endl;
  }

  // Convert phonon pseudovector to real space, compute energy
  G4ThreeVector Pphonon;
  if (eLuke_valley_kinematics)
    Pphonon = theLattice->MapK_valleyToP(iv, qvec);
  else
    Pphonon = theLattice->MapK_HVtoP(iv, qvec);

  RotateToGlobalDirection(Pphonon);

  G4double Ephonon = Etrack - Enew;		// E conservation

  // FIXME:  Need to generate actual phonon!
  
#ifdef G4CMP_DEBUG
  G4cout << "p (post-step) = " << p << "\np_mag = " << p.mag()
	 << " M_track = " << p.mag2() / (2.*Etrack)
	 << "\nk_HV = " << k_HV << "\nkmag = " << kmag
	 << "\ntheta_phonon = " << theta_phonon << " phi_phonon = " << phi_phonon
	 << "\nacos(ks/k)   = " << acos(ksound_e/kmag)
	 << "\nq = " << q << "\nqvec = " << qvec
	 << "\nk_recoil = " << k_recoil << "\nk_recoil-mag = " << k_recoil.mag()
	 << "\nPphonon = " << Pphonon << "\nEphonon = " << Ephonon
	 << "\np_new     = " << p_new << "\np_new_mag = " << p_new.mag()
	 << " M_new = " << p_new.mag2() / (2.*Enew)
	 << "\nEtrack = " << Etrack << " Enew = " << Enew
	 << G4endl;
#endif

  aParticleChange.ProposeMomentumDirection(p_new.unit());
  aParticleChange.ProposeEnergy(Enew);
  aParticleChange.ProposeNonIonizingEnergyDeposit(Ephonon);

  ResetNumberOfInteractionLengthLeft();
  return &aParticleChange;
}


G4bool G4CMPeLukeScattering::IsApplicable(const G4ParticleDefinition& aPD) {
  return (&aPD==G4CMPDriftElectron::Definition());
}
