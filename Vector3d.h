#ifndef VECTOR3D_H
#define VECTOR3D_H

struct Vector3d {

	double x, y, z;

	Vector3d();
	Vector3d(double x, double y, double z);

	double const& operator[](unsigned i) const;
	Vector3d& Vector3d::operator+=(const Vector3d &v);
	Vector3d& Vector3d::operator-=(const Vector3d &v);
	Vector3d& Vector3d::operator*=(double d);
	Vector3d& Vector3d::operator/=(double );

	const Vector3d Vector3d::operator+(const Vector3d &v) const;
	const Vector3d Vector3d::operator-(const Vector3d &v) const;
	bool Vector3d::operator==(const Vector3d &v) const;
	bool Vector3d::operator!=(const Vector3d &v) const;
	Vector3d& Vector3d::operator=(const Vector3d &v);

};

#endif