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

void Track::initValues() {
	this->trackLength = 1;
	this->nPoints = 0;
	this->delta_t = 1;


}

Track::Track(vector<Vector3d> const &pos, vector<Vector3d> const &up)
{
	assert(pos.size() == up.size());
	initValues();

	this->nPoints = pos.size();
	this->pos = pos;
	this->up = up;

	this->delta_t = (double)1 / (double)pos.size();

}

Track::Track(void) {
	this->nPoints = 0;
	this->delta_t = 1;
	this->trackLength = 1.0;
	
	//this->generateTrack();
}

Track::Track(int nPoints) {
	Track();
	this->nPoints = nPoints;
	pos.resize(nPoints);
	up.resize(nPoints);
	this->delta_t = (double)1 / (double)nPoints;
	
	//this->generateTrack();
}

Track::~Track(void)
{
		
}

double Track::getDelta(void) const
{
	return this->delta_t/this->smoothValue;
}

int Track::getSmoothValue(void) const
{
	return this->smoothValue;
}


Vector3d Track::getControlPoint(int index) const
{
	if (index < 0) index = 0;
	else if (index >= nPoints) index = nPoints-1;
	assert(index >= 0 && index < nPoints);

	return pos[index];
}

/*void Track::setTrackPoint(double index, Vector3d v)
{
	assert(index >= 0 && index < nPoints);

	pos[index] = v;
}*/

void Track::setTrackLength(double length) {
	trackLength = length;
}

double Track::getTrackLength() const
{
	return trackLength;
}

Vector3d Track::getControlUp(int index) const
{
	assert(index >= 0 && index < nPoints);
	return Vector3d(0,1,0);

	//return up[index];
}

Vector3d Track::getUp(double t) const
{
	// TODO: interpolate up vectors
	// return this->up[something];
	return Vector3d(0,1,0);
}

void Track::setUp(int index, Vector3d v)
{
	assert(index >= 0 && index < nPoints);
	
	up[index] = v;
}

// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
Vector3d Track::Eq(double t, const Vector3d p1, const Vector3d p2, const Vector3d p3, const Vector3d p4) const
{
	//printf("P1 x:%e y:%e z:%e \n", p3.x, p3.y, p3.z);
	//printf("P2 x:%e y:%e z:%e \n", p3.x, p3.y, p3.z);
	//printf("P3 x:%e y:%e z:%e \n", p3.x, p3.y, p3.z);
	//printf("P4 x:%e y:%e z:%e \n", p3.x, p3.y, p3.z);

    double t2 = t * t;
    double t3 = t2 * t;

    double b1 = 0.5 * (  -t3 + 2*t2 - t);
    double b2 = 0.5 * ( 3*t3 - 5*t2 + 2);
    double b3 = 0.5 * (-3*t3 + 4*t2 + t);
    double b4 = 0.5 * (   t3 -   t2    );

	

	//double x = p1.x*b1 + p2.x*b2 + p3.x*b3 + p4.x*b4;
	//double y = p1.y*b1 + p2.y*b2 + p3.y*b3 + p4.y*b4;
	//double z = p1.z*b1 + p2.z*b2 + p3.z*b3 + p4.z*b4;

	//printf("New x:%e y:%e z:%e \n", x, y, z);

    //return Vector3d(x, y, z); 
	return (p1*b1 + p2*b2 + p3*b3 + p4*b4);
}

void Track::addPos(const Vector3d v)
{
	nPoints += 1;
	delta_t = (double)1 / (double)nPoints;
	//printf("Add point x:%f y:%f z:%f \n", v.x, v.y, v.z);
    pos.push_back(v);
	//printf("Added point x:%f, y:%f, z:%f \n", getTrackPoint(nPoints-1));
}

Vector3d Track::getPos(double t) const
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
	//printf("lt: %f, p: %d, p0:%d, p1:%d, p2:%d, p3:%d \n", lt, p, p0, p1, p2, p3);
	return Track::Eq(lt, getControlPoint(p0), getControlPoint(p1), getControlPoint(p2), getControlPoint(p3));
}

double Track::metersToT(double meters) const 
{
	assert(meters >= 0.0 && meters <= trackLength);
	return meters/trackLength;
}

double Track::getDistance(int index) const
{
	assert(index >= 0 && index < nPoints);

	// Possibly other calculation?
	// return ds*index;
	return -1.0;
}

Vector3d Track::getTangentVector(double index) const
{
	if (index < 0) index = 0;
	else if (index >= 1-getDelta()) index = 1-getDelta();
	assert(index >= 0 && index <= 1);

	// printf("GetTangent t: %f \n", index);


	Vector3d pos0, pos1;
	pos0 = getPos(index);
	pos1 = getPos(index+getDelta());
		
	Vector3d tangent = pos1 - pos0;
	double length = tangent.length();
	if(length != 0){
		tangent /= length;
	}
	
	return tangent;
}

double Track::getCurvature(double index) const
{
	if (index < 0 || index >= 1) return 0.0;
	assert(index >= 0 && index <= 1);

	// printf("GetCurvature t: %f \n", index);

	Vector3d pos0, pos1;
	pos0 = this->getPos(index);
	pos1 = this->getPos(index+getDelta());
	double ds = (pos1 - pos0).length();
	
	Vector3d dT = getTangentVector(index+getDelta()) - getTangentVector(index);
	dT /= ds;

	return dT.length();

}

Vector3d Track::getNormalVector(double index) const
{
	// TODO: assert that normal is not of inf length!
	// assert(index >= 0 && index <= 1);

	// printf("GetNormal t: %f \n", index);

	// We need one point back and one point forward for the normal vector, so on the boundaries we hack.
	if (index <= 0) return getNormalVector(getDelta());
	if (index >= 1) return getNormalVector(1-getDelta());
	
	Vector3d T0, T1;
	T0 = getTangentVector(index-getDelta());
	T1 = getTangentVector(index);
		
	// The normal vector points towards T1-T0
	Vector3d normal = T1 - T0;
	double length = normal.length();
	normal /= length;
		
	return normal;
}


void Track::generateTrack(void)
{

	// Generate test track 2
	// Loop/circle radius 50.0 with center in origo.
	const int nPoints = 1000;
	const double PI = acos(-1.0);

	this->pos.clear();
	this->up.clear();
	pos.resize(nPoints);
	up.resize(nPoints);

	for (int i = 0; i < nPoints; i++) {
		Vector3d p(50.0*cos(1.0*i/nPoints *2*PI - PI/2), 50.0*sin(1.0*i/nPoints *2*PI - PI/2), 0.0);
		up[i] = -p/p.length();
		
		p += Vector3d(0,100,0);	// move up 100 units
		pos[i] = p;
	}
	
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

