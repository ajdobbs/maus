// MAUS WARNING: THIS IS LEGACY CODE.
//This file is a part of MAUS
//
//MAUS is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//MAUS is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with MAUS in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

#ifndef _SRC_COMMON_CPP_MATHS_SYMMETRIC_MATRIX_HH_
#define _SRC_COMMON_CPP_MATHS_SYMMETRIC_MATRIX_HH_

#include <iostream>
#include <vector>

#include "Interface/Complex.hh"
#include "Interface/Squeal.hh"
#include "Interface/Vector.hh"
#include "Interface/Matrix.hh"

namespace CLHEP
{
  class HepSymMatrix;
}

namespace MAUS
{

//*************************
// Forward Declarations
//*************************
class Matrix<double>;
class SymmetricMatrix;
class Vector<double>;


//*************************
// Conversion Functions
//*************************

/** @brief returns the inverse of a matrix
 */
SymmetricMatrix inverse(const SymmetricMatrix& matrix);

namespace CLHEP
{
/** @brief Creates a CLHEP::HepMatrix from a MAUS::Matrix. This function is
 *         meant to look like a copy constructor for the HepMatrix class, but
 *         it is a free function in the namespace MAUS::CLHEP.
 *
 */
::CLHEP::HepSymMatrix HepSymMatrix(const SymmetricMatrix& matrix);
}

//*************************
// Eigensystem Functions
//*************************

/** @brief returns a vector of eigenvalues. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
Vector<double> eigenvalues(const SymmetricMatrix& matrix);

/** @brief returns a vector of std::pair containing a vector of eigenvalues
 *         and a matrix of eigenvectors. Throws an exception if either this
 *         matrix is not square or the eigenvalues could not be found (e.g.
 *         singular matrix or whatever).
 */
std::pair<Vector<double>, Matrix<double> > eigensystem(
  const SymmetricMatrix& matrix);

//*************************
// Unitary Operators
//*************************

MAUS::SymmetricMatrix operator-(const SymmetricMatrix& matrix);

//*************************
// Scalar Operators
//*************************

MAUS::SymmetricMatrix  operator*(
  const double&					 value,
  const SymmetricMatrix& matrix);


/** @class SymmetricMatrix extends Matrix by enforcing symmetric arrangements of
 *				 elements. All elements are stored despite the redundent nature of
 *				 symmetric matrices. This is so that base class functions will work
 *				 properly. They will just return an ordinary Matrix object and be
 *				 uncastable as a SymmetricMatrix.
 */
class SymmetricMatrix : public Matrix<double>
{
public:
  /** @brief default constructor makes an empty SymmetricMatrix of size 0
   */
	SymmetricMatrix();

  /** @brief Copy constructor
   */
	SymmetricMatrix(const SymmetricMatrix& original_instance);

  /** @brief Copy constructor for CLHEP::HepSymMatrix
   */
	SymmetricMatrix(const ::CLHEP::HepSymMatrix& hep_matrix );

  /** @brief Construct a matrix and fill all fields with 0
   *
   *  @params size number of rows/columns
   */
	SymmetricMatrix(const size_t size);

  /** @brief Construct a matrix and fill with identical data
   *
   *  @params size number of rows/columns
   *  @params data variable to be copied into all items in the matrix
   */
	SymmetricMatrix(const size_t size, const double& value);

  /** @brief Construct a matrix and fill with data from an array
   *
   *  @params size number of rows/columns
   *  @params data pointer to the start of a memory block of size
   *          size^2 with data laid out <row> <row> <row>. Only the lower
	 *					triangle of data is used to easily ensure element symmetry.
	 *					Note SymmetricMatrix does not take ownership of memory in data.
   */
	SymmetricMatrix(const size_t size, double const * const data);

  //*************************
  // Size Functions
  //*************************

  /** @brief returns number of rows/columns in the matrix
   */
  const size_t size() const;
  
  //*************************
  // Assignment Operators
  //*************************
  SymmetricMatrix& operator =(
		const SymmetricMatrix&									rhs);
  SymmetricMatrix& operator+=(
    const SymmetricMatrix&									rhs);
  SymmetricMatrix& operator-=(
    const SymmetricMatrix&									rhs);
  SymmetricMatrix& operator*=(const double& rhs);
  SymmetricMatrix& operator/=(const double& rhs);
  
  //*************************
  // Algebraic Operators
  //*************************
  const SymmetricMatrix operator+(
    const SymmetricMatrix&											rhs) const;
  const SymmetricMatrix operator-(
    const SymmetricMatrix&											rhs) const;
  const SymmetricMatrix operator*(const double& rhs) const;
  const SymmetricMatrix operator/(const double& rhs) const;

  //*************************
  // Befriending
  //*************************

  //These use their Matrix counterparts and then use the protected base class
	//copy constructor to cast the return type as a SymmetricMatrix
  friend SymmetricMatrix inverse(
		const SymmetricMatrix& matrix);
	friend SymmetricMatrix MAUS::operator-(
		const SymmetricMatrix& matrix);
	friend SymmetricMatrix MAUS::operator*(
		const  double&						value,
		const SymmetricMatrix&	matrix);

	//These use special low-level gsl functions for symmetric matricies
  friend Vector<double> eigenvalues(const SymmetricMatrix& matrix);
  friend std::pair<Vector<double>, Matrix<double> >
  eigensystem(const SymmetricMatrix& matrix);

protected:
	
  /** @brief Base class copy constructor
   */
	SymmetricMatrix(const Matrix<double>& original_instance);

  //build the matrix with size^2 elements initialised to zero by default
  void build_matrix(const size_t size, const bool initialize=true);  

  //build the matrix with size^2 elements initialised to array data in
  //row major order
  void build_matrix(const size_t size,  double const * const data);

};

} //end namespace MAUS
        
#endif
