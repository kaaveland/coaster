#ifndef VECTOR3D_H
#define VECTOR3D_H

struct Vector3d {

	double x, y, z;

	Vector3d();
	Vector3d(double x, double y, double z);
	double& operator[](unsigned i);
};

#endif