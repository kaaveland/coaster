#ifndef __Track_h_
#define __Track_h_

#include <vector>
#include "Vector3d.h"
#include <istream>
#include <ostream>

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
	Track(vector<Vector3d> const &pos, vector<double> const &rot);
	Track(int nPoints);
	Track(void);
	~Track(void);
	
	// Serialization of instance into streams for saving/loading.
	void read(std::istream &in);
	void dump(std::ostream &out);

	// Gives the track's unit up vector at the given t value.
	Vector3d getUp(double t) const;
	
	// Gives the unit tangent vector at the given t value.
	Vector3d getTangentVector(double t) const;

	// Returns the curvature at the given t value.
	double getCurvature(double t) const;

	// Gives the unit normal vector at the given t value. The normal vector points 
	// towards the center of curvature.
	Vector3d getNormalVector(double t) const;

	// Gives the not-normalized normal vector
	Vector3d getNonNormalizedNormalVector(double t) const;

	// Returns the unit binormal vector (right vector) for the given t value.
	Vector3d getUnitBinormal(double t) const;

	// Returns the rotation angle set at the given control point
	double getTrackRotation(int index);
	
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
	Vector3d getPos(double t) const;   // t = 0...1; 0->pos[0] ... 1->pos[pos.size()-1]

	// Convert an arc length movement of ds to the equivalent change in the curve parameter t.
	double deltaDistanceTodeltaT(double ds, double current_t) const;	

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    Vector3d Eq(double t, const Vector3d p1, const Vector3d p2, const Vector3d p3, const Vector3d p4) const;

	// returns the value of the smoothed delta t value--that is, the small step along the curve used for forward differences etc.
	double getSmoothedDelta(void) const;
	int getSmoothingValue(void) const;

	// Returns the arc distance from start to the point of the given track parameter.
	double getArcLengthToControlPoint(double t) const;
	
	double getTrackLength() const;
	Vector3d getControlPoint(int index) const;
	
	// Edit the (already-added) control point for the given index 
	void setControlPoint(int index, Vector3d position);

	// Edti the rotation angle for the (already-added) control point
	void setTrackRotation(int index, double radian);

	// Reset all instance variables to initial state
	void initValues();

protected:
	int nControlPoints;
	std::vector<Vector3d> pos;
	std::vector<double> rotations;
	std::vector<double> arcDistances;	// A vector that contains the (accumulative) arc distances to each of the control points.
	//std::vector<Vector3d> finePoints;
	double delta_t;
	int smoothingValue;
	double trackLength;
			
private:
	void generateTrack();
	void calculateArcDistances();
	void makePlaneUpVectors();
	
	int lastAccessedTrackIndex;
};

#endif // __Track_h_