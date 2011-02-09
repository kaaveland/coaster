#include "Track.h"


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
	// Delete the vectors containing track data
	delete [] pointsx;
	delete [] pointsy;
	delete [] pointsz;
	delete [] upx;
	delete [] upy;
	delete [] upz;

}

void Track::getPos3d(double s, double* const pos3d) const
{
	
}

void Track::getUp3d(double s, double* const up3d) const
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

void Track::getTangentVector3d(double s, double* const tangent3d) const
{
	// Note: rounding might become a problem?
	

}