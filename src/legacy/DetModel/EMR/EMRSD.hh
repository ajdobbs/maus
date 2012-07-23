// MAUS WARNING: THIS IS LEGACY CODE.
#ifndef EMRSD_h
#define EMRSD_h 1
// MAUS
#include "DetModel/MAUSSD.hh"
// Geant 4
#include "Geant4/G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;
class MICEEvent;
class MiceModule;

/**
* EMRSD is the sensitive detector for GEANT4 that handles the 
* EMR calorimeter simulation.
**/

class EMRSD : public MAUS::MAUSSD {
  public:

      //! Constructor
      EMRSD( MiceModule*);

      //! Default destructor
     ~EMRSD();

      //! Initialise this this instance with the G4 event information
      void Initialize( G4HCofThisEvent* );

      //! process all hits in this detector
      G4bool ProcessHits( G4Step*, G4TouchableHistory* );

      //! code that is called at the end of processing of an event
      void EndOfEvent( G4HCofThisEvent* );

};

#endif
