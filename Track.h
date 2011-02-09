#pragma once
class Track
{
public:
	Track(double *pointsx, double *pointsy, double *pointsz, double *normx, double *normy, double *normz, int nPoints, double length);
	~Track(void);

	double* getPos3d(double s) const;
	double* getNorm3d(double s) const;

private:
	int nPoints;
	double *pointsx, *pointsy, *pointsz;
	double *normx, *normy, *normz;
	double length;
};

