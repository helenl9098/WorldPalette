/****************************************************************
*																*
* C++ Vector and Matrix Algebra routines						*
* Author: Jean-Francois DOUE									*
* Version 3.1 --- October 1993									*
*																*
****************************************************************/
//
//	From "Graphics Gems IV / Edited by Paul S. Heckbert
//	Academic Press, 1994, ISBN 0-12-336156-9
//	"You are free to use and modify this code in any way 
//	you like." (p. xv)
//
//	Modified by J. Nagle, March 1997
//	-	All functions are inline.
//	-	All functions are const-correct.
//	-	All checking is via the standard "assert" macro.
//	-	Stream I/O is disabled for portability, but can be
//		re-enabled by defining ALGEBRA3IOSTREAMS.
//
//  Modified by Aline N
// - Added vec4
// - Added Cross Product operator, set/Print functions
// - Add divide by zero length check to Length()

#pragma once

#include <iostream>
#include <assert.h>
#include <cmath>

using namespace std;

enum {VX, VY, VZ, VW};		    // axes
enum {PA, PB, PC, PD};		    // planes
enum {RED, GREEN, BLUE};	    // colors
enum {KA, KD, KS, ES};		    // phong coefficients

//////////////////////////////////////////////////////////////////////////
//PI
//
#ifndef M_PI
const double M_PI = 3.14159265358979323846f;		// per CRC handbook, 14th. ed.
#endif
const double M_PI_2 = double(M_PI/2.0f);				// PI/2
const double M2_PI = double(M_PI*2.0f);				// PI*2
const double Rad2Deg = double(180.0f / M_PI);			// Rad to Degree
const double Deg2Rad = double(M_PI / 180.0f);			// Degree to Rad

#ifndef EPSILON
#define EPSILON 0.001
#endif

// this line defines a new type: pointer to a function which returns a
// double and takes as argument a double
typedef double (*V_FCT_PTR)(double);

// min-max macros
#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

// error handling macro
#define ALGEBRA_ERROR(E) { assert(false); }

class vec2;
class vec3;
class mat3;

/****************************************************************
*																*
*			    2D Vector										*
*																*
****************************************************************/

class vec2
{
protected:

	double n[2];

public:

	// Constructors
	vec2();
	vec2(const double x, const double y);
	vec2(const vec2& v);				// copy constructor

	// Assignment operators
	vec2& operator	= ( const vec2& v );	// assignment of a vec2
	vec2& operator += ( const vec2& v );	// incrementation by a vec2
	vec2& operator -= ( const vec2& v );	// decrementation by a vec2
	vec2& operator *= ( const double d );	// multiplication by a constant
	vec2& operator /= ( const double d );	// division by a constant
	double& operator [] ( int i);			// indexing
	double operator [] ( int i) const;// read-only indexing

	// Special functions
	double Length() const;			// length of a vec2
	double SqrLength() const;		// squared length of a vec2
	vec2& Normalize() ;				// normalize a vec2 in place

	// friends
	friend vec2 operator- (const vec2& v);					// -v1
	friend vec2 operator+ (const vec2& a, const vec2& b);	// v1 + v2
	friend vec2 operator- (const vec2& a, const vec2& b);	// v1 - v2
	friend vec2 operator* (const vec2& a, const double d);	// v1 * 3.0
	friend vec2 operator* (const double d, const vec2& a);	// 3.0 * v1
	friend double operator* (const vec2& a, const vec2& b);  // dot product
	friend vec2 operator/ (const vec2& a, const double d);	// v1 / 3.0
	friend vec3 operator^ (const vec2& a, const vec2& b);	// cross product
	friend int operator== (const vec2& a, const vec2& b);	// v1 == v2 ?
	friend int operator!= (const vec2& a, const vec2& b);	// v1 != v2 ?
	friend vec2 Prod(const vec2& a, const vec2& b);		    // term by term *
	friend double Dot(const vec2& a, const vec2& b);			// dot product
};

/****************************************************************
*																*
*			    3D Vector										*
*																*
****************************************************************/

class vec3
{
public:

	double n[3];

public:

	// Constructors
	vec3();
	vec3(const double x, const double y, const double z);
	vec3(const vec3& v);					// copy constructor

	// Assignment operators
	vec3& operator	= ( const vec3& v );	    // assignment of a vec3
	vec3& operator += ( const vec3& v );	    // incrementation by a vec3
	vec3& operator -= ( const vec3& v );	    // decrementation by a vec3
	vec3& operator *= ( const double d );	    // multiplication by a constant
	vec3& operator /= ( const double d );	    // division by a constant
	double& operator [] ( int i);				// indexing
	double operator[] (int i) const;				// read-only indexing
   void set(const double x, const double y, const double z); 

	// special functions
	double Length() const;				// length of a vec3
	double SqrLength() const;			// squared length of a vec3
	vec3& Normalize();					// normalize a vec3 in place
	vec3 Cross(const vec3 &v) const;			// cross product: self cross v

	// friends
	friend vec3 operator - (const vec3& v);					// -v1
	friend vec3 operator + (const vec3& a, const vec3& b);	// v1 + v2
	friend vec3 operator - (const vec3& a, const vec3& b);	// v1 - v2
	friend vec3 operator * (const vec3& a, const double d);	// v1 * 3.0
	friend vec3 operator * (const double d, const vec3& a);	// 3.0 * v1
	friend vec3 operator * (const vec3& a, const vec3& b); // piecewise muliply
	friend vec3 operator / (const vec3& a, const double d);	// v1 / 3.0
	friend vec3 operator ^ (const vec3& a, const vec3& b);	// cross product
	friend int operator == (const vec3& a, const vec3& b);	// v1 == v2 ?
	friend int operator != (const vec3& a, const vec3& b);	// v1 != v2 ?
	friend vec3 Prod(const vec3& a, const vec3& b);		    // term by term *
	friend double Dot(const vec3& a, const vec3& b);			// dot product
	friend double Distance(const vec3& a, const vec3& b);  // distance
	friend double DistanceSqr(const vec3& a, const vec3& b);  // distance sqr
   void Print(const char* title) const;
};

const vec3 axisX(1.0f, 0.0f, 0.0f);
const vec3 axisY(0.0f, 1.0f, 0.0f);
const vec3 axisZ(0.0f, 0.0f, 1.0f);
const vec3 vec3Zero(0.0f, 0.0f, 0.0f);

inline ostream& operator << (ostream& ostrm, const vec3& v)
{
   ostrm << "(" << v[0] << ", " << v[1] << ", " << v[2] << ") ";
   return ostrm;
}


class vec4 
{
public:
	double n[4];

public:
	vec4();
	vec4(const double x, const double y, const double z, const double w);
	vec4(const vec4& v);					// copy constructor

	// Assignment operators
	vec4& operator	= ( const vec4& v );	    // assignment of a vec3
	double& operator [] ( int i);				// indexing
	double operator[] (int i) const;				// read-only indexing
   void set(const double x, const double y, const double z, const double w); 

	// friends
	friend vec4 operator - (const vec4& v);					// -v1
	friend vec4 operator + (const vec4& a, const vec4& b);	// v1 + v2
	friend vec4 operator - (const vec4& a, const vec4& b);	// v1 - v2
	friend vec4 operator * (const vec4& a, const double d);	// v1 * 3.0
	friend vec4 operator * (const double d, const vec4& a);	// 3.0 * v1
	friend vec4 operator * (const vec4& a, const vec4& b); // piecewise muliply

   void Print(const char* title) const;

};