#pragma once
class Track
{
public:

	// Constructs new Track instance. 
	// 
	//	Parameters:
	//	pointsx, pointsy, pointsz - pointers to arrays of the coordinates to the discrete points along the track.
	//  upx, upy, upz - pointers to arrays of "up" vectors along the track
	//  nSegments - number of segments, that is, the number of points will be nSegments+1
	//  length - total length (distance) of track from beginning to start
	//
	//  On destruction, these vectors will be deleted - so they must be dynamically allocated
	Track(double *pointsx, double *pointsy, 
		double *pointsz, double *upx, double *upy, double *upz, 
		int nPoints, double length);
	
	// Consructs a new Track instance with empty data for the given number of segments.
	//
	// Parameters:
	// nSegments - number of segments, see the above constructor.
	Track(int nPoints);

	~Track(void);
	
	// Gives the coordinates to the point on the track at the given segment, possibly
	// rounded or interpolated between the track's discrete points.
	void getPos3d(int index, double* pos3d) const;

	// Gives the track's unit up vector at the given segment.
	void getUp3d(int index, double* up3d) const;
	
	// Gives the unit tangent vector at the given segment from start.
	void getTangentVector3d(int index, double* tangent3d) const;

	// Gives the unit normal vector at the given segment. The normal vector points 
	// towards the center of curvature.
	void getNormalVector3d(int index, double* normal3d) const;

	// Calculates the data for a track that is parallel to this instance, offset at
	// the given distance. The new track is offset at the given distance perpendicular both to the Track's up vector
	// and tangent vector (that is, the width direction). Positive values gives an offset along the positive y axis,
	// which is the to the left.
	void getParallelTrack(double offset, Track& track) const;

protected:
	// Number of segments ( =number of points-1 )
	int nPoints;
	double *pointsx, *pointsy, *pointsz;
	double *upx, *upy, *upz;
	double trackLength, ds;

	// True if the track data is allocated dynamically
	bool isAllocatedDynamically;
	
	// Returns the index of the section where given distance ends up.
	// Range of getSectionIndex is [-1, nSegments]. -1 is returned if the distance is out
	// of track range.
	int inline getSegmentIndex(double distance) const;
	
	// Returns the distance travelled to the start of the given track section.
	double inline getDistance(int index) const;

	
	
};

