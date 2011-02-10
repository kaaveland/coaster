#include <cmath>
#include "Track.h"

#define NULL 0

// Calculates the vector difference v1-v0 and returns the length of the resulting vector
double inline vectorDiff(double const *v1, double const *v0, double *diff)
{
	double dx = v1[0]-v0[0];	// dx
	double dy = v1[1]-v0[1];	// dy
	double dz = v1[2]-v0[2];	// dz
	
	diff[0] = dx;
	diff[1] = dy;
	diff[2] = dz;

	return sqrt( pow(dx,2) + pow(dy,2) + pow(dz,2) );

}


Track::Track(double *pointsx, double *pointsy, double *pointsz, double *upx, double *upy, double *upz, int nSegments, double trackLength)
{
	this->pointsx = pointsx;
	this->pointsy = pointsy;
	this->pointsz = pointsz;
	this->upx = upx;
	this->upy = upy;
	this->upz = upz;
	this->nSegments = nSegments;
	this->trackLength = trackLength;

	ds = trackLength/nSegments;	// The distance between each point
}


Track::~Track(void)
{
	
}

void Track::getPos3d(double s, double *pos3d) const
{
	int index = getSectionIndex(s);
	pos3d[0] = pointsx[index];
	pos3d[1] = pointsy[index];
	pos3d[2] = pointsz[index];
}

void Track::getUp3d(double s, double *up3d) const
{
	int index = getSectionIndex(s);
	up3d[0] = upx[index];
	up3d[1] = upy[index];
	up3d[2] = upz[index];
}

int inline Track::getSectionIndex(double distance) const 
{
	// TODO: Possibility for interpolation if neccessary?
	
	int index = (int)(distance/ds * nSegments);
	if (index > nSegments || index < 0) return -1;	// The distance lies before or after the track

	return index;
}

void Track::getTangentVector3d(int segment, double *tangent3d) const
{
	double *pos0 = NULL, *pos1 = NULL;
	getPos3d(segment, pos0);
	getPos3d(segment + 1, pos1);
		
	double diff[3];
	double length = vectorDiff(pos1, pos0, diff); 

	tangent3d[0] = diff[0]/length;
	tangent3d[1] = diff[1]/length;
	tangent3d[2] = diff[2]/length;
}

void Track::getNormalVector3d(int segment, double* normal3d) const
{
	double *T0 = NULL, *T1 = NULL;
	getTangentVector3d(segment, T0);
	getTangentVector3d(segment+1, T1);
		
	double diff[3];
	// The normal vector points towards T1-T0
	double length = vectorDiff(T1, T0, diff); 

	normal3d[0] = diff[0]/length;
	normal3d[1] = diff[1]/length;
	normal3d[2] = diff[2]/length;
}
