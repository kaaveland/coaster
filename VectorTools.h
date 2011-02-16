#ifndef VECTORTOOLS_H
#define VECTORTOOLS_H
#include "Vector3d.h"

// Prototypes
Vector3d vectorSum(Vector3d const &v0, Vector3d const &v1);
Vector3d vectorDiff(Vector3d const &v1, Vector3d const &v0);
double vectorLength(Vector3d const &v);
Vector3d vectorTimesScalar(Vector3d const &v, double scalar);
double vectorDot(Vector3d const &v0, Vector3d const &v1);
Vector3d vectorCross(Vector3d const &v0, Vector3d const &v1);


#endif