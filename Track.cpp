#include <cmath>
#include <vector>
#include "Track.h"
#include "Vector3d.h"
#include <cassert>
#include <math.h>

#define NULL 0
#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

void Track::initValues() {
	this->trackLength = 1;
	this->nControlPoints = 0;
	this->delta_t = 1;
	this->smoothingValue = 50;
	
	pos = vector<Vector3d>(0);
	up = vector<Vector3d>(0);
	arcDistances = vector<double>(0);
		
}

Track::Track(vector<Vector3d> const &pos, vector<Vector3d> const &up)
{
	assert(pos.size() == up.size());
	initValues();

	this->nControlPoints = pos.size();
	this->pos = pos;
	this->up = up;
	this->arcDistances = vector<double>(nControlPoints);
	//this->section_dS = vector<double>(nControlPoints);

	this->delta_t = (double)1 / (double)pos.size();
	calculateArcDistances();
	makePlaneUpVectors();
	//	calculateSections_dS();

}

Track::Track(void) {
	initValues();

}

/*Track::Track(int nControlPoints) {
	Track();
	this->nControlPoints = nControlPoints;
	pos.resize(nControlPoints);
	up.resize(nControlPoints);
	this->delta_t = (double)1 / (double)nControlPoints;
	
	//this->generateTrack();
}*/

Track::~Track(void)
{
		
}

double Track::getSmoothedDelta(void) const
{
	return this->delta_t/this->smoothingValue;
}

int Track::getSmoothingValue(void) const
{
	return this->smoothingValue;
}


Vector3d Track::getControlPoint(int index) const
{
	if (index < 0) index = 0;
	else if (index >= nControlPoints) index = nControlPoints-1;
	assert(index >= 0 && index < nControlPoints);

	return pos[index];
}

void Track::setControlPoint(int index, Vector3d position)
{
	if (index < 0) index = 0;
	else if (index >= nControlPoints) index = nControlPoints-1;
	assert(index >= 0 && index < nControlPoints);

	pos[index] = position;
}


Vector3d Track::getControlUp(int index) const
{
	if (index < 0) index = 0;
	else if (index >= nControlPoints) index = nControlPoints-1;
	assert(index >= 0 && index < nControlPoints);

	return up[index];
}

void Track::makePlaneUpVectors() 
{
	for (int i=0; i < nControlPoints; i++) {
		double t = (double)i/nControlPoints;
		up[i] = getNormalVector(t);
	}
}

void Track::setTrackRotation(int index, double delta_radian)
{
	assert (index >= 0 && index < nControlPoints);
	double t = 1.0/index;
	Vector3d right = getTangentVector(t).cross(getUp(t));
	right /= right.length();

	up[index] = up[index] + right * sin(delta_radian);
	
}
/*void Track::setTrackPoint(double index, Vector3d v)
{
	assert(index >= 0 && index < nControlPoints);

	pos[index] = v;
}*/

/*void Track::setTrackLength(double length) {
	trackLength = length;
}*/

double Track::getTrackLength() const
{
	return trackLength;
}


Vector3d Track::getUp(double t) const
{
	 // Find out in which interval we are on the spline
    int p = (int)(t / delta_t);
    // Compute local control point indices
#define BOUNDS2(pp) { if (pp < 0) pp = 0; else if (pp >= (int)up.size()-1) pp = up.size() - 1; }
    int p0 = p - 1;     BOUNDS2(p0);
    int p1 = p;         BOUNDS2(p1);
    int p2 = p + 1;     BOUNDS2(p2);
    int p3 = p + 2;     BOUNDS2(p3);
    // Relative (local) time 
	double lt = (t - delta_t*(double)p) / delta_t;
	// Interpolate
	//printf("lt: %f, p: %d, p0:%d, p1:%d, p2:%d, p3:%d \n", lt, p, p0, p1, p2, p3);

	Vector3d upPos = Track::Eq(lt, getControlPoint(p0)+getControlUp(p0), getControlPoint(p1)+getControlUp(p1), getControlPoint(p2)+getControlUp(p2), getControlPoint(p3)+getControlUp(p3));
	
	Vector3d diff = upPos - getPos(t);
	return diff/diff.length();
	
}

void Track::setUp(int index, Vector3d up)
{
	assert(index >= 0 && index < nControlPoints);
	this->up[index] = up;		
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

void Track::addPos(const Vector3d v, double rotation_radians)
{
	nControlPoints += 1;
	//printf("Add point x:%f y:%f z:%f \n", v.x, v.y, v.z);
    this->pos.push_back(v);
	
	//printf("Added point x:%f, y:%f, z:%f \n", getTrackPoint(nControlPoints-1));
	this->arcDistances.resize(nControlPoints);
	up.resize(nControlPoints);

	this->delta_t = 1.0/pos.size();
	calculateArcDistances();
	makePlaneUpVectors();
	//setTrackRotation(nControlPoints-1, rotation_radians);

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

double Track::deltaDistanceTodeltaT(double ds, double current_t) const 
{
	// Check bounds
	if (current_t < 0.0) return 0.0;
	if (current_t > 1.0) return 0.0;

	//bool searchForward = (distance >= arcDistances[lastAccessedTrackIndex]);
	//int searchIndex = lastAccessedTrackIndex;

	// Find the segment that meters will "land on"
	/*bool found = false;
	while (!found) {
		if (searchForward) {
			if (arcDistances[searchIndex+1] > distance)
				found = true;
			else 
				searchIndex += searchForward;
		}
		else {
			if (arcDistances[searchIndex-1] < distance)
				found = true;
			else
				searchIndex--;
			
		}
	}*/

//	assert(distance - arcDistances[searchIndex] >= 0.0);
	double ds_dt = (getPos(current_t+getSmoothedDelta()) - getPos(current_t)).length() / getSmoothedDelta();
	double dt = ds / ds_dt;
		//(double)searchIndex / (double)nControlPoints + 
		//delta_t * (distance-arcDistances[searchIndex]) / (arcDistances[searchIndex+1]-arcDistances[searchIndex]);

	return dt;
}

//double Track::getDistanceTo(double t) const
//{
//	assert(false);
//	return 0.0;
//
//	assert(t >= 0.0 && t <= 1.0);
//	
//	int index = (int)(t*nControlPoints);
//	double start_t = index * delta_t;
//	double local_t = t - start_t;
//
//	if (index == nControlPoints-1) return arcDistances[nControlPoints-1];
//
//	double distance = arcDistances[index];		// This is the arc distance to the start of the segment that contains the point with parameter t
//	distance += (arcDistances[index+1]-arcDistances[index]) * (local_t/delta_t);	// Linear interpolation between this and next control point
//
//	return distance;
//}

Vector3d Track::getTangentVector(double t) const
{
	if (t < 0) t = 0; 
	else if (t >= 1-getSmoothedDelta()) t = 1-getSmoothedDelta();
	assert(t >= 0 && t <= 1);


	//printf("GetTangent t: %f \n", t);

	Vector3d pos0, pos1;
	pos0 = getPos(t);
	pos1 = getPos(t+getSmoothedDelta());
		
	Vector3d tangent = pos1 - pos0;
	double length = tangent.length();
	if(length != 0){
		tangent /= length;
	}
	
	return tangent;
}

double Track::getCurvature(double t) const
{
	if (t < 0 || t >= 1) return 0.0;
	assert(t >= 0 && t <= 1);

	// printf("GetCurvature t: %f \n", t);

	Vector3d pos0, pos1;
	pos0 = this->getPos(t);
	pos1 = this->getPos(t+getSmoothedDelta());
	double ds = (pos1 - pos0).length();
	
	Vector3d dT = getTangentVector(t+getSmoothedDelta()) - getTangentVector(t);
	dT /= ds;

	return dT.length();

}

Vector3d Track::getNormalVector(double t) const
{
	// TODO: assert that normal is not of inf length!
	// assert(t >= 0 && t <= 1);

	// printf("GetNormal t: %f \n", t);

	// We need one point back and one point forward for the normal vector, so on the boundaries we hack.
	if (t <= 0) return getNormalVector(getSmoothedDelta());
	if (t >= 1) return getNormalVector(1-getSmoothedDelta());
	
	Vector3d T0, T1;
	T0 = getTangentVector(t-getSmoothedDelta());
	T1 = getTangentVector(t);
		
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
	const int nControlPoints = 1000;
	const double PI = acos(-1.0);

	this->pos.clear();
	this->up.clear();
	pos.resize(nControlPoints);
	up.resize(nControlPoints);

	for (int i = 0; i < nControlPoints; i++) {
		Vector3d p(50.0*cos(1.0*i/nControlPoints *2*PI - PI/2), 50.0*sin(1.0*i/nControlPoints *2*PI - PI/2), 0.0);
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

	assert(nControlPoints == track.nControlPoints);
	
	Vector3d perpendicularVector, up, tangent;
	
	for (int i = 0; i < nControlPoints-1; i++) {	
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

void Track::calculateArcDistances()
{
	assert(arcDistances.size() == nControlPoints);
	
	if (nControlPoints == 0) return;

	arcDistances[0] = 0.0;
	for (int i=0; i<nControlPoints-1; i++) {
		
		arcDistances[i+1] = arcDistances[i];
		double tstart = (double)i/(double)nControlPoints;

		// Step throug this segment and accumulate arc lengths
		for (double dt = 0.0; dt < delta_t; dt += delta_t/smoothingValue)
			arcDistances[i+1] += (getPos(tstart + dt + delta_t/smoothingValue) - getPos(tstart + dt)).length(); 
	}

	this->trackLength = arcDistances[nControlPoints-1];
}

//void Track::calculateSections_dS() {
//	assert (section_dS.size() == nControlPoints);
//	if (nControlPoints == 4) {
//		int asd = 0;
//	}
//	if (nControlPoints == 0) return;
//
//	for (int i=0; i < nControlPoints - 1; i++) {
//		section_dS[i] = (arcDistances[i+1]-arcDistances[i])/delta_t;
//	}
//	section_dS[nControlPoints-1] = 0.0;
//}


double Track::getArcLengthToControlPoint(double t) const
{
	if (t < 0.0) t = 0.0;
	else if (t > 1.0) t = 1.0;

	return arcDistances[(int)(t*nControlPoints)];
}

/*double Track::getSection_dS(double t) const 
{
	return section_dS[(int)(nControlPoints*t)];
}*/

int Track::getNumberOfPoints(void) const
{
	return this->nControlPoints;
}
