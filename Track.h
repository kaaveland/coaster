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

	Track(void);
	
	~Track(void);
	
	/* void setTrackPoint(double index, Vector3d v); */

	void setUp(int index, Vector3d v);

	// Gives the track's unit up vector at the given t value.
	Vector3d getUp(double t) const;
	
	// Gives the unit tangent vector at the given segment.
	Vector3d getTangentVector(double t) const;

	// Returns the curvature at the given segment.
	double getCurvature(double t) const;

	// Gives the unit normal vector at the given segment. The normal vector points 
	// towards the center of curvature.
	Vector3d getNormalVector(double t) const;
	// Gives the not-normalized normal vector
	Vector3d getNonNormalizedNormalVector(double t) const;


	// Load and fill the track with points
	/* void generateTrack(void); */

	// Calculates the data for a track that is parallel to this instance, offset at
	// the given distance. The new track is offset at the given distance perpendicular both to the Track's up vector
	// and tangent vector (that is, the width direction). Positive values gives an offset along the positive y axis,
	// which is the to the left.
	void getParallelTrack(double offset, Track& track) const;

	// return nPoints;
	int getNumberOfPoints(void) const;

	 // Operations
    void addPos(const Vector3d v, double rotation_radians);
	Vector3d getPos(double t) const;   // t = 0...1; 0=pos[0] ... 1=pos[max]
	double deltaDistanceTodeltaT(double ds, double current_t) const;	// meters = [0, trackLength];

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    Vector3d Eq(double t, const Vector3d p1, const Vector3d p2, const Vector3d p3, const Vector3d p4) const;

	double getSmoothedDelta(void) const;
	int getSmoothingValue(void) const;

	// Returns the arc distance from start to the point of the given track parameter.
	//double getDistanceTo(double t) const;	
	//double getSection_dS(double t) const;
	double getArcLengthToControlPoint(double t) const;
	
	// Set the (dimensional/in meters) length of the track
	// void setTrackLength(double length);
	double getTrackLength() const;
	Vector3d getControlPoint(int index) const;
	Vector3d getControlUp(int index) const;
	void setControlPoint(int index, Vector3d position);
	void setTrackRotation(int index, double radian);

protected:
	int nControlPoints;
	std::vector<Vector3d> pos;
	std::vector<Vector3d> up;
	std::vector<double> arcDistances;	// A vector that contains the (accumulative) arc distances to each of the control points.
	//std::vector<double> section_dS;		// A vector that contains the dS values for each segment
	//std::vector<Vector3d> finePoints;
	double delta_t;
	int smoothingValue;
	double trackLength;
	//double trackLength, ds;

	// returns vector at distance [0,1]
	//Vector3d inline getVectorAt(double distance) const;
		
private:
	void generateTrack();
	void initValues();
	void calculateArcDistances();
	void makePlaneUpVectors();
	
	int lastAccessedTrackIndex;
	
};

#endif // __Track_h_