// @(#) $Id: BTSheet.cc,v 1.5 2008-03-20 17:04:27 torun Exp $  $Name:  $
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of     *
// * FERMILAB.                                                        *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// BTSheet.cc
//
// Created: V.Daniel Elvira (4/19/00)
// Modified: Y. Torun (8/22/02) - Use GSL interface instead of ZOOM
//
//
//   The BTSheet Class inherits from G4MagneticField. The class objects are
//   field maps generated by an infinitelly thin solenoidal current sheets.
//   The class data members are all the parameters necessary to generate
//   analytically a magnetic field in r,z space (there is phi symmetry).
//   No geometric volumes or materials are associated with BTSheets.

#include "BTSheet.hh"
// Use GSL special functions to evaluate the current sheet field analytically
#include "micegsl.hh"
#include "CLHEP/Units/PhysicalConstants.h"

// For a current Sheet, Bfield[0], Bfield[1], Bfield[2] are
// the r,phi,z components of the magnetic field. For BTSolenoid
// and the global fields, they are the cartesian coordinates which need
// to be fed to GEANT4. Bfield[1] is zero because there is phi symmetry.

// Get method to retrieve the field at Point[4] generated by a single
// current sheet.

using CLHEP::m;
using CLHEP::pi;

void BTSheet::GetFieldValue( const  double Point[4],
			   double *Bfield ) const
{

  double k;

  double rposition = Point[0]/m; // transform from mm to meters
  double zposition = Point[2]/m;

  double rad = myrad/m;
  double len = mylen/m;
  double cur = mycur;

  //  Left-end of sheet

  double rho = rposition;
  double z = location().z()/m - zposition;

  double r1 = sqrt( pow(rad + rho,2) + z*z );
  k = 2.0 / r1 * sqrt( rad * rho );
  double c2 = -4.0 * rad*rho / pow(rad+rho,2);

  double f1 = z * rad / ((rad + rho) * r1);
  double f2 = (rad - rho) / (2.0 * rad);

  double elle = GSL::SpecialFunctions::EllipticIntegrals::IncompleteE(pi/2.0,k);
  double ellf = GSL::SpecialFunctions::EllipticIntegrals::IncompleteF(pi/2.0,k);
  double ellpi = GSL::SpecialFunctions::EllipticIntegrals::IncompleteP(pi/2.0,k,c2);

  double bz1 = f1 * ( ellf + f2 * ( ellpi - ellf ) );

  double br1;

  if( rho != 0.0 ) {
    f1 = r1 / (4.*rho);
    br1 = f1 * ( 2. * ( ellf - elle ) - k * k * ellf );
  }
  else {
    br1 = 0.0;
  }

  double bz = -bz1;
  double br = -br1;

  //  Right-end of sheet

  z = z + len;
  r1 = sqrt( pow(rad + rho,2) + z*z );
  k = 2.0 / r1 * sqrt( rad * rho );
  c2 = -4.0 * rad * rho / pow(rad + rho,2);

  f1 = z * rad / ( (rad + rho) * r1);
  f2 = (rad - rho) / (2.0 * rad);

  elle = GSL::SpecialFunctions::EllipticIntegrals::IncompleteE(pi/2.0,k);
  ellf = GSL::SpecialFunctions::EllipticIntegrals::IncompleteF(pi/2.0,k);
  ellpi = GSL::SpecialFunctions::EllipticIntegrals::IncompleteP(pi/2.0,k,c2);

  bz1 = f1 * ( ellf + f2 * ( ellpi - ellf ) );

  if ( rho != 0.0 ) {
    f1 = r1 / (4.0 * rho);
    br1 = f1 * ( 2.0 * ( ellf - elle ) - k * k * ellf );
  }
  else {
    br1 = 0.0;
  }

  bz = bz + bz1;       // note change of sign
  br = br + br1;

  double permfactor = 4.0e-7;

  Bfield[0] = permfactor * cur * br; // Remember this is the r component
  Bfield[1] = 0.0;                   // This is the phi component (zero)
  Bfield[2] = permfactor * cur * bz; // This is the z component

 }

void BTSheet::Print(std::ostream & out)
{
	out << "Sheet Length: " << mylen << " Radius: " << myrad << " Current: " << mycur;
}

BTSheet& BTSheet::operator=(const BTSheet & rhs)
{
	if(&rhs==this) return *this;
	myid = rhs.myid;
	mytype = rhs.mytype;
	mythick = rhs.mythick;
	myrad = rhs.myrad;
	mylen = rhs.mylen;
	mycur = rhs.mycur;
	mylocation = rhs.mylocation;
	return *this;

}

