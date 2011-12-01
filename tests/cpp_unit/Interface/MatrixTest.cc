// Copyright 2010 Chris Rogers
//
//This file is a part of G4MICE
//
//G4MICE is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//G4MICE is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with G4MICE in the doc folder.  If not, see 
//<http://www.gnu.org/licenses/>.

//Issues:
// * No ASSERT_EQ(size_t, size_t) - gives compiler warning for overload size_t to int
// * 

#include <limits>
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Interface/Complex.hh"
#include "Interface/Matrix.hh"
#include "Interface/Vector.hh"

#include "gtest/gtest.h" 
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Random/Random.h"

using namespace MAUS;
namespace MAUS { using namespace Complex; }

//Defined in ComplexTest.cc
bool equal(const MAUS::complex c1, const MAUS::complex c2);
bool equal(double c1, double c2);

class MatrixTest : public ::testing::Test
{
public:
  MatrixTest()
  {
    for (size_t index=0; index<data_size_; ++index)
    {
      double_data_doubled_[index] = double_data_[index] * 2;
      complex_data_doubled_[index] = complex_data_[index] * 2.;
      complex_data_multiple_[index] = complex_data_[index]
                                    * complex_scaling_factor_;
    }
  }

protected:
  template <typename Tmplt>
  void elements_equal(Matrix<Tmplt>& matrix, Tmplt value)
  {
    bool are_equal = true;
    if(   matrix.number_of_rows()    != rows_
       || matrix.number_of_columns() != columns_) 
    {
      return false;
    }
    for(size_t row=1; row<rows; ++row) 
      for(size_t column=1; column<columns; ++column) 
        are_equal &= equal(matrix(row,column), value);
    return are_equal;
  }

  template <typename Tmplt>
  void equal(Matrix<Tmplt>& matrix, Tmplt* array)
  {
    bool are_equal = true;
    if(   matrix.number_of_rows()    != rows_
       || matrix.number_of_columns() != columns_) 
    {
      return false;
    }
    for(size_t row=1; row<rows; ++row) 
      for(size_t column=1; column<columns; ++column) 
        are_equal &= equal(matrix(row,column), array[(row-1)*columns_+column]);
    return are_equal;
  }


  template <typename Tmplt>
  bool equal(Matrix<Tmplt> m1, Matrix<Tmplt> m2)
  { 
    bool are_equal = true;
    if(   m1.number_of_rows()    != m2.number_of_rows()
       || m1.number_of_columns() != m2.number_of_columns()) 
    {
      return false;
    }
    for(size_t i=1; i<m1.number_of_rows(); i++) 
      for(size_t j=1; j<m1.number_of_columns(); j++) 
        are_equal &= equal(m1(i,j), m2(i,j));
    return are_equal;
  }

  template <typename Tmplt>
  bool equal(Vector<Tmplt> v1, Vector<Tmplt> v2)
  { 
    bool are_equal = true;
    if(v1.size() != v2.size() )
    {
      return false;
    }
    for(size_t i=1; i<v1.number_of_rows(); i++) 
      are_equal &= equal(v1(i), v2(i));
    return are_equal;
  }
  static const size_t data_size_;
  static const double double_data_[24];
  static const complex complex_data_[24];
  static const size_t size_; //square matrix
  static const size_t rows_;
  static const size_t columns_;
  static const size_t submatrix_start_row_;
  static const size_t submatrix_rows_;
  static const size_t submatrix_start_column_;
  static const size_t submatrix_columns_;
  double double_data_doubled_[24];
  double complex_data_doubled_[24];
  static const complex complex_scaling_factor_;
  complex complex_data_multiple_[24];
};

template bool MatrixTest::elements_equal(Matrix<double> matrix, double value);
template bool MatrixTest::elements_equal(Matrix<complex> matrix, complex value);
template bool MatrixTest::equal(Matrix<double> matrix, double* array);
template bool MatrixTest::equal(Matrix<complex> matrix, complex* array);
template bool MatrixTest::equal(Matrix<double> m1, Matrix<double> m2);
template bool MatrixTest::equal(Matrix<complex> m1, Matrix<complex> m2);
template bool MatrixTest::equal(Vector<double> v1, Vector<double> v2);
template bool MatrixTest::equal(Vector<complex> v1, Vector<complex> v2);


//****************************************
//MatrixTest static const initializations
//****************************************

const double MatrixTest::double_data_[24] = {
  0.,   1.,   2.,   3.,     -5.,    -8.,    -13.,   -21.,
  34.5, 55.7, 89.3, 144.2,  -32.5,  -57.5,  -91.2,  -146.3,
  3.14, -1.59 2.65, -3.58,  9.79,   -3.23,  8.46,   -2.64
};

const complex MatrixTest::complex_data_[24] = {
  complex(0., -2.64),     complex(1., 8.46),      complex(2., -3.23),
  complex(3., 9.79),      complex(-5., -3.58),    complex(-8., 2.65),
  complex(-13., -1.59),   complex(-21, 3.14),

  complex(34.5, -146.3),  complex(55.7, -91.2),   complex(89.3, -57.5),
  complex(144.2, -32.5),  complex(-32.5, 144.2),  complex(-57.5, 89.3),
  complex(-91.2, 55.7),   complex(-146.3, 34.5)
  
  complex(3.14, -21.),    complex(-1.59, -13.),   complex(2.65, -8.),
  complex(-3.58, -5.),    complex(9.79, 3.),      complex(-3.23, 2.),
  complex(8.46, 1.),      complex(-2.64, 0.)
};

const size_t MatrixTest::data_size_ = 24;
const size_t MatrixTest::size_      = 4;
const size_t MatrixTest::rows_      = 3;
const size_t MatrixTest::columns_   = 8;
const size_t MatrixTest::submatrix_rows_         = 2;
const size_t MatrixTest::submatrix_columns_      = 4;
const size_t MatrixTest::submatrix_start_row_    = 2;
const size_t MatrixTest::submatrix_start_column_ = 3;
const complex MatrixTest::complex_scaling_factor_ = complex(2, -5);

//***********
//test cases
//***********

TEST_F(MatrixTest, DefaultConstructor) {
  Matrix<double> matrix_d0;
  ASSERT_EQ(matrix_d0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d0.number_of_columns(),  (size_t) 0);
  
  Matrix<complex> matrix_c0;
  ASSERT_EQ(matrix_c0.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c0.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, ConstructorFromArray) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  EXPECT_TRUE(equal(matrix_d0, double_data_));

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  EXPECT_TRUE(equal(matrix_c0, complex_data_));
}

TEST_F(MatrixTest, IndexingElements) {
  Matrix<double> matrix_d1(rows_, columns_, double_data_);
  const Matrix<double> matrix_d2(rows_, columns_, double_data_);
  for(size_t i=1; i<=rows_; ++i)
  {
    for(size_t j=0; j<columns_; ++j)
    {
      //check that it returns correct value
      EXPECT_EQ(matrix_d1(i+1,j+1), double_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_d1[i][j], double_data_[i*columns_ + j]);
      //check that it works with const matrix
      EXPECT_EQ(matrix_d2(i+1,j+1), double_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_d2[i][j], double_data_[i*columns_ + j]);

      //check that it can set the correct value
      matrix_d1(i+1,j+1) = 4.;
      EXPECT_EQ(matrix_d1(i+1,j+1), 4.);
      matrix_d1[i][j] = -6.;
      EXPECT_EQ(matrix_d1[i][j], -6.);
    }
  
  Matrix<complex> matrix_c1(rows_, columns_, complex_data_);
  //check that it works for const as well
  const Matrix<complex> matrix_c2(rows_, columns_, complex_data_);
  complex new_value1 = complex(4., -2.);
  complex new_value2 = complex(-6., 15.);
  for(size_t i=1; i<=rows_; ++i)
  {
    for(size_t j=0; j<columns_; ++j)
    {
      //check that it returns correct value
      EXPECT_EQ(matrix_c1(i+1,j+1), complex_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_c1[i][j], complex_data_[i*columns_ + j]);
      //check that it works with const matrix
      EXPECT_EQ(matrix_c2(i+1,j+1), complex_data_[i*columns_ + j]);
      EXPECT_EQ(matrix_c2[i][j], complex_data_[i*columns_ + j]);

      //check that it can set the correct value
      matrix_c1(i+1,j+1) = new_value1;
      EXPECT_EQ(matrix_c1(i+1,j+1), new_value1);
      matrix_c1[i][j] = new_value2;
      EXPECT_EQ(matrix_c1[i][j], new_value2);
    }
  }
}

TEST_F(MatrixTest, CopyConstructor) {
  //should work if matrix is const
  const Matrix<double> matrix_d0(rows_, columns_, double_data_);
  const Matrix<double> matrix_d1(matrix_d0);
  EXPECT_TRUE(equal(matrix_d0, matrix_d1);
  
  //check that handles 0 length okay
  Matrix<double> matrix_d2;
  Matrix<double> matrix_d3(matrix_d2);
  EXPECT_TRUE(equal(matrix_d2, matrix_d3);
  
  //should work if matrix is const
  const Matrix<complex> matrix_c0(complex, rows_, columns_);
  const Matrix<complex> matrix_c1(matrix_c0);
  EXPECT_TRUE(equal(matrix_c0, matrix_c1);
  
  //check that handles 0 length okay
  Matrix<complex> matrix_c2;
  Matrix<complex> matrix_c3(matrix_c2);
  EXPECT_TRUE(equal(matrix_c2, matrix_c3);
}

TEST_F(MatrixTest, HepMatrixConstructor) {
  ::CLHEP::HepRandom hep_random;
  const ::CLHEP::HepMatrix matrix_hep0(rows_, columns_, hep_random);
  const Matrix<double> matrix_d0(hep_matrix);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) rows_);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) columns_);
  for(size_t i=1; i<=rows_; ++i)
    for(size_t j=1; j<=columns_; ++j)
      EXPECT_EQ(matrix_d0(i,j), matrix_hep0(i,j));

  //check that handles 0 length okay
  ::CLHEP::HepMatrix matrix_hep1;
  Matrix<double> matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) 0);

  const Matrix<complex> matrix_c0(hep_matrix);
  ASSERT_EQ(matrix_d1.number_of_rows(),     (size_t) rows_);
  ASSERT_EQ(matrix_d1.number_of_columns(),  (size_t) columns_);
  for(size_t i=1; i<=rows_; ++i)
    for(size_t j=1; j<=columns_; ++j)
      EXPECT_EQ(matrix_d0(i,j), complex(matrix_hep0(i,j)));
  
  //check that handles 0 length okay
  Matrix<complex> matrix_d1(matrix_hep1);
  ASSERT_EQ(matrix_c1.number_of_rows(),     (size_t) 0);
  ASSERT_EQ(matrix_c1.number_of_columns(),  (size_t) 0);
}

TEST_F(MatrixTest, ConstructorSizeOnly) {
  Matrix<double> matrix_d0(rows_, columns_);
  EXPECT_TRUE(elements_equal(matrix_d0, 0.);
  
  Matrix<complex> matrix_c0(rows_, columns_);
  EXPECT_TRUE(elements_equal(matrix_c0, complex(0.0));
}

TEST_F(MatrixTest, ConstructorFill) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_d0, double_data_[4]);
  
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_[4]);
  EXPECT_TRUE(elements_equal(matrix_c0, complex_data_[4]);
}

TEST_F(MatrixTest, IndexingRows) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  for (size_t row=0; row<rows_; ++row)
  {
    Vector<double> row_vector = matrix_d0.row(row);
    for (size_t column=0; column<columns_; ++column)
    {
      EXPECT_EQ(row_vector[column], double_data_[row*columns_ + column]);
    }
  }

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  for (size_t row=0; row<rows_; ++row)
  {
    Vector<complex> row_vector = matrix_c0.row(row);
    for (size_t column=0; column<columns_; ++column)
    {
      EXPECT_EQ(row_vector[column], complex_data_[row*columns_ + column]);
    }
  }
}

TEST_F(MatrixTest, IndexingColumns) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  for (size_t column=0; column<columns_; ++column)
  {
    Vector<double> column_vector = matrix_d0.column(column);
    for (size_t row=0; row<rows_; ++row)
    {
      EXPECT_EQ(column_vector[row], double_data_[row*columns_ + column]);
    }
  }
  
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  for (size_t column=0; column<columns_; ++column)
  {
    Vector<complex> column_vector = matrix_c0.column(column);
    for (size_t row=0; row<rows_; ++row)
    {
      EXPECT_EQ(column_vector[row], complex_data_[row*columns_ + column]);
    }
  }
}

TEST_F(MatrixTest, Submatrix) {
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  //union of the last two rows_ and middle four columns_
  Matrix<double> submatrix = matrix_d0.submatrix(
    submatrix_start_row_,    submatrix_rows_,
    submatrix_start_column_, submatrix_columns_);
  ASSERT_EQ(submatrix.number_of_rows(),     (size_t) submatrix_rows_);
  ASSERT_EQ(submatrix.number_of_columns(),  (size_t) submatrix_columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      EXPECT_EQ(
        submatrix(row,column),
        double_data_[  (row+submatrix_start_row-1)*columns_
                     + column+submatrix_start_column]);
    }
  }

  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  //union of the last two rows_ and middle four columns_
  Matrix<complex> submatrix = matrix_c0.submatrix(
    submatrix_start_row_,    submatrix_rows_,
    submatrix_start_column_, submatrix_columns_);
  ASSERT_EQ(submatrix.number_of_rows(),     (size_t) submatrix_rows_);
  ASSERT_EQ(submatrix.number_of_columns(),  (size_t) submatrix_columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      EXPECT_EQ(
        submatrix(row,column),
        complex_data_[  (row+submatrix_start_row-1)*columns_
                      + column+submatrix_start_column]);
    }
  }
}

TEST_F(MatrixTest, Comparison) {
  //*** double equality ***

  //self-equality
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d0);
  
  //identically prepared equality
  Matrix<double> matrix_d1(rows_, columns_, double_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d1);
  
  //empty equality
  Matrix<double> matrix_d2();
  Matrix<double> matrix_d3();
  EXPECT_TRUE(matrix_d2 == matrix_d3);

  //*** double inequality ***

  //different sizes
  Matrix<double> matrix_d4 = matrix_d0.submatrix(1, 1, rows_-1, columns-1);
  EXPECT_TRUE(matrix_d0 != matrix_d4);
  EXPECT_FALSE(matrix_d0 == matrix_d4);

  //empty/non-empty
  EXPECT_TRUE(matrix_d2 != matrix_d0);
  EXPECT_FALSE(matrix_d2 == matrix_d0);

  //same size, different elements
  Matrix<double> matrix_d5(rows_, columns_, double_data_);
  Matrix<double> matrix_d6(rows_, columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      matrix_d6(row, column) = (double) row*column;
    }
  }
  EXPECT_TRUE(matrix_d5 != matrix_d6);
  EXPECT_FALSE(matrix_d5 == matrix_d6);

  //*** complex equality ***
  
  //self-equality
  Matrix<complex> matrix_d0(rows_, columns_, complex_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d0);
  
  //identically prepared equality
  Matrix<complex> matrix_d1(rows_, columns_, complex_data_);
  EXPECT_TRUE(matrix_d0 == matrix_d1);
  
  //empty equality
  Matrix<complex> matrix_d2();
  Matrix<complex> matrix_d3();
  EXPECT_TRUE(matrix_d2 == matrix_d3);
  
  //*** complex inequality ***
  
  //different sizes
  Matrix<complex> matrix_c4 = matrix_c0.submatrix(1, 1, rows_-1, columns-1);
  EXPECT_TRUE(matrix_c0 != matrix_c4);
  EXPECT_FALSE(matrix_c0 == matrix_c4);
  
  //empty/non-empty
  EXPECT_TRUE(matrix_c2 != matrix_c0);
  EXPECT_FALSE(matrix_c2 == matrix_c0);
  
  //same size, different elements
  Matrix<complex> matrix_c5(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c6(rows_, columns_);
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      matrix_c6(row, column) = (complex) row*column;
    }
  }
  EXPECT_TRUE(matrix_c5 != matrix_c6);
  EXPECT_FALSE(matrix_c5 == matrix_c6);
}

//TODO: CONTINUE CLEANUP FROM HERE

TEST_F(MatrixTest, Assignment) {
  //plain vanilla assignment
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1 = matrix_d0;
  EXPECT_TRUE(matrix_d0, matrix_d1);

  //plain vanilla assignment
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1 = matrix_c0;
  EXPECT_TRUE(matrix_c0, matrix_c1);
}

TEST_F(MatrixTest, Addition) {
  //add and assign
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1();
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  //add
  Matrix<double> matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, double_data_doubled_));

  //add and assign
  Matrix<complex> matrix_d0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_d1();
  matrix_d1 += matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d1));

  //add
  Matrix<complex> matrix_d2 = matrix_d0 + matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, complex_data_doubled_));
}

TEST_F(MatrixTest, Subtraction) {
  //subtract and assign
  Matrix<double> matrix_d0(rows_, columns_, double_data_);
  Matrix<double> matrix_d1(rows_, columns_, double_data_doubled_);
  Matrix<double> matrix_d2(matrix_d1);
  matrix_d2 -= matrix_d0;
  ASSERT_TRUE(equal(matrix_d0, matrix_d2));

  //subtract
  Matrix<double> matrix_d3 = matrix_d1 - matrix_d0;
  EXPECT_TRUE(equal(matrix_d3, double_data_));

  //subtract and assign
  Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  Matrix<complex> matrix_c1( rows_, columns_, complex_data_multiple_);
  Matrix<complex> matrix_c2(matrix_c1);
  matrix_c2 -= matrix_c0;
  ASSERT_TRUE(equal(matrix_c0, matrix_c2));

  //subtract
  Matrix<complex> matrix_c3 = matrix_c1 - matrix_c0;
  EXPECT_TRUE(equal(matrix_c3, complex_data_));
}

TEST_F(MatrixTest, Multiplication) {
  bool testpass = true;

  double d1[6] = {1,2,3,7,6,5};
  double d2[6] = {3,4,5,2,8,3};
  const Matrix<double>  matrix_d0(3, 2, d1);
  const Matrix<double>  matrix_d1(2, 3, d2);
  Matrix<double>        matrix_d2(3, 3);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_d2(i,j) += matrix_d0(i,k)*matrix_d1(k,j);
  Matrix<double> matrix_d3(matrix_d0)
  matrix_d3 *= matrix_d1;
  ASSERT_TRUE(equal(matrix_d2, matrix_d3));
  
  Matrix<double> matrix_d4 = matrix_d0 * matrix_d1;
  EXPECT_TRUE(equal(matrix_d2, matrix_d4));
       
  complex c1[6];
  complex c2[6];
  for(int i=0; i<6; i++) c1[i] = complex( d1[i],       d2[i]);
  for(int i=0; i<6; i++) c2[i] = complex(-d2[i]*d1[i], d1[i]);
  const Matrix<complex>  matrix_c0(3, 2, d1);
  const Matrix<complex>  matrix_c1(2, 3, d2);
  Matrix<complex>        matrix_c2(3, 3);
  for(int i=1; i<=3; i++)
    for(int j=1; j<=3; j++)
      for(int k=1; k<=2; k++)
        matrix_c2(i,j) += matrix_c0(i,k) * matrix_c1(k,j);
  Matrix<complex> matrix_c3(matrix_c0)
  matrix_c3 *= matrix_c1;
  ASSERT_TRUE(equal(matrix_c2, matrix_c3));
  
  Matrix<complex> matrix_c4 = matrix_c0 * matrix_c1;
  EXPECT_TRUE(equal(matrix_c2, matrix_c4));
}

//FIXME: add scalar multiplication on the left
TEST_F(MatrixTest, ScalarMultiplication) {
  //*** double multiplication ***
  
  //multiply and assign
  const Matrix<double> matrix_d0(double_data_, rows_, columns_);
  Matrix<double> matrix_d1(matrix_d0);
  matrix_d1 *= 2.;
  ASSERT_TRUE(equal(matrix_d1, double_data_doubled_));

  //multiplication on the right by a scalar
  Matrix<double> matrix_d2 = matrix_d0 * 2.;
  ASSERT_TRUE(equal(matrix_d2, double_data_doubled_));

  //multiplication on the left by a scalar
  Matrix<double> matrix_d3 = 2. * matrix_d0;
  ASSERT_TRUE(equal(matrix_d2, double_data_doubled_));
  
  //*** double multiplication ***

  //multiply and assign
  const Matrix<complex> matrix_c0(complex_data_, rows_, columns_);
  Matrix<complex> matrix_c1(matrix_c0);
  matrix_c1 *= complex_scaling_factor_;
  ASSERT_TRUE(equal(matrix_c1, complex_data_multiple_));

  //multiplication on the right by a scalar
  Matrix<complex> matrix_c2 = matrix_c0 * complex_scaling_factor_;
  ASSERT_TRUE(equal(matrix_c2, complex_data_multiple_));

  //multiplication on the left by a scalar
  Matrix<complex> matrix_c3 = complex_scaling_factor_ * matrix_d0;
  ASSERT_TRUE(equal(matrix_d2, complex_data_multiple_));
}

TEST_F(MatrixTest, ScalarDivision) {
  const Matrix<double> matrix_d0(rows_, columns_, double_data_doubled_);
  Matrix<double> matrix_d1(matrix_d0);
  matrix_d1 /= 2.;
  ASSERT_TRUE(equal(matrix_d1, double_data_));

  Matrix<double> matrix_d2 = matrix_d0 / 2.;
  ASSERT_TRUE(equal(matrix_d2, double_data_));

  const Matrix<complex> matrix_c0(rows_, columns_, complex_data_multiple_);
  Matrix<complex> matrix_c1(matrix_c0);
  matrix_c1 /= complex_scaling_factor_;
  ASSERT_TRUE(equal(matrix_c1, complex_data_));

  Matrix<complex> matrix_c2 = matrix_c0 / complex_scaling_factor_;
  ASSERT_TRUE(equal(matrix_c2, complex_data_));
}

TEST_F(MatrixTest, ComplexDecomposition) {
  const Matrix<complex> matrix_c0(rows_, columns_, complex_data_);
  const Matrix<double> matrix_d0(rows_, columns_, double_data_);

  //the real part of complex_data_ is just double_data_
  Matrix<double> matrix_real = real(matrix_c0);
  EXPECT_TRUE(equal(matrix_real, matrix_d0));

  //subtract off the imaginary part of complex_data_
  //which is again just double_data_
  Matrix<double> matrix_imag = imag(matrix_c0);
  //create a pure-imaginary matrix
  Matrix<complex> matrix_pure_imag(rows_, columns_);
  for(size_t row=1; row<=rows_; ++row)
  {
    for(size_t column=1; column<=columns_; ++column)
    {
      matrix_pure_imag(row, column) += complex(0., matrix_imag(row, column));
    }
  }
  Matrix<complex> matrix_c1 = matrix_c0 - matrix_pure_imag;
  Matrix<double> matrix_d1(rows_, columns_);
  for(size_t row=1; row<=rows_; ++row)
  {
    for(size_t column=1; column<=columns_; ++column)
    {
      matrix_d1(row, column) = real(matrix_c1(row, column));
    }
  }
  EXPECT_TRUE(equal(matrix_d1, matrix_d0));
}

TEST_F(MatrixTest, ComplexComposition) {
//TODO: test complex(Matrix<double>) and complex(Matrix<double>, Matrix<double>)
}

TEST_F(MatrixTest, Determinant) {
  Matrix<double> matrix_d1(size_, size_, double_data_);
  for(size_t i=1; i<=2; ++i)
  {
    for(size_t j=3; j<=4; ++j)
    {
      matrix_d1(i,j) = 0.;
      matrix_d1(j,i) = 0.;
    }
  }
  //det is equal to det(upper block diag)*det(lower block diag)
  double upper_block_diag_det = matrix_d1(1,1)*matrix_d1(2,2)
                              - matrix_d1(1,2)*matrix_d1(2,1);
  double lower_block_diag_det = matrix_d1(3,3)*matrix_d1(4,4)
                              -matrix_d1(3,4)*matrix_d1(4,3)
  EXPECT_NEAR(upper_block_diag_det * lower_block_diag_det,
              matrix_d1.determinant(),
              1e-6);

  //if 2 rows are identical, det is 0
  Matrix<double> matrix_d2(size_, size_, double_data_);
  for(size_t column=1; column<=size_; ++column)
  {
    matrix_d2(2, column) = matrix_d2(1, column);
  }
  EXPECT_NEAR(0., matrix_d2.determinant(), 1e-6);
}

TEST_F(MatrixTest, Inverse) {
  Matrix<double> matrix_d1(size_, size_, double_data_);
  Matrix<double> matrix_d2 = inverse(matrix_d1);
  Matrix<double> matrix_d3 = matrix_d1 * matrix_d2; //M^-1 * M = Identity
  for(size_t row=1; row<=rows_; ++row)
  {
    for(size_t column=1; column<=columns_; ++column)
    {
      EXPECT_NEAR(row==column ? 1. : 0., matrix_d3(row, column), 1e-9);
    }
  }
}

TEST_F(MatrixTest, Trace) {
  Matrix<double> matrix_d1(size_, size_, double_data_);
  double tr = matrix_d1.trace();
  for(size_t i=1; i<=size_; i++)
  {
    tr -= matrix_d1(i, i);
  }
  EXPECT_DOUBLE_EQ(tr, 0.);
}

TEST_F(MatrixTest, Transpose) {
  Matrix<double> matrix_da(rows_, columns_, double_data_);
  Matrix<double> matrix_dt = transpose(matrix_da);
  ASSERT_EQUALS(matrix_da.number_of_rows(), matrix_dt.number_of_columns());
  ASSERT_EQUALS(matrix_da.number_of_columns(), matrix_dt.number_of_rows());
  for(size_t i = 1; i<=matrix_da.number_of_rows(); ++i)
    for(size_t j = 1; j<=matrix_da.number_of_columns(); ++j)
      EXPECT_EQUALS(matrix_da(i,j), matrix_dt(j,i));

  Matrix<complex> matrix_ca(rows_, columns_, complex_data_);  
  Matrix<complex> matrix_ct = transpose(matrix_ca);
  ASSERT_EQUALS(matrix_ca.number_of_rows(), matrix_ct.number_of_columns());
  ASSERT_EQUALS(matrix_ca.number_of_columns(), matrix_ct.number_of_rows());
  for(size_t i = 1; i<=matrix_ca.number_of_rows(); ++i)
    for(size_t j = 1; j<=matrix_ca.number_of_columns(); ++j)
      EXPECT_EQUALS(matrix_ca(i,j), matrix_ct(j,i));
}

TEST_F(MatrixTest, HepMatrix) {
  ::CLHEP::HepRandom hep_random;
  const Matrix<double> matrix_d0(rows_, columns_, double_data_);
  ::CLHEP::HepMatrix matrix_hep0 = MAUS::CLHEP::HepMatrix(matrix_d0);
  ASSERT_EQUALS(matrix_hep0.num_row(), matrix_d0.number_of_rows());
  ASSERT_EQUALS(matrix_hep0.num_col(), matrix_d0.number_of_columns());
  for(size_t row=1; row<=submatrix_rows_; ++row)
  {
    for(size_t column=1; column<=submatrix_columns_; ++column)
    {
      EXPECT_NEAR(matrix_hep0(row, column), matrix_d0(row, column), 1e-9);
    }
  }
}

TEST_F(MatrixTest, HepMatrix) {
  Matrix<double> md(size_, size_, double_data_);
  std::pair< Vector<complex>, Matrix<complex> > evec  = md.eigenvectors();
  Vector<complex> eval  = md.eigenvalues();
  for(size_t i=1; i<evec.first.number_of_rows(); i++)
    testpass &= equal(complex(md) * evec.second.get_mvector(1), evec.second.get_mvector(1)*evec.first(1) ) ;

  Vector<complex> eval2 = evec.first;
  for(size_t i=1; i<=eval2.number_of_rows(); i++)
  {
    bool test_here = false;
    for(size_t j=1; j<=eval2.number_of_rows(); j++)
      if( equal( eval2(i),eval(j) ) ) test_here = true;
    testpass &= test_here; //pass test if for each value in eval2 there is also a value in eval and size_ of eval2 == size_ of eval
  }    
  testpass &= eval.number_of_rows() == eval2.number_of_rows();
}

TEST_F(MatrixTest, Negation) {
}

TEST_F(MatrixTest, VectorMultiplication) {
  //reverse the order of a vector's elements using a matrix
  Matrix<double> matrix_d0(size_, size_, double_data_);
  Matrix<double> matrix_reverse_d(size_, size_);
  for (size_t i=1; i<=size_; ++i)
  {
    matrix_d1(i,size_-i+1) = 1.;
  }
  Vector<double> vector_d0 = matrix_d0.row(1);
  Vector<double> vector_product_d = matrix_reverse_d * vector_d0;
  for (size_t i=1; i<=size_; ++i)
  {
    EXPECT_NEAR(vector_product_d(i), vector_d0(size_-i+1), 1e-9);
  }

  //reverse the order of a vector's elements using a matrix
  Matrix<complex> matrix_c0(size_, size_, complex_data_);
  Matrix<complex> matrix_reverse_c(size_, size_);
  for (size_t i=1; i<=size_; ++i)
  {
    matrix_d1(i,size_-i+1) = complex(1.,0.);
  }
  Vector<complex> vector_c0 = matrix_c0.row(1);
  Vector<complex> vector_product_c = matrix_reverse_c * vector_c0;
  for (size_t i=1; i<=size_; ++i)
  {
    EXPECT_NEAR(vector_product_c(i), vector_c0(size_-i+1), 1e-9);
  }
}

//FIXME
TEST_F(MatrixTest, Streaming) {
  bool testpass = true;
  std::stringstream test;
  Matrix<double> matrix_da( rows_, columns_, test_data );
  Matrix<double> matrix_db;
  test << matrix_da;
  test >> matrix_db;
  testpass &= equal(matrix_da, matrix_db);

  Matrix<complex> matrix_ca( rows_, columns_, complex_data_ );
  Matrix<complex> matrix_cb;
  test << matrix_ca;
  test >> matrix_cb;
  testpass &= equal(matrix_ca, matrix_cb);

  return testpass;
}
