/*
** Modified example 1 from the GEANT4 distribution to simulate the
** MICE scintillating fiber tracker for the MICE proposal
** Ed McKigney - August 21, 2002
*/
#include "SciFiSD.hh"

#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "Interface/dataCards.hh"
#include <fstream>

#include "Interface/MICEEvent.hh"
#include "Config/MiceModule.hh"

SciFiSD::SciFiSD( MICEEvent* event, MiceModule* mod, bool dEdXcut ) : G4VSensitiveDetector( mod->fullName() )
{
  miceMemory.addNew( Memory::SciFiSD ); 
	
  _event = event;
  _module = mod;
  _dEdXcut = dEdXcut;
}

SciFiSD::~SciFiSD()
{
  miceMemory.addDelete( Memory::SciFiSD ); 
}

void SciFiSD::Initialize(G4HCofThisEvent* HCE)
{
  if( HCE ) ;
}

G4bool SciFiSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  if( edep == 0. && _dEdXcut ) return false;
  else if( edep == 0. ) edep = 1.0 * MeV;	// fake energy deposit just so that we get some photons digitised out of this hit!

  SciFiHit* newHit = new SciFiHit();

  newHit->SetTrackID( aStep->GetTrack()->GetTrackID() );

  // determine the fibre number based on the position in the tracker and the
  // MiceModule information about the fibre orientation and numbering scheme

  Hep3Vector pos = _module->globalPosition();
  Hep3Vector perp( -1., 0., 0. );

  perp *= _module->globalRotation();

  Hep3Vector delta = aStep->GetPostStepPoint()->GetPosition() - pos;

  double dist = delta.x() * perp.x() + delta.y() * perp.y() + delta.z() * perp.z();

  double fibre = _module->propertyDouble( "CentralFibre" ) + dist * 2.0 / ( _module->propertyDouble( "Pitch" ) * 7.0 );

  newHit->SetFiberNo( (int) ( fibre + 0.5 ) );

  newHit->SetVolumeName( aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() );

  newHit->SetTrackerNo( _module->propertyInt( "Tracker" ) );
  newHit->SetStationNo( _module->propertyInt( "Station" ) );
  newHit->SetPlaneNo( _module->propertyInt( "Plane" ) );

  newHit->SetEdep( edep );
  newHit->SetPos( aStep->GetPostStepPoint()->GetPosition() );
  newHit->SetMom( aStep->GetTrack()->GetMomentum() );
  newHit->SetTime( aStep->GetPostStepPoint()->GetGlobalTime() );
  newHit->SetEnergy( aStep->GetTrack()->GetTotalEnergy() );
  newHit->SetPID( aStep->GetTrack()->GetDefinition()->GetPDGEncoding() );
  newHit->SetCharge( aStep->GetTrack()->GetDefinition()->GetPDGCharge() );
  newHit->SetMass( aStep->GetTrack()->GetDefinition()->GetPDGMass() );

  _event->sciFiHits.push_back( newHit );
  
  return true;
}

void SciFiSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  // loop over hits and merge together hits that are on the same fibre

  std::vector<SciFiHit*> hits;

  for( unsigned int i = 0; i < _event->sciFiHits.size(); ++i )
  {
    SciFiHit* found = NULL;

    for( unsigned int j = 0; j < hits.size(); ++j )
      if( hits[j]->GetFiberNo() == _event->sciFiHits[i]->GetFiberNo() &&
          hits[j]->GetPlaneNo() == _event->sciFiHits[i]->GetPlaneNo() &&
          hits[j]->GetStationNo() == _event->sciFiHits[i]->GetStationNo() &&
          hits[j]->GetTrackerNo() == _event->sciFiHits[i]->GetTrackerNo() )
        found = hits[j];

   if( found )
   {
     found->mergeHit( _event->sciFiHits[i] );
     delete _event->sciFiHits[i];
   }
   else
     hits.push_back( _event->sciFiHits[i] );
  }

  _event->sciFiHits = hits;
}