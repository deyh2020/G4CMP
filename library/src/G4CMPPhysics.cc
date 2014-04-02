// $Id$
//
// Create particles and physics processes for phonons and charge carriers
// Usage:  [physics-list]->AddPhysics(new G4CMPPhysics(<verbose>));

#include "G4CMPPhysics.hh"
#include "G4CMPDriftBoundaryProcess.hh"
#include "G4CMPDriftElectron.hh"
#include "G4CMPDriftHole.hh"
#include "G4CMPInterValleyScattering.hh"
#include "G4CMPTimeStepper.hh"
#include "G4CMPeLukeScattering.hh"
#include "G4CMPhLukeScattering.hh"
#include "G4PhononDownconversion.hh"
#include "G4PhononLong.hh"
#include "G4PhononReflection.hh"
#include "G4PhononScattering.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"


// Create phonon and charage carrier particles for later use

void G4CMPPhysics::ConstructParticle() {
  G4CMPDriftElectron::Definition();
  G4CMPDriftHole::Definition();
  G4PhononLong::Definition();
  G4PhononTransFast::Definition();
  G4PhononTransSlow::Definition();
}

// Add physics processes to appropriate particles

void G4CMPPhysics::ConstructProcess() {
  // Only make processes once; will be deleted when physics list goes away
  G4VProcess* phScat = new G4PhononScattering;
  G4VProcess* phRefl = new G4PhononReflection;
  G4VProcess* phDown = new G4PhononDownconversion;
  G4VProcess* tmStep = new G4CMPTimeStepper;
  G4VProcess* driftB = new G4CMPDriftBoundaryProcess;
  G4VProcess* ivScat = new G4CMPInterValleyScattering;
  G4VProcess* eLuke  = new G4CMPeLukeScattering(tmStep);
  G4VProcess* hLuke  = new G4CMPhLukeScattering(tmStep);

  // Set process verbosity to match physics list, for diagnostics
  phScat->SetVerboseLevel(verboseLevel);
  phRefl->SetVerboseLevel(verboseLevel);
  phDown->SetVerboseLevel(verboseLevel);
  tmStep->SetVerboseLevel(verboseLevel);
  driftB->SetVerboseLevel(verboseLevel);
  ivScat->SetVerboseLevel(verboseLevel);
  eLuke->SetVerboseLevel(verboseLevel);
  hLuke->SetVerboseLevel(verboseLevel);

  G4ParticleDefinition* particle = 0;	// Reusable buffer for convenience

  // Add processes only to locally known particles
  particle = G4PhononLong::PhononDefinition();
  RegisterProcess(phScat, particle);
  RegisterProcess(phDown, particle);
  RegisterProcess(phRefl, particle);

  particle = G4PhononTransSlow::PhononDefinition();
  RegisterProcess(phScat, particle);
  RegisterProcess(phDown, particle);
  RegisterProcess(phRefl, particle);

  particle = G4PhononTransFast::PhononDefinition();
  RegisterProcess(phScat, particle);
  RegisterProcess(phDown, particle);
  RegisterProcess(phRefl, particle);

  particle = G4CMPDriftElectron::Definition();
  RegisterProcess(tmStep, particle);
  RegisterProcess(eLuke, particle);
  RegisterProcess(ivScat, particle);
  RegisterProcess(driftB, particle);

  particle = G4CMPDriftHole::Definition();
  RegisterProcess(tmStep, particle);
  RegisterProcess(hLuke, particle);
  RegisterProcess(driftB, particle);
}
