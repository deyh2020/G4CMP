#include "ChargeFETDigitizerMessenger.hh"
#include "ChargeFETDigitizerModule.hh"
#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

ChargeFETDigitizerMessenger::ChargeFETDigitizerMessenger(
                          ChargeFETDigitizerModule* digitizer) : fet(digitizer)
{    
    fetDir = new G4UIdirectory("/g4cmp/FETSim/");
    fetDir->SetGuidance("FETSim commands");

    SetOutputFileCmd = new G4UIcmdWithAString("/g4cmp/FETSim/SetOutputFile",this);
    SetOutputFileCmd->SetGuidance("Set path to FET output file.");

    GetOutputFileCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetOutputFile",this);
    GetOutputFileCmd->SetGuidance("Current path to FET output file.");

    SetTemplateFileCmd = new G4UIcmdWithAString("/g4cmp/FETSim/SetTemplateFile",this);
    SetTemplateFileCmd->SetGuidance("Set path to FET template file.");

    GetTemplateFileCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetTemplateFile",this);
    GetTemplateFileCmd->SetGuidance("Current path to FET template file.");

    SetTemplateFileCmd = new G4UIcmdWithAString("/g4cmp/FETSim/SetRamoFileDir",this);
    SetTemplateFileCmd->SetGuidance("Set path to Ramo potential files.");

    GetTemplateFileCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetRamoFileDir",this);
    GetTemplateFileCmd->SetGuidance("Current path to Ramo potential files.");

    SetNumChanCmd = new G4UIcmdWithAnInteger("/g4cmp/FETSim/SetNumberOfChannels",this);
    SetNumChanCmd->SetGuidance("Number of FET channels in detector needs to match number of Ramo potential files.");

    GetNumChanCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetNumberOfChannels",this);
    GetNumChanCmd->SetGuidance("Number of FET channels");

    SetTimeBinCmd = new G4UIcmdWithAnInteger("/g4cmp/FETSim/SetNumberOfBins",this);
    SetTimeBinCmd->SetGuidance("Set number of digitizer bins");

    GetTimeBinCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetNumberOfBins",this);
    GetTimeBinCmd->SetGuidance("Number of digitizer bins");

    SetDecayTimeCmd = new G4UIcmdWithADoubleAndUnit("/g4cmp/FETSim/SetDecayTime",this);
    SetDecayTimeCmd->SetGuidance("Pulse decay time (if not using templates)");

    GetDecayTimeCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetDecayTime",this);
    GetDecayTimeCmd->SetGuidance("Get pulse decay time (if not using templates)");

    SetUnitTimeCmd = new G4UIcmdWithADoubleAndUnit("/g4cmp/FETSim/SetUnitTime",this);
    SetUnitTimeCmd->SetGuidance("Set dt for pulse bins (if not using templates)");

    GetUnitTimeCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetUnitTime",this);
    GetUnitTimeCmd->SetGuidance("Get dt for pulse bins (if not using templates)");

    SetPreTrigCmd = new G4UIcmdWithADoubleAndUnit("/g4cmp/FETSim/SetPreTriggerTime",this);
    SetPreTrigCmd->SetGuidance("Set pre-trigger time for pulse (if not using templates)");

    GetPreTrigCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetPreTriggerTime",this);
    GetPreTrigCmd->SetGuidance("Get pre-trigger time for pulse (if not using templates)");

    SetTemplateEnergyCmd = new G4UIcmdWithADoubleAndUnit("/g4cmp/FETSim/SetTemplateEnergy",this);
    SetTemplateEnergyCmd->SetGuidance("Set energy scaling for template pulses");

    GetTemplateEnergyCmd = new G4UIcmdWithoutParameter("/g4cmp/FETSim/GetTemplateEnergy",this);
    GetTemplateEnergyCmd->SetGuidance("Get energy scaling for templates pulses");
}

ChargeFETDigitizerMessenger::~ChargeFETDigitizerMessenger()
{
    delete fetDir;
    delete SetOutputFileCmd;
    delete GetOutputFileCmd;
    delete SetTemplateFileCmd;
    delete GetTemplateFileCmd;
    delete SetNumChanCmd;
    delete GetNumChanCmd;
    delete SetTimeBinCmd;
    delete GetTimeBinCmd;
    delete SetDecayTimeCmd;
    delete GetDecayTimeCmd;
    delete SetUnitTimeCmd;
    delete GetUnitTimeCmd;
    delete SetPreTrigCmd;
    delete GetPreTrigCmd;
    delete SetTemplateEnergyCmd;
    delete GetTemplateEnergyCmd;
}

void ChargeFETDigitizerMessenger::SetNewValue(G4UIcommand* command, G4String NewValue)
{
  if (command == SetOutputFileCmd)
    fet->SetOutputFilename(NewValue);
  else if (command == GetOutputFileCmd)
    fet->GetOutputFilename();
  else if (command == SetTemplateFileCmd)
    fet->SetTemplateFilename(NewValue);
  else if (command == GetTemplateFileCmd)
    fet->GetTemplateFilename();
  else if (command == SetRamoFileDirCmd)
    fet->SetRamoFileDir(NewValue);
  else if (command == GetRamoFileCmd)
    fet->GetRamoFileDir();
  else if (command == SetNumChanCmd)
    fet->SetNumberOfChannels(SetNumChanCmd->ConvertToInt(NewValue));
  else if (command == GetNumChanCmd)
    fet->GetNumberOfChannels();
  else if (command == SetTimeBinCmd)
    fet->SetTimeBins(SetTimeBinCmd->ConvertToDimensionedDouble(NewValue));
  else if (command == GetTimeBinCmd)
    fet->GetTimeBins();
  else if (command == SetDecayTimeCmd)
    fet->SetDecayTime(SetDecayTimeCmd->ConvertToDimensionedDouble(NewValue));
  else if (command == GetDecayTimeCmd)
    fet->GetDecayTime();
  else if (command == SetUnitTimeCmd)
    fet->SetUnitTime(SetUnitTimeCmd->ConvertToDimensionedDouble(NewValue));
  else if (command == GetUnitTimeCmd)
    fet->GetUnitTime();
  else if (command == SetPreTrigCmd)
    fet->SetPreTrig(SetPreTrigCmd->ConvertToDimensionedDouble(NewValue));
  else if (command == GetPreTrigCmd)
    fet->GetPreTrig();
  else if (command == SetTemplateEnergyCmd)
    fet->SetTemplateEnergy(SetTemplateEnergyCmd->ConvertToDimensionedDouble(NewValue));
  else if (command == GetTemplateEnergyCmd)
    fet->GetTemplateEnergy();
}