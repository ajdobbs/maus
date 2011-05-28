// MAUS WARNING: THIS IS LEGACY CODE.
// @(#) $Id: BTSheet.hh,v 1.3 2006-02-09 17:56:32 rogers Exp $  $Name:  $
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
// BTSheet.hh
//
// Created: V.Daniel Elvira (4/19/00)
//
//
//   The BTSheet Class inherits from G4MagneticField. The class objects are
//   field maps generated by an infinitelly thin solenoidal current sheets.
//   The class data members are all the parameters necessary to generate
//   analytically a magnetic field in r,z space (there is phi symmetry).
//   No geometric volumes or materials are associated with BTSheets.

#ifndef BTSHEET_HH
#define BTSHEET_HH

#include "CLHEP/Vector/ThreeVector.h"
#include "BTField.hh"

class BTSheet: public BTField {

  // Data members

  int myid;  // Sheet id (not used for now)
  int mytype; // Sheet type (only one type is supported)
  double mythick, myrad, mylen, mycur; // Sheet thickness (infinitelly thin
                                         // for now, z-length,
                                         // current in A/mm**2)
  Hep3Vector mylocation; // Sheet position in the same reference frame as
                            // Point[4] in GetFieldValue.

public:

  // Constructor

  BTSheet(Hep3Vector location, int id, int type, double thick,
	  double rad, double len, double cur ) :
    myid(id), mytype(type), mythick(thick), myrad(rad), mylen(len),
    mycur(cur), mylocation(location)
  { ; }
	BTSheet(const BTSheet& rhs)
	{ *this = rhs; }
	BTSheet& operator=(const BTSheet & rhs);


  ~BTSheet() { ; }

	// Accessor methods
	BTSheet * Clone()  const {return new BTSheet(*this);}


  int id() const { return myid;}
  int type() const{ return mytype;}
  double thick() const { return mythick;}
  double rad() const{ return myrad;}
  double len() const { return mylen;}
  double cur() const { return mycur;}
  Hep3Vector location() const { return mylocation; }

  // Concrete method inherited from BTField. It
  // returns the field value at Point[4].

  void GetFieldValue( const  double Point[4], double *Bfield ) const;

	void Print(std::ostream& out);

  // Destructor


};  // BTSheet

#endif  // BTSHEET_HH
