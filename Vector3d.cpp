#include "Vector3d.h"
#include <math.h>
#include <exception>
#include <istream>
#include <ostream>

Vector3d::Vector3d()
{

}

Vector3d::Vector3d(double x, double y, double z)
{ 
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3d::Vector3d(Ogre::Vector3 v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

double const& Vector3d::operator[](unsigned i) const
{
	if (i==0) return x;
	if (i==1) return y;
	if (i==2) return z;
	throw std::exception("Vector3d::operator[]: index out of range");
}

void Vector3d::read(std::istream &in)
{
	in >> x; in >> y; in >> z;
}

void Vector3d::dump(std::ostream &out)
{
	out << x << " " << y << " " << z << std::endl;
}

Ogre::Vector3 Vector3d::ogreVector()
{
	return Ogre::Vector3(this->x, this->y, this->z);
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

Vector3d& Vector3d::operator*=(double d)
{
	this->x *= d;
	this->y *= d;
	this->z *= d;
	return *this;
}
Vector3d& Vector3d::operator/=(double d)
{
	this->x /= d;
	this->y /= d;
	this->z /= d;
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

const Vector3d Vector3d::operator*(double d) const
{
	Vector3d result = *this;
	result *= d;
	return result;
}

const Vector3d Vector3d::operator/(double d) const
{
	Vector3d result = *this;
	result /= d;
	return result;
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

double Vector3d::operator*(const Vector3d &v) const 
{
	return (this->x*v.x + this->y*v.y + this->z*v.z);
}

const Vector3d Vector3d::cross(const Vector3d &v) const
{
	double x = this->y*v.z - this->z*v.y;
	double y = this->z*v.x - this->x*v.z;
	double z = this->x*v.y - this->y*v.x;
	return Vector3d(x,y,z);
}

double Vector3d::length() const
{
	return sqrt( pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2) );
}


const Vector3d operator*(double d, Vector3d &v)
{
	return v * d;
}

const Vector3d operator/(double d, Vector3d &v)
{
	return v / d;
}

Vector3d operator+(const Vector3d &v)
{
	return v;
}
Vector3d operator-(const Vector3d &v)
{
	return Vector3d(-v.x, -v.y, -v.z);
}

Vector3d Vector3d::normalizedCopy() const
{
	double length = this->length();
	if (length == 0.0) return *this;	// Do nothing if length is zero

	return Vector3d(x,y,z)/length;
}

