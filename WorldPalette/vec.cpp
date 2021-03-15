#include "vec.h"

/****************************************************************
*																*
*		    vec2 Member functions								*
*																*
****************************************************************/

// CONSTRUCTORS

vec2::vec2() 
{
}

vec2::vec2(const double x, const double y)
{
	n[VX] = x; n[VY] = y; 
}

vec2::vec2(const vec2& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; 
}

// ASSIGNMENT OPERATORS

vec2& vec2::operator = (const vec2& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; return *this; 
}

vec2& vec2::operator += ( const vec2& v )
{ 
	n[VX] += v.n[VX]; n[VY] += v.n[VY]; return *this; 
}

vec2& vec2::operator -= ( const vec2& v )
{ 
	n[VX] -= v.n[VX]; n[VY] -= v.n[VY]; return *this; 
}

vec2& vec2::operator *= ( const double d )
{ 
	n[VX] *= d; n[VY] *= d; return *this; 
}

vec2& vec2::operator /= ( const double d )
{ 
	double d_inv = 1.0f/d; n[VX] *= d_inv; n[VY] *= d_inv; return *this; 
}

double& vec2::operator [] ( int i) 
{
	assert(!(i < VX || i > VY));		// subscript check
	return n[i];
}

double vec2::operator [] ( int i) const 
{
	assert(!(i < VX || i > VY));
	return n[i];
}


// SPECIAL FUNCTIONS

double vec2::Length() const
{ 
	return sqrt(SqrLength()); 
}

double vec2::SqrLength() const
{ 
	return n[VX]*n[VX] + n[VY]*n[VY]; 
}

vec2& vec2::Normalize() // it is up to caller to avoid divide-by-zero
{ 
   double len = Length();
	if (len > 0.000001) *this /= len; 
   return *this; 
}

// FRIENDS

vec2 operator - (const vec2& a)
{ 
	return vec2(-a.n[VX],-a.n[VY]); 
}

vec2 operator + (const vec2& a, const vec2& b)
{ 
	return vec2(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY]); 
}

vec2 operator - (const vec2& a, const vec2& b)
{ 
	return vec2(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY]); 
}

vec2 operator * (const vec2& a, const double d)
{ 
	return vec2(d*a.n[VX], d*a.n[VY]); 
}

vec2 operator * (const double d, const vec2& a)
{ 
	return a*d; 
}

double operator * (const vec2& a, const vec2& b)
{ 
	return (a.n[VX]*b.n[VX] + a.n[VY]*b.n[VY]); 
}

vec2 operator / (const vec2& a, const double d)
{ 
	double d_inv = 1.0f/d; return vec2(a.n[VX]*d_inv, a.n[VY]*d_inv); 
}

vec3 operator ^ (const vec2& a, const vec2& b)
{ 
	return vec3(0.0, 0.0, a.n[VX] * b.n[VY] - b.n[VX] * a.n[VY]); 
}

int operator == (const vec2& a, const vec2& b)
{ 
	return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]); 
}

int operator != (const vec2& a, const vec2& b)
{ 
	return !(a == b); 
}

vec2 Prod(const vec2& a, const vec2& b)
{ 
	return vec2(a.n[VX] * b.n[VX], a.n[VY] * b.n[VY]); 
}

double Dot(const vec2& a, const vec2& b)
{
	return a*b;
}


/****************************************************************
*																*
*		    vec3 Member functions								*
*																*
****************************************************************/

// CONSTRUCTORS

vec3::vec3() 
{
}

vec3::vec3(const double x, const double y, const double z)
{ 
	n[VX] = x; n[VY] = y; n[VZ] = z; 
}

vec3::vec3(const vec3& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; 
}

// ASSIGNMENT OPERATORS

vec3& vec3::operator = (const vec3& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; return *this; 
}

vec3& vec3::operator += ( const vec3& v )
{ 
	n[VX] += v.n[VX]; n[VY] += v.n[VY]; n[VZ] += v.n[VZ]; return *this; 
}

vec3& vec3::operator -= ( const vec3& v )
{ 
	n[VX] -= v.n[VX]; n[VY] -= v.n[VY]; n[VZ] -= v.n[VZ]; return *this; 
}

vec3& vec3::operator *= ( const double d )
{ 
	n[VX] *= d; n[VY] *= d; n[VZ] *= d; return *this; 
}

vec3& vec3::operator /= ( const double d )
{ 
	double d_inv = 1.0f/d; n[VX] *= d_inv; n[VY] *= d_inv; n[VZ] *= d_inv;
	return *this; 
}

double& vec3::operator [] ( int i) {
	assert(! (i < VX || i > VZ));
	return n[i];
}

double vec3::operator [] ( int i) const {
	assert(! (i < VX || i > VZ));
	return n[i];
}

void vec3::set(const double x, const double y, const double z)
{
   n[0] = x; n[1] = y; n[2] = z;
}

// SPECIAL FUNCTIONS

double vec3::Length() const
{  
	return sqrt(SqrLength()); 
}

double vec3::SqrLength() const
{  
	return n[VX]*n[VX] + n[VY]*n[VY] + n[VZ]*n[VZ]; 
}

vec3& vec3::Normalize() // it is up to caller to avoid divide-by-zero
{ 
   double len = Length();
   if (len > 0.000001) *this /= Length(); 
   return *this; 
}

vec3 vec3::Cross(const vec3 &v) const
{
	vec3 tmp;
	tmp[0] = n[1] * v.n[2] - n[2] * v.n[1];
	tmp[1] = n[2] * v.n[0] - n[0] * v.n[2];
	tmp[2] = n[0] * v.n[1] - n[1] * v.n[0];
	return tmp;
}

void vec3::Print(const char* title) const
{
   printf("%s (%.4f, %.4f, %.4f)\n", title, n[0], n[1], n[2]);
}

// FRIENDS

vec3 operator - (const vec3& a)
{  
	return vec3(-a.n[VX],-a.n[VY],-a.n[VZ]); 
}

vec3 operator + (const vec3& a, const vec3& b)
{ 
	return vec3(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY], a.n[VZ] + b.n[VZ]); 
}

vec3 operator - (const vec3& a, const vec3& b)
{ 
	return vec3(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY], a.n[VZ]-b.n[VZ]); 
}

vec3 operator * (const vec3& a, const double d)
{ 
	return vec3(d*a.n[VX], d*a.n[VY], d*a.n[VZ]); 
}

vec3 operator * (const double d, const vec3& a)
{ 
	return a*d; 
}

vec3 operator * (const vec3& a, const vec3& b)
{ 
	return vec3(a.n[VX]*b.n[VX], a.n[VY]*b.n[VY], a.n[VZ]*b.n[VZ]); 
}

vec3 operator / (const vec3& a, const double d)
{ 
	double d_inv = 1.0f/d; 
	return vec3(a.n[VX]*d_inv, a.n[VY]*d_inv, a.n[VZ]*d_inv); 
}

vec3 operator ^ (const vec3& a, const vec3& b) 
{
	return vec3(a.n[VY]*b.n[VZ] - a.n[VZ]*b.n[VY],
		a.n[VZ]*b.n[VX] - a.n[VX]*b.n[VZ],
		a.n[VX]*b.n[VY] - a.n[VY]*b.n[VX]);
}

int operator == (const vec3& a, const vec3& b)
{ 
	return (a.n[VX] == b.n[VX]) && (a.n[VY] == b.n[VY]) && (a.n[VZ] == b.n[VZ]);
}

int operator != (const vec3& a, const vec3& b)
{ 
	return !(a == b); 
}

vec3 Prod(const vec3& a, const vec3& b)
{ 
	return vec3(a.n[VX] * b.n[VX], a.n[VY] * b.n[VY], a.n[VZ] * b.n[VZ]); 
}

double Dot(const vec3& a, const vec3& b)
{
   vec3 tmp = a*b;
	return tmp[0] + tmp[1] + tmp[2];
}


double Distance(const vec3& a, const vec3& b)  // distance
{
   return sqrt( (b[0]-a[0])*(b[0]-a[0]) +
                (b[1]-a[1])*(b[1]-a[1]) +
                (b[2]-a[2])*(b[2]-a[2]));
}

double DistanceSqr(const vec3& a, const vec3& b)  // distance
{
   return ( (b[0]-a[0])*(b[0]-a[0]) +
            (b[1]-a[1])*(b[1]-a[1]) +
            (b[2]-a[2])*(b[2]-a[2]));
}

///-------------------------------------------
vec4::vec4()
{
}

vec4::vec4(const double x, const double y, const double z, const double w)
{ 
	n[VX] = x; n[VY] = y; n[VZ] = z; n[VW] = w;
}

vec4::vec4(const vec4& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ];  n[VW] = v.n[VW];
}

vec4& vec4::operator = (const vec4& v)
{ 
	n[VX] = v.n[VX]; n[VY] = v.n[VY]; n[VZ] = v.n[VZ]; n[VW] = v.n[VW]; return *this; 
}


double& vec4::operator [] ( int i) {
	assert(! (i < VX || i > VW));
	return n[i];
}

double vec4::operator [] ( int i) const {
	assert(! (i < VX || i > VW));
	return n[i];
}

void vec4::set(const double x, const double y, const double z, const double w)
{
   n[0] = x; n[1] = y; n[2] = z; n[3] = w;
}

// FRIENDS

vec4 operator - (const vec4& a)
{  
	return vec4(-a.n[VX],-a.n[VY],-a.n[VZ], -a.n[VW]); 
}

vec4 operator + (const vec4& a, const vec4& b)
{ 
	return vec4(a.n[VX]+ b.n[VX], a.n[VY] + b.n[VY], a.n[VZ] + b.n[VZ], a.n[VW] + b.n[VW]); 
}

vec4 operator - (const vec4& a, const vec4& b)
{ 
	return vec4(a.n[VX]-b.n[VX], a.n[VY]-b.n[VY], a.n[VZ]-b.n[VZ], a.n[VW]-b.n[VW]); 
}

vec4 operator * (const vec4& a, const double d)
{ 
	return vec4(d*a.n[VX], d*a.n[VY], d*a.n[VZ], d*a.n[VW]); 
}

vec4 operator * (const double d, const vec4& a)
{ 
	return a*d; 
}

vec4 operator * (const vec4& a, const vec4& b)
{
	return vec4(a.n[VX]*b.n[VX], a.n[VY]*b.n[VY], a.n[VZ]*b.n[VZ], a.n[VW]*b.n[VW]); 

}

void vec4::Print(const char* title) const
{
   printf("%s (%.4f, %.4f, %.4f, %.4f)\n", title, n[0], n[1], n[2], n[3]);
}
