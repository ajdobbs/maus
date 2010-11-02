/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#ifndef EMRSD_h
#define EMRSD_h 1

#include "G4VSensitiveDetector.hh"
#include "Interface/EMRHit.hh"

#include "Interface/Memory.hh" 

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* EMRSD is the sensitive detector for GEANT4 that handles the 
* EMR calorimeter simulation.
**/

class EMRSD : public G4VSensitiveDetector
{
  public:

      //! Constructor
      EMRSD(  MICEEvent*, MiceModule* );

      //! Default destructor
     ~EMRSD();

      //! Initialise this this instance with the G4 event information
      void Initialize( G4HCofThisEvent* );

      //! process all hits in this detector
      G4bool ProcessHits( G4Step*, G4TouchableHistory* );

      //! code that is called at the end of processing of an event
      void EndOfEvent( G4HCofThisEvent* );

  private:

        MICEEvent*      _event;

        MiceModule*     _module;
};

#endif
