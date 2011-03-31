/** @class  MAUSEventAction
 *  Geant4 calls this class before and after each event
 *
 *  MICEEventAction controls what happens at the beginning
 *  and end of every event. GEANT4 calls BeginOfEventAction
 *  and EndOfEventAction at the beginning and end of the event,
 *  respectively.  At present, this class mainly just stores
 *  the primary track per event.
 *
 */

#ifndef _COMPONENTS_MAP_MAUSEVENTACTION_H_
#define _COMPONENTS_MAP_MAUSEVENTACTION_H_

#include <json/json.h>
#include "MAUSSteppingAction.h"
#include <G4Event.hh>
#include <G4UserEventAction.hh>  // inherit from
#include <G4TrajectoryContainer.hh>
#include <G4VTrajectory.hh>
#include <G4VTrajectoryPoint.hh>
#include <string>
#include <cmath>

namespace MAUS {

class MAUSEventAction : public G4UserEventAction
{
 public:
  void BeginOfEventAction(const G4Event* currentEvent);
  void EndOfEventAction(const G4Event* currentEvent) {}
  
};

}  //  ends MAUS namespace

#endif  //  _COMPONENTS_MAP_MAUSEVENTACTION_H_

