#pragma once
#include <vector>
#include "Vector3d.h"

using std::vector;

class Track
{
public:

	// Constructs new Track instance. 
	// 
	//	Parameters:
	//	pos - vector containing the points along the curve
	//  up - vector containing the up vectors along the curve
	//
	Track(vector<Vector3d> const &pos, vector<Vector3d> const &up);

	Track(int nPoints);
	
	~Track(void);
	
	// Gives the coordinates to the point on the track at the given segment, possibly
	// rounded or interpolated between the track's discrete points.
	Vector3d getPos(int index) const;

	// Gives the track's unit up vector at the given segment.
	Vector3d getUp(int index) const;
	
	// Gives the unit tangent vector at the given segment from start.
	Vector3d getTangentVector(int index) const;

	// Gives the unit normal vector at the given segment. The normal vector points 
	// towards the center of curvature.
	Vector3d getNormalVector(int index) const;

	// Calculates the data for a track that is parallel to this instance, offset at
	// the given distance. The new track is offset at the given distance perpendicular both to the Track's up vector
	// and tangent vector (that is, the width direction). Positive values gives an offset along the positive y axis,
	// which is the to the left.
	void getParallelTrack(double offset, Track& track) const;

protected:
	int nPoints;
	vector<Vector3d> pos;
	vector<Vector3d> up;
	//double trackLength, ds;

	// Returns the index of the section where given distance ends up.
	// Range of getSectionIndex is [-1, nSegments]. -1 is returned if the distance is out
	// of track range.
	int inline getSegmentIndex(double distance) const;
	
	// Returns the distance travelled to the start of the given track section.
	double inline getDistance(int index) const;	
};

