// @(#) $Id: BeamParameters.cc,v 1.10 2010-03-10 08:14:10 rogers Exp $  $Name:  $
//
//  MICE beam parameters
//
//  Yagmur Torun 

#include "BeamParameters.hh"
#include "Interface/dataCards.hh"

BeamParameters::BeamParameters() {
  miceMemory.addNew( Memory::BeamParameters );
}


BeamParameters* BeamParameters::fInstance = (BeamParameters*) NULL;

BeamParameters* BeamParameters::getInstance()
{

  if(!fInstance) fInstance = new BeamParameters;
  return fInstance;

}
