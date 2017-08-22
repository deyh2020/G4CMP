/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file library/src/G4CMPTrackLimiter.cc
/// \brief Implementation of the G4CMPTrackLimiter process, to kill tracks
///        falling below a minimum energy (set in G4CMPConfigManager).
//
// $Id$
//
// 20170822  M. Kelsey -- Add checking on current vs. original volume

#include "G4CMPTrackLimiter.hh"
#include "G4CMPConfigManager.hh"
#include "G4CMPUtils.hh"
#include "G4ForceCondition.hh"
#include "G4ParticleChange.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include <limits.h>


// Only applies to G4CMP particles

G4bool G4CMPTrackLimiter::IsApplicable(const G4ParticleDefinition& pd) {
  return (G4CMP::IsPhonon(pd) || G4CMP::IsChargeCarrier(pd));
}


// Force killing if below cut

G4double G4CMPTrackLimiter::
PostStepGetPhysicalInteractionLength(const G4Track& track, G4double,
                                     G4ForceCondition* condition) {
  *condition = Forced;
  return DBL_MAX;
}

G4VParticleChange* G4CMPTrackLimiter::PostStepDoIt(const G4Track& track,
                                                    const G4Step& step) {
  aParticleChange.Initialize(track);

  // Apply minimum energy cut to kill tracks with NIEL deposit
  if (BelowEnergyCut(track)) {
    aParticleChange.ProposeNonIonizingEnergyDeposit(track.GetKineticEnergy());
    aParticleChange.ProposeTrackStatus(fStopAndKill);
  }

  // Ensure that track is still in original, valid volume
  if (EscapedFromVolume(step)) {
    aParticleChange.SetNumberOfSecondaries(0);	// Don't launch bad tracks!
    aParticleChange.ProposeTrackStatus(fStopAndKill);
  }

  return &aParticleChange;
}


// Evaluate current track

G4bool G4CMPTrackLimiter::BelowEnergyCut(const G4Track& track) const {
  G4double ecut =
    (G4CMP::IsChargeCarrier(track) ? G4CMPConfigManager::GetMinChargeEnergy()
     : G4CMP::IsPhonon(track) ? G4CMPConfigManager::GetMinPhononEnergy() : -1.);

  return (track.GetKineticEnergy() < ecut);
}

G4bool G4CMPTrackLimiter::EscapedFromVolume(const G4Step& step) const {
  G4VPhysicalVolume* prePV  = step.GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postPV = step.GetPostStepPoint()->GetPhysicalVolume();

  // Track is NOT at a boundary, is stepping outside volume, or already escaped
  return ( (step.GetPostStepPoint()->GetStepStatus() != fGeomBoundary) &&
	   (postPV != GetCurrentVolume() || prePV != GetCurrentVolume())
	   );
}
