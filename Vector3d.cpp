#include "Vector3d.h"

Vector3d::Vector3d()
{

}

Vector3d::Vector3d(double x, double y, double z)
{ 
	this->x = x;
	this->y = y;
	this->z = z;
}

double const& Vector3d::operator[](unsigned i) const
{
	if (i==0) return x;
	if (i==1) return y;
	if (i==2) return z;
}