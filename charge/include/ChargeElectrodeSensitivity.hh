#ifndef ChargeElectrodeSensitivity_h
#define ChargeElectrodeSensitivity_h 1

#include "G4CMPElectrodeSensitivity.hh"
#include "G4CMPElectrodeHit.hh"

class G4HCofThisEvent;
class ChargeFETDigitizerModule;

using std::ofstream;

class ChargeElectrodeSensitivity
    : public G4CMPElectrodeSensitivity
{
public:
  ChargeElectrodeSensitivity(G4String);
  virtual ~ChargeElectrodeSensitivity();
  virtual void Initialize(G4HCofThisEvent*);
  
  virtual void EndOfEvent(G4HCofThisEvent*);
  
private:
  ofstream output;
  ChargeFETDigitizerModule* FET;
};

#endif