#include <cmath>
#include <vector>
#include "Track.h"
#include "Vector3d.h"
#include <cassert>

#define NULL 0
#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

Track::Track(vector<Vector3d> const &pos, vector<Vector3d> const &up)
{
	assert(pos.size() == up.size());

	this->nPoints = pos.size();
	this->pos = pos;
	this->up = up;

	this->delta_t = (double)1 / (double)pos.size();

}

Track::Track(int nPoints) {
	this->nPoints = nPoints;
	pos.resize(nPoints);
	up.resize(nPoints);
	this->delta_t = (double)1 / (double)nPoints;

	this->generateTrack();
}

Track::~Track(void)
{
		
}

double Track::getDelta(void)
{
	return this->delta_t;
}

int Track::getSmoothValue(void)
{
	return this->smoothValue;
}

Vector3d& Track::getPos(int index)
{
	if (index < 0) index = 0;
	else if (index >= nPoints) index = nPoints-1;
	assert(index >= 0 && index < nPoints);

	return pos[index];
}

void Track::setPos(int index, Vector3d v)
{
	assert(index >= 0 && index < nPoints);

	//TODO: delete old?

	pos[index] = v;
}

Vector3d& Track::getUp(int index)
{
	assert(index >= 0 && index < nPoints);

	return up[index];
}

void Track::setUp(int index, Vector3d v)
{
	assert(index >= 0 && index < nPoints);
	
	//TODO: delete old?

	up[index] = v;
}

// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
Vector3d Track::Eq(double t, const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& p4)
{
    double t2 = t * t;
    double t3 = t2 * t;

    double b1 = .5 * (  -t3 + 2*t2 - t);
    double b2 = .5 * ( 3*t3 - 5*t2 + 2);
    double b3 = .5 * (-3*t3 + 4*t2 + t);
    double b4 = .5 * (   t3 -   t2    );

    return (p1*b1 + p2*b2 + p3*b3 + p4*b4); 
}

void Track::AddSplinePoint(const Vector3d& v)
{
    up.push_back(v);
    delta_t = (double)1 / (double)pos.size();
}

Vector3d Track::getInterpolatedSplinePoint(double t)
{
    // Find out in which interval we are on the spline
    int p = (int)(t / delta_t);
    // Compute local control point indices
#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp >= (int)pos.size()-1) pp = pos.size() - 1; }
    int p0 = p - 1;     BOUNDS(p0);
    int p1 = p;         BOUNDS(p1);
    int p2 = p + 1;     BOUNDS(p2);
    int p3 = p + 2;     BOUNDS(p3);
    // Relative (local) time 
	double lt = (t - delta_t*(double)p) / delta_t;
	// Interpolate
    return Track::Eq(lt, pos[p0], pos[p1], pos[p2], pos[p3]);
}
/*
double Track::getDistance(int index) const
{
	assert(index >= 0 && index < nPoints);

	// Possibly other calculation?
	// return ds*index;
	return -1.0;
}

Vector3d Track::getTangentVector(int index) const
{
	if (index < 0) index = 0;
	else if (index >= nPoints-1) index = nPoints-2;
	assert(index >= 0 && index < nPoints-1);

	Vector3d pos0, pos1;
	pos0 = getPos(index);
	pos1 = getPos(index+1);
		
	Vector3d tangent = pos1 - pos0;
	double length = tangent.length();
	tangent /= length;
	
	return tangent;
}

double Track::getCurvature(int index) const
{
	if (index < 0 || index >= nPoints-1) return 0.0;
	assert(index >= 0 && index < nPoints-1);

	Vector3d pos0, pos1;
	pos0 = getPos(index);
	pos1 = getPos(index+1);
	double ds = (pos1 - pos0).length();
	
	Vector3d dT = getTangentVector(index-1) - getTangentVector(index);
	dT /= ds;

	return dT.length();

}

Vector3d Track::getNormalVector(int index) const
{
	// TODO: assert that normal is not of inf length!
	assert(index >= 0 && index < nPoints);

	// We need one point back and one point forward for the normal vector, so on the boundaries we hack.
	if (index == 0) return getNormalVector(1);
	if (index == nPoints-1) return getNormalVector(nPoints-2);
	
	Vector3d T0, T1;
	T0 = getTangentVector(index-1);
	T1 = getTangentVector(index);
		
	// The normal vector points towards T1-T0
	Vector3d normal = T1 - T0;
	double length = normal.length();
	normal /= length;
	
	return normal;
}
*/
void Track::generateTrack(void)
{

	const double PI = acos(-1.0);
	Vector3d v(0,0,0);
	Vector3d v_0(0,0,0);

	v.x = 0;
	v.z = 20;
	this->setPos(0, v);
	this->setUp(0, v_0);

	v.x = 20;
	v.z = 0;
	this->setPos(1, v);
	this->setUp(0, v_0);

	v.x = 0;
	v.z = -20;
	this->setPos(2, v);
	this->setUp(0, v_0);

	v.x = -20;
	v.y = 20;
	v.z = 0;
	this->setPos(3, v);
	this->setUp(0, v_0);

	v.x = 10;
	v.y = -20;
	v.z = 20;
	this->setPos(4, v);
	this->setUp(0, v_0);

	v.x = 40;
	v.y = -60;
	v.z = 40;
	this->setPos(5, v);
	this->setUp(0, v_0);


	/*
	for (int i = 0; i < this->nPoints; i++) {
		v.x = 5*i;
		v.y = 100;
		v.z = v.z*2;
		this->setPos(i, v);

		this->setUp(i, v_0);
	}
	*/
	
}

/*
void Track::getParallelTrack(double offset, Track &track) const 
{
#ifdef DEBUG
	std::cout << "Generating data for parallel Track...";
#endif

	assert(nPoints == track.nPoints);
	
	Vector3d perpendicularVector, up, tangent;
	
	for (int i = 0; i < nPoints-1; i++) {	
		// Get the up and tangential vector for this section
		up *= getUp(i);
		tangent = getTangentVector(i);
		
		// Calculate the cross product betwen these. This vector will point "to the left".
		perpendicularVector = up.cross(tangent);

		// Divide by current length and multiply by the offset to get correct length
		double length = perpendicularVector.length();
		perpendicularVector *= offset/length;
	
		// Calculate point coordinates for parallel track
		Vector3d newPoint = this->pos[i] + perpendicularVector;

		// Insert new point into array
		track.pos[i] = newPoint;

		// Up, normal and tangent vectors are assumed to be equal to this track. (Correct?)
		track.up[i] = this->up[i];
	}
		
#ifdef DEBUG
	std::cout << "Done\n";
#endif

}
*/

int Track::getNumberOfPoints(void) const
{
	return this->nPoints;
}

