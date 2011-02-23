#include "Vector3d.h"
#include <exception>

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
	throw std::exception("Vector3d::operator[]: index out of range");
}

Vector3d& Vector3d::operator+=(const Vector3d &v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d &v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

const Vector3d Vector3d::operator+(const Vector3d &v) const
{
	Vector3d sum = *this;
	sum += v;
	return sum;
}

const Vector3d Vector3d::operator-(const Vector3d &v) const
{
	Vector3d sum = *this;
	sum -= v;
	return sum;
}

bool Vector3d::operator==(const Vector3d &v) const
{
	return (this->x == v.x && this->y == v.y && this->z);
}

bool Vector3d::operator!=(const Vector3d &v) const
{
	return !(*this == v);
}

Vector3d& Vector3d::operator=(const Vector3d &v)
{
	if (this != &v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}
	return *this;
}