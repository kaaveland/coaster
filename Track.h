#pragma once
class Track
{
public:

	// Constructs new Track instance. 
	// 
	//	Parameters:
	//	pointsx, pointsy, pointsz - pointers to arrays of the coordinates to the discrete points along the track.
	//  upx, upy, upz - pointers to arrays of "up" vectors along the track
	//  nSegments - number of segments, that is, the number of points will be nSegments-1
	//  length - total length (distance) of track from beginning to start
	Track(double *pointsx, double *pointsy, 
		double *pointsz, double *upx, double *upy, double *upz, 
		int nSegments, double length);
	
	~Track(void);
	
	// Gives the coordinates to the point on the track at the given distance from start, possibly
	// rounded or interpolated between the track's discrete points.
	void getPos3d(double s, double* pos3d) const;

	// Gives the track's unit up vector at the given distance from start.
	void getUp3d(double s, double* up3d) const;
	
	// Gives the unit tangent vector at the given segment from start.
	void getTangentVector3d(int segment, double* tangent3d) const;

	// Gives the unit normal vector at the given segment. The normal vector points 
	// towards the center of curvature.
	void getNormalVector3d(int segment, double* normal3d) const;

protected:
	int nSegments;
	double *pointsx, *pointsy, *pointsz;
	double *upx, *upy, *upz;
	double trackLength, ds;
	
	// Returns the index of the section where given distance ends up.
	// Range of getSectionIndex is [-1, nSegments]. -1 is returned if the distance is out
	// of track range.
	int inline getSectionIndex(double distance) const;

	
};

