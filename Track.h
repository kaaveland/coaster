#ifndef __Track_h_
#define __Track_h_

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

	void setPos(int index, Vector3d v);

	// Gives the track's unit up vector at the given segment.
	Vector3d getUp(int index) const;

	void setUp(int index, Vector3d v);
	
	// Gives the unit tangent vector at the given segment.
	Vector3d getTangentVector(int index) const;

	// Returns the curvature at the given segment.
	double getCurvature(int index) const;

	// Gives the unit normal vector at the given segment. The normal vector points 
	// towards the center of curvature.
	Vector3d getNormalVector(int index) const;


	// Load and fill the track with points
	void generateTrack(void);

	// Calculates the data for a track that is parallel to this instance, offset at
	// the given distance. The new track is offset at the given distance perpendicular both to the Track's up vector
	// and tangent vector (that is, the width direction). Positive values gives an offset along the positive y axis,
	// which is the to the left.
	void getParallelTrack(double offset, Track& track) const;

	// return nPoints;
	int getNumberOfPoints(void) const;

	 // Operations
    void AddSplinePoint(const Vector3d& v);
	Vector3d getInterpolatedSplinePoint(double t);   // t = 0...1; 0=pos[0] ... 1=pos[max]

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    static Vector3d Eq(double t, const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& p4);

	double getDelta(void) const;
	int getSmoothValue(void) const;

protected:
	int nPoints;
	std::vector<Vector3d> pos;
	std::vector<Vector3d> up;
	double delta_t;
	static int const smoothValue = 50;
	//double trackLength, ds;

	// returns vector at distance [0,1]
	Vector3d inline getVectorAt(double distance) const;
	
	// Returns the distance travelled to the start of the given track section.
	double inline getDistance(int index) const;	
	
};

#endif // __Track_h_