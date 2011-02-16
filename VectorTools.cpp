#include "Vector3d.h"
#include "Vector3d.h"
#include <math.h>

Vector3d vectorSum(Vector3d const &v0, Vector3d const &v1)
{
	double dx = v0.x+v1.x;	// dx
	double dy = v0.y+v1.y;	// dy
	double dz = v0.z+v1.z;	// dz
	
	return Vector3d(dx,dy,dz);
}

// Calculates and returns the vector difference v1-v0 
Vector3d vectorDiff(Vector3d const &v1, Vector3d const &v0)
{
	double dx = v1.x-v0.x;	// dx
	double dy = v1.y-v0.y;	// dy
	double dz = v1.z-v0.z;	// dz
	
	return Vector3d(dx,dy,dz);
}

double vectorLength(Vector3d const &v) {
	return sqrt( pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) );
}

Vector3d vectorTimesScalar(Vector3d const &v, double scalar) {
	return Vector3d(v.x*scalar, v.y*scalar, v.z*scalar);
}

// Calculates the vector scalar (dot) product of v1 and v2 and returns the scalar
double vectorDot(Vector3d const &v0, Vector3d const &v1)
{
	return (v0.x*v1.x + v0.y*v1.y + v0.z*v1.z);
}

// Calculates the vector cross product of v0 and v1.
Vector3d vectorCross(Vector3d const &v0, Vector3d const &v1) {
	double x = v0.y*v1.z - v0.z*v1.y;
	double y = v0.z*v1.x - v0.x*v1.z;
	double z = v0.x*v1.y - v0.y*v1.x;
	return Vector3d(x,y,z);
}