/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

// $Id$
// File: G4CMPGeometryUtils.cc
//
// Description: Free standing helper functions for geometry based calculations.
//
// 20161107  Rob Agnese
// 20170605  Pass touchable from track, not just local PV
// 20170815  Move AdjustSecondaryPosition to here as ApplySurfaceClearance

#include "G4CMPGeometryUtils.hh"
#include "G4CMPConfigManager.hh"
#include "G4CMPGlobalLocalTransformStore.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"


G4ThreeVector G4CMP::GetLocalDirection(const G4VTouchable* touch,
                                       const G4ThreeVector& dir) {
  return G4CMPGlobalLocalTransformStore::ToLocal(touch).TransformAxis(dir);
}

G4ThreeVector G4CMP::GetLocalPosition(const G4VTouchable* touch,
                                      const G4ThreeVector& pos) {
  return G4CMPGlobalLocalTransformStore::ToLocal(touch).TransformPoint(pos);
}

G4ThreeVector G4CMP::GetGlobalDirection(const G4VTouchable* touch,
                                        const G4ThreeVector& dir) {
  return G4CMPGlobalLocalTransformStore::ToGlobal(touch).TransformAxis(dir);
}

G4ThreeVector G4CMP::GetGlobalPosition(const G4VTouchable* touch,
                                       const G4ThreeVector& pos) {
  return G4CMPGlobalLocalTransformStore::ToGlobal(touch).TransformPoint(pos);
}

void G4CMP::RotateToLocalDirection(const G4VTouchable* touch,
                                   G4ThreeVector& dir) {
  G4CMPGlobalLocalTransformStore::ToLocal(touch).ApplyAxisTransform(dir);
}

void G4CMP::RotateToLocalPosition(const G4VTouchable* touch,
                                  G4ThreeVector& pos) {
  G4CMPGlobalLocalTransformStore::ToLocal(touch).ApplyPointTransform(pos);
}

void G4CMP::RotateToGlobalDirection(const G4VTouchable* touch,
                                    G4ThreeVector& dir) {
  G4CMPGlobalLocalTransformStore::ToGlobal(touch).ApplyAxisTransform(dir);
}

void G4CMP::RotateToGlobalPosition(const G4VTouchable* touch,
                                   G4ThreeVector& pos) {
  G4CMPGlobalLocalTransformStore::ToGlobal(touch).ApplyPointTransform(pos);
}


// Get normal to enclosing volume at boundary point

G4ThreeVector G4CMP::GetSurfaceNormal(const G4Step& step) {
  // Get outward normal using G4Navigator method (more reliable than G4VSolid)
  G4TransportationManager* transMan =
    G4TransportationManager::GetTransportationManager();
  G4Navigator* nav = transMan->GetNavigatorForTracking();

  G4bool goodNorm;
  G4ThreeVector surfNorm = nav->GetGlobalExitNormal(
                                      step.GetPostStepPoint()->GetPosition(),
                                      &goodNorm);

  // FIXME:  Sometimes G4Navigator fails, but still returns "good"
  if (!goodNorm || surfNorm.mag()<0.99) {
    G4VPhysicalVolume* thePrePV = step.GetPreStepPoint()->GetPhysicalVolume();
    G4VPhysicalVolume* thePostPV = step.GetPostStepPoint()->GetPhysicalVolume();
    G4Exception("G4CMPGeometryUtils::GetSurfaceNormal", "Geometry001",
                EventMustBeAborted, ("Can't get normal vector of surface between " +
                                    thePrePV->GetName() + " and " +
                                    thePostPV->GetName()+ ".").c_str());
  }

  return surfNorm;
}


// Get placement volume at specified global position

G4VPhysicalVolume* G4CMP::GetVolumeAtPoint(const G4ThreeVector& pos) {
  G4TransportationManager* transMan =
    G4TransportationManager::GetTransportationManager();
  G4Navigator* nav = transMan->GetNavigatorForTracking();
  G4VPhysicalVolume* volume = nav->LocateGlobalPointAndSetup(pos,0,false);

  return volume;
}


// Adjust specified position to avoid surface of current (touchable) volume

G4ThreeVector G4CMP::ApplySurfaceClearance(const G4VTouchable* touch,
					   G4ThreeVector pos) {
  // Clearance is the minimum distance where a position is guaranteed Inside
  const G4double clearance = G4CMPConfigManager::GetSurfaceClearance();

  // If the step is near a boundary, create the secondary in the initial volume
  G4VPhysicalVolume* pv = touch->GetVolume();
  G4ThreadLocalStatic auto latMan = G4LatticeManager::GetLatticeManager();
  G4LatticePhysical* lat = latMan->GetLattice(pv);

  if (!lat) {		// No lattice in touchable's volume, try pos instead
    pv = G4CMP::GetVolumeAtPoint(pos);
    lat = latMan->GetLattice(pv);

    if (!lat) {
      G4ExceptionDescription msg;
      msg << "Position " << pos << " not associated with valid volume.";
      G4Exception("G4CMP::CreateSecondary", "Secondary008",
		  EventMustBeAborted, msg);
      return pos;
    }
  }

  // Work in local coordinates, adjusting position to be clear of surface
  RotateToLocalPosition(touch, pos);

  G4VSolid* solid = pv->GetLogicalVolume()->GetSolid();
  G4ThreeVector norm = solid->SurfaceNormal(pos);

  while (solid->Inside(pos) != kInside ||
	 solid->DistanceToOut(pos,norm) < clearance) {
    pos -= norm*clearance;
    norm = solid->SurfaceNormal(pos);	// Nearest surface may change
  }

  RotateToGlobalPosition(touch, pos);
  return pos;
}
