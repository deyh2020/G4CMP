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
//
// $Id$
//
#ifndef XVCrystalIntegratedDensity_h
#define XVCrystalIntegratedDensity_h

#include "XVCrystalCharacteristic.hh"

class XVCrystalIntegratedDensity {

public:
    void SetIntegrationPoints(unsigned int,unsigned int);
    unsigned int GetIntegrationPoints(unsigned int);
    unsigned int GetIntegrationPoints();

    void SetNumberOfPoints(unsigned int);
    unsigned int GetNumberOfPoints();

    void SetDensity(XVCrystalCharacteristic*);
    XVCrystalCharacteristic* GetDensity();
    
    void SetPotential(XVCrystalCharacteristic*);
    XVCrystalCharacteristic* GetPotential();

    void SetXPhysicalLattice(XPhysicalLattice*);
    XPhysicalLattice* GetXPhysicalLattice();

    void PrintOnFile(char*,XPhysicalLattice*);
    G4bool HasBeenInitialized(XPhysicalLattice*);

    G4double GetValue(G4double,XPhysicalLattice*);

protected:
    G4double GetStep();

public:
    virtual void InitializeTable();
    
protected:
    virtual G4double ComputeValue(G4double) = 0;
    G4double FindCatmullRomInterpolate(G4double &p0, G4double &p1, G4double &p2, G4double &p3, G4double &x);

protected:
    XPhysicalLattice* fLattice;
    XVCrystalCharacteristic* fDensity;
    XVCrystalCharacteristic* fPotential;

    G4double fPotentialMinimum;
    G4double fPotentialRange;
    
    std::vector<G4double> fTable;
    unsigned int fNumberOfPoints;
    unsigned int fIntegrationPoints[3];
    

public:   //Contructors
    XVCrystalIntegratedDensity();
    ~XVCrystalIntegratedDensity();
};

#endif
