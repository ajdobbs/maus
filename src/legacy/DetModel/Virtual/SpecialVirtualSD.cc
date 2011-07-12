// MAUS WARNING: THIS IS LEGACY CODE.
//-----------------------------------------------------------------------------
//
//     File :     SpecialVirtualSD.cc
//     Purpose :  MICE Special Virtual detector for the MICE proposal.
//                Sensitive detector definition.
//     Created :  12-Aug-2003  by Steve Kahn
//
//-----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "DetModel/Virtual/SpecialVirtualSD.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4Field.hh"
#include "G4ThreeVector.hh"
#include "G4VTouchable.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4VoxelLimits.hh"
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"

using namespace std;

int SpecialVirtualSD::_uniqueID=0;

SpecialVirtualSD::SpecialVirtualSD(MICEEvent * _event, MiceModule * mod )
                 : MAUSSD( mod ),
                   _steppingThrough(true), _steppingInto(true), _steppingOutOf(true), _steppingAcross(true),
                   _localRotation(), _globalRotation(), _localPosition(), _globalPosition()
{
  _module = mod;
  simEvent = _event;
  _tname = _module->fullName() + intToString(_uniqueID);
  if(_module->propertyExistsThis("ZSegmentation","int"))
    _numberCellsInZ = _module->propertyInt("ZSegmentation");
  else _numberCellsInZ = 1;
  if(_module->propertyExistsThis("PhiSegmentation","int"))
    _numberCellsInPhi = _module->propertyInt("PhiSegmentation");
  else _numberCellsInPhi = 1;
  if(_module->propertyExistsThis("RSegmentation","int"))
    _numberCellsInR = _module->propertyInt("RSegmentation");
  else _numberCellsInR = 1;
  if(_module->propertyExistsThis("SteppingThrough", "bool"))
    _steppingThrough = _module->propertyBool("SteppingThrough");
  if(_module->propertyExistsThis("SteppingInto", "bool"))
    _steppingInto    = _module->propertyBool("SteppingInto");
  if(_module->propertyExistsThis("SteppingOutOf", "bool"))
    _steppingOutOf   = _module->propertyBool("SteppingOutOf");
  if(_module->propertyExistsThis("SteppingAcross", "bool"))
    _steppingAcross  = _module->propertyBool("SteppingAcross");
  _uniqueID++;
  if(_module->propertyExistsThis("Station","int"))
    _stationNumber   = _module->propertyInt("Station");
  else 
    _stationNumber = _uniqueID; 

  if(_module->propertyExistsThis("LocalRefRotation", "hep3vector"))
  {
    Hep3Vector rot  = _module->propertyHep3Vector("LocalRefRotation");
    _globalRotation = _module->globalRotation();
    _localRotation  = HepRotation( HepRotationX(rot.x()) * HepRotationY(rot.y()) * HepRotationZ(rot.z()) );
  }
  else if(_module->propertyExistsThis("GlobalRefRotation", "hep3vector"))
  {
    Hep3Vector rot  = _module->propertyHep3Vector("GlobalRefRotation");
    _globalRotation = HepRotation( HepRotationX(rot.x()) * HepRotationY(rot.y()) * HepRotationZ(rot.z()) );
  }

  if(_module->propertyExistsThis("LocalRefPosition", "hep3vector"))
  {
    _localPosition   = _module->propertyHep3Vector("LocalRefPosition");
    _globalPosition  = _module->globalPosition();
  }
  if(_module->propertyExistsThis("GlobalRefPosition", "hep3vector"))
    _globalPosition  = _module->propertyHep3Vector("GlobalRefPosition");
}

void SpecialVirtualSD::SetStepping(bool SteppingThrough, bool SteppingInto, bool SteppingOutOf, bool SteppingAcross)
{
  _steppingThrough = SteppingThrough;
  _steppingInto    = SteppingInto;
  _steppingOutOf   = SteppingOutOf;
  _steppingAcross  = SteppingAcross;
}


void SpecialVirtualSD::Initialize(G4HCofThisEvent* HCE)
{
  //ME use variable

  if( HCE ) ;

}

G4bool SpecialVirtualSD::ProcessHits(G4Step* aStep, G4TouchableHistory*
             History)
{
  G4double edep = aStep->GetTotalEnergyDeposit();

  G4StepPoint* thePrePoint  = aStep->GetPreStepPoint();
  G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
  G4VPhysicalVolume* thePrePV  = thePrePoint->GetPhysicalVolume();
  G4String preName  = thePrePV->GetName();


  int istat = 0;
  G4StepStatus prestat  = thePrePoint->GetStepStatus();
  G4StepStatus poststat = thePostPoint->GetStepStatus();
  if (prestat  == fGeomBoundary) istat = 1;  // entering volume
  if (poststat == fGeomBoundary) istat = -1; // leaving
  // unless it's a single internal step across the volume
  if (prestat == fGeomBoundary && poststat == fGeomBoundary) istat = 2;

  // we want to accumulate all energy depoited and write hit on exit of volume

  //  const G4VTouchable* fondle = thePrePoint->GetTouchable();

  
  int copyIDinZ = 0;
  int copyIDinPhi = 0;
  int copyIDinR =0;

  if(_numberCellsInZ != 1 || _numberCellsInPhi != 1 ||_numberCellsInR != 1)
  {
    getCellIndeces(aStep);
    copyIDinR = _iCellR; // fondle->GetReplicaNumber(2);
    copyIDinZ = _iCellZ; //fondle->GetReplicaNumber(1);
    copyIDinPhi = _iCellPhi;  // fondle->GetReplicaNumber();
  }
  else
  {
    _iCellZ = 0;
    _iCellR = 0;
    _iCellPhi = 0;
  }

  //int indx = index(copyIDinZ, copyIDinPhi, copyIDinR);
  //  if(_cellID[indx] =  -1)
  Json::Value hit_i, channel_id;
  channel_id["type"] = "SpecialVirtual";
  hit_i["track_id"] = aStep->GetTrack()->GetTrackID();
  channel_id[ "station_number" ] = _stationNumber;
  hit_i["channel_id"] = channel_id;
  hit_i["energy_deposited"] = edep;
  
  hit_i["time"] = aStep->GetPostStepPoint()->GetGlobalTime();
  hit_i["energy"] = aStep->GetTrack()->GetTotalEnergy();
  hit_i["pid"] = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  hit_i["charge"] =  aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  hit_i["mass"] = aStep->GetTrack()->GetDefinition()->GetPDGMass();

  _hits.push_back(hit_i);
  return true;
}

int  SpecialVirtualSD::index(int iz, int iphi, int ir)
{   int indx = iz + _numberCellsInZ*(iphi + _numberCellsInPhi*ir);
    return indx;
}

void SpecialVirtualSD::clear()  {}

void SpecialVirtualSD::DrawAll() {}

void SpecialVirtualSD::PrintAll() {}

void SpecialVirtualSD::EndOfEvent(G4HCofThisEvent* HCE)
{
  //ME use variable

  if( HCE ) ;
}

string SpecialVirtualSD::getTypeName() const {   return _tname; }

int SpecialVirtualSD::idFromName()
{
  G4CollectionNameVector::iterator it = collectionName.begin();
  int idord = 0;

  string vname = _tname + "collection";

  while( it != collectionName.end())
    {
      string cname = (string) *it;
      if( cname == vname) return idord;
      it++;
      idord++;
    }
  return -1;
}

void SpecialVirtualSD::getCellIndeces(G4Step* aStep)
{
  G4StepPoint* point = aStep->GetPreStepPoint();
  const  G4VTouchable* touch = point->GetTouchable();
  G4ThreeVector transl = touch->GetTranslation();
  const G4RotationMatrix* rotm = touch->GetRotation();
  G4ThreeVector gpos = point->GetPosition();  // global position
  G4AffineTransform at(rotm, transl);
  G4AffineTransform ati = at.Inverse();
  G4ThreeVector lpos = ati.TransformPoint(gpos);
  if(verboseLevel > 1)
    {
      cout << "Global Point : " << gpos << ", Local Point : " << lpos << endl;
    }

  G4VPhysicalVolume* physivol = point->GetPhysicalVolume();
  G4LogicalVolume* logic = physivol->GetLogicalVolume();
  G4Tubs* solid = dynamic_cast<G4Tubs*>(logic->GetSolid());
  double zmin, zmax, rmin, rmax, phimin, phimax;
  G4VoxelLimits vl;
  G4ThreeVector zero(0., 0., 0.);
  G4AffineTransform unity(zero);
  solid->CalculateExtent(kZAxis, vl, unity, zmin, zmax);
  rmin = solid->GetInnerRadius();
  rmax = solid->GetOuterRadius();
  phimin = solid->GetStartPhiAngle();
  phimax = phimin + solid->GetDeltaPhiAngle();
  double phi = lpos.phi();
  if(phi < 0) phi += twopi;
  _iCellZ = (int) (_numberCellsInZ * (lpos.z() - zmin)/(zmax - zmin));
  _iCellR = (int) (_numberCellsInR * (lpos.perp() - rmin)/(rmax - rmin));
  _iCellPhi = (int)(_numberCellsInPhi * (phi - phimin)/(phimax-phimin));
  if(verboseLevel > 1)
    {
      cout << "Extents : " << zmin << " " << zmax << " " << rmin
     << " " << rmax << " " << phimin << " " << phimax << endl;
      cout << "Cell Indeces : " << _iCellZ << " " << _iCellPhi << " "
     << _iCellR << endl;
    }
}

std::string SpecialVirtualSD::intToString(int number)
{
  std::ostringstream aStream;
  aStream << '_' << number << std::flush;
  return(aStream.str());
}


