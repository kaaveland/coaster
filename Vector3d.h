#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <istream>
#include <ostream>

struct Vector3d {

	double x, y, z;

	Vector3d();
	Vector3d(double x, double y, double z);
	
	void read(std::istream &in);
	void dump(std::ostream &out);
	
	double const& operator[](unsigned i) const;
	Vector3d& Vector3d::operator+=(const Vector3d &v);
	Vector3d& Vector3d::operator-=(const Vector3d &v);
	
	double Vector3d::operator*(const Vector3d &v) const;
	Vector3d& Vector3d::operator*=(double d);
	Vector3d& Vector3d::operator/=(double d);
	const Vector3d Vector3d::operator*(double d) const;
	const Vector3d Vector3d::operator/(double d) const;
	const Vector3d Vector3d::cross(const Vector3d &v) const;
	const Vector3d Vector3d::operator+(const Vector3d &v) const;
	const Vector3d Vector3d::operator-(const Vector3d &v) const;
	bool Vector3d::operator==(const Vector3d &v) const;
	bool Vector3d::operator!=(const Vector3d &v) const;
	Vector3d& Vector3d::operator=(const Vector3d &v);

	double Vector3d::length() const;

	// Returns a normalized copy of this vector (of unit length). An unchanged copy of this is returned if the length is zero.
	Vector3d Vector3d::normalizedCopy() const;	

};

// Non-member functions
const Vector3d operator*(double d, Vector3d &v);
const Vector3d operator/(double d, Vector3d &v);
Vector3d operator+(const Vector3d &v);
Vector3d operator-(const Vector3d &v);


#endif