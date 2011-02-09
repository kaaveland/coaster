#include "Track.h"


Track::Track(double *pointsx, double *pointsy, double *pointsz, double *normx, double *normy, double *normz, int nPoints, double length)
{
	this->pointsx = pointsx;
	this->pointsy = pointsy;
	this->pointsz = pointsz;
	this->normx = normx;
	this->normy = normy;
	this->normz = normz;
	this->nPoints = nPoints;
	this->length = length;
}


Track::~Track(void)
{
	// Delete the vectors containing track data
	delete [] pointsx;
	delete [] pointsy;
	delete [] pointsz;
	delete [] normx;
	delete [] normy;
	delete [] normz;

}

double* Track::getPos3d(double s) const
{

}
double* Track::getNorm3d(double s) const
{

}

