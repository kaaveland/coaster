#ifndef VECTOR3D_H
#define VECTOR3D_H

struct Vector3d {

	double x, y, z;

	Vector3d();

	Vector3d(double x, double y, double z) 
	{ 
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	double& operator[](unsigned i) 
	{
		if (i==0) return x;
		if (i==1) return y;
		if (i==2) return z;
	}
};

#endif