#include <cmath>
#include <vector>
#include "Track.h"
#include "Vector3d.h"
#include <cassert>
#include <math.h>
#include <sstream>

#define NULL 0
#define DEBUG

#define VECTORLENGHTMINIMUM 1e-6*1e-6
#define NULLVECTOR Vector3d(0,0,0)

#ifdef DEBUG
#include <iostream>
#endif

void Track::initValues() {
	this->trackLength = 1;
	this->nControlPoints = 0;
	this->delta_t = 1;
	this->smoothingValue = 50;
	
	pos = vector<Vector3d>(0);
	arcDistances = vector<double>(0);
	rotations = vector<double>(0);
		
}

Track::Track(vector<Vector3d> const &pos, vector<double> const &rot)
{
	// Do not use this constructor
	assert(false);

	assert(pos.size() == rot.size());
	initValues();

	this->nControlPoints = pos.size();
	this->pos = pos;
	this->rotations = rot;
	this->arcDistances = vector<double>(nControlPoints);
	//this->section_dS = vector<double>(nControlPoints);

	this->delta_t = (double)1 / (double)pos.size();
	calculateArcDistances();
	//makePlaneUpVectors();
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
	calculateArcDistances();
	//makePlaneUpVectors();
}

void Track::makePlaneUpVectors() 
{
	/*
	for (int i=0; i < nControlPoints; i++) {
		double t = (double)i/nControlPoints;

		// Ensure up is always pointing "upwards"/positive y axis
		// If this gives the wrong intention, set a rotation of pi radians with setRotation()
		if (getNormalVector(t) * Vector3d(0,1,0) >= 0) up[i] = getNormalVector(t);	
		else up[i] = -getNormalVector(t);
	}
	*/
}

void Track::setTrackRotation(int index, double radian)
{
	assert (index >= 0 && index < nControlPoints);
	rotations[index] = radian;
		
	
	//double t = (double)index/1.0;
	//Vector3d right = getTangentVector(t).cross(getUp(t));
	//if (
	//right /= right.length();

	//up[index] = Vector(0,1,0) + right * sin(radian);
	
}
double Track::getTrackRotation(int index)
{
	assert (index >= 0 && index < nControlPoints);
	return rotations[index];
}

double Track::getTrackLength() const
{
	return trackLength;
}

Vector3d Track::getUnitBinormal(double t) const
{
	//assert ((getTangentVector(t).cross(getUp(t))).normalizedCopy() != NULLVECTOR);
	return (getTangentVector(t).cross(getUp(t))).normalizedCopy();
}


Vector3d Track::getUp(double t) const
{	
	//Vector3d vup = getNonNormalizedNormalVector(t);
	//assert (vup.x==vup.x && vup.y==vup.y && vup.z==vup.z); // Ensure is not NAN
	//if (vup.length() < VECTORLENGHTMINIMUM) return Vector3d(0,0,0);
	//return vup;

	 // Find out in which interval we are on the spline
    int p = (int)(t / delta_t);
	double lt = (t - delta_t*(double)p) / delta_t;
	#define BOUNDS2(pp) { if (pp < 0) pp = 0; else if (pp >= (int)rotations.size()-2) pp = rotations.size() - 2; }
	BOUNDS2(p);
	//double angle0 = rotations[p];
	//double angle1 = rotations[p+1];
	//double angleinterpolated = (angle1-angle0)*lt;

	// TODO: if y axis and tangent is almost parallel, we get gimbal lock. FIX!
	Vector3d yaxis(0,1,0);
	Vector3d tangent = getTangentVector(t);
	
	// Find the unit right/binormal vector in the right-hand system (tangent, (0,1,0), right).
	// Note that this will lead to problems if the tangent vector is very close to +- (0,1,0)
	Vector3d right = tangent.cross(yaxis).normalizedCopy();
	Vector3d up = -tangent.cross(right).normalizedCopy();
	
	// Interpolate the angle linearly between this and next point
	double angleInterpolated = rotations[p] + (rotations[p+1]-rotations[p]) * lt;
	up = cos(angleInterpolated)*up + sin(angleInterpolated)*right;
	return up.normalizedCopy();

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
	printf("Pos add n:%d x:%f y:%f, z:%f, rotation: %f\n",nControlPoints, v.x, v.y, v.z, rotation_radians);
	//printf("Add point x:%f y:%f z:%f \n", v.x, v.y, v.z);
    pos.push_back(v);
	rotations.push_back(rotation_radians);

	
	//printf("Added point x:%f, y:%f, z:%f \n", getTrackPoint(nControlPoints-1));
	this->arcDistances.resize(nControlPoints);
	//up.resize(nControlPoints);
	
	this->delta_t = 1.0/pos.size();
	calculateArcDistances();
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

//	assert(distance - arcDistances[searchIndex] >= 0.0);
	double ds_dt = (getPos(current_t+getSmoothedDelta()) - getPos(current_t)).length() / getSmoothedDelta();
	assert (abs(ds_dt) != 0.0);
	double dt = ds / ds_dt;

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
	if (t < getSmoothedDelta()) t = getSmoothedDelta(); 
	else if (t >= 1-getSmoothedDelta()) t = 1-getSmoothedDelta();
	//assert(t >= 0 && t <= 1);
	//printf("GetTangent t: %f \n", t);

	Vector3d pos0, pos1;
	
	pos0 = getPos(t);
	pos1 = getPos(t+getSmoothedDelta());
		
	Vector3d tangent = pos1 - pos0;
	double length = tangent.length();
	//assert (length >= VECTORLENGHTMINIMUM);
	tangent /= length;
	
	return tangent;
}

double Track::getCurvature(double t) const
{
	if (!(t >= 0 && t <= 1)) return 0.0;
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
	
	Vector3d normal = getNonNormalizedNormalVector(t);
	double length = normal.length();
	if (length < VECTORLENGHTMINIMUM) return Vector3d(0,0,0);
	return normal/length;
}

Vector3d Track::getNonNormalizedNormalVector(double t) const
{
	if (t <= 0) t = getSmoothedDelta();
	if (t >= 1) t = 1.0-getSmoothedDelta();
	
	Vector3d T0, T1;
	T0 = getTangentVector(t-getSmoothedDelta());
	T1 = getTangentVector(t+getSmoothedDelta());
		
	// The normal vector points towards T1-T0
	Vector3d normal = T1 - T0;
	return normal;
}


void Track::generateTrack(void)
{
	// Generate test track 2
	// Loop/circle radius 50.0 with center in origo.
	const int nControlPoints = 1000;
	const double PI = acos(-1.0);

	this->pos.clear();
	pos.resize(nControlPoints);

	for (int i = 0; i < nControlPoints; i++) {
		Vector3d p(50.0*cos(1.0*i/nControlPoints *2*PI - PI/2), 50.0*sin(1.0*i/nControlPoints *2*PI - PI/2), 0.0);
		//up[i] = -p/p.length();
		
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

double Track::getArcLengthToControlPoint(double t) const
{
	if (t < 0.0) t = 0.0;
	else if (t > 1.0) t = 1.0;

	return arcDistances[(int)(t*nControlPoints)];
}

int Track::getNumberOfPoints(void) const
{
	return this->nControlPoints;
}

void Track::read(std::istream &in)
{
	enum { BEGIN, CONTROL, POS, ROT, END, ERROR} state = BEGIN;
	char buf[512];

	while (in.getline(buf, 512) && state != END) {
		std::string input(buf);
		std::string trash;
		std::istringstream parse(input);

		if (state == BEGIN && input.substr(0, 3) == "CP:") {
			state = CONTROL;
			parse >> trash; // Skip CP: and space
			parse >> nControlPoints; // Fill in control points
		} else if (state == CONTROL && input.substr(0, 2) == "P:") {
			state = POS; // Skip this line, but expect data on pos on following lines
		} else if (state == POS && input.substr(0, 2) != "R:") {
			Vector3d parsed;
			parsed.read(parse);
			printf("Pos: %f\n", parsed);
			pos.push_back(parsed);
		} else if (state == POS && input.substr(0, 2) == "R:") {
			state = ROT; // Skip this line, but expect data on pos on following lines
		} else if (state == ROT && input.size() > 0) {
			double parsed;
			parse >> parsed;
			rotations.push_back(parsed);
		} else if (state == ROT && input.size() < 1) {
			state = END; // This means we're done
		} else if (state == END)
			continue; // Empty line after up has been parsed, ignore
		else // Can only happen if input doesn't have strict
			state = ERROR; // May want to try to recover or something here
	}

	this->arcDistances = vector<double>(nControlPoints);
	//this->section_dS = vector<double>(nControlPoints);

	this->delta_t = (double)1 / (double)pos.size();
	calculateArcDistances();
}

void Track::dump(std::ostream &out)
{
	out << "CP: " << nControlPoints << std::endl;
	out << "P: " << std::endl;
	for (int i = 0; i < pos.size(); ++i)
		pos[i].dump(out);
	out << "R: " << std::endl;
	for (int i = 0; i < rotations.size(); ++i)
		out << rotations[i] << std::endl;
		
}
