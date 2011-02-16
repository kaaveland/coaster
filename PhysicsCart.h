#pragma once

#include "Track.h"
#include "Vector3d.h"

class PhysicsCart
{
public:
	PhysicsCart(Vector3d initpos, Vector3d initup);
	~PhysicsCart(void);
	
	// Tells the simulation to increase the clock by dt, and such calculate updated properties
	// for the cart.
	//
	void nextStep(double dt);

	// Assign a track to this cart. 
	//
	void setTrack(Track *track);

	// Sets the braking factor for the cart.
	//	
	// Parameters:
	// braking - braking factor, ranging from 0.0 to 1.0
	// 
	// Values outside the range [0.0, 1.0] will cause the braking to be set to 0.0 and 1.0, respectively.
	//
	void setBraking(double braking);

	// Sets the thrust factor for the cart.
	//
	// Parameters:
	// thrust - thrust factor ranging from 0.0 to 1.0
	//
	// Values outside the range [0.0, 1.0] will cause the thrust to be set to 0.0 and 1.0, respectively.
	void setThrust(double thrust);

	Vector3d getPos() const;
	Vector3d getUp() const;

private:
	// Cart "constants"
	double mass;				// Cart mass
	double Ix;					// Moment of inertia
	double friction_static; 	// Static coefficient of friction
	double friction_glide;		// Gliding friction coefficient
	double C_x, C_y, C_z;		// Displacements of CG
	double gravityAccel;
	double wheelsOffsety;		// Offset sideways of wheels (along y-axis)
	double wheelsOffsetx;		// Offset of wheels along x axis

	// Cart properties
	Vector3d pos;				// Position vector, origin is lower back center (is OK??)
	Vector3d up;				// Normal/"up" vector/z-axis
	double v;					// Track speed
	Vector3d velocity;			// Velocity vector
	Vector3d angularVelocity;	// Angular velocities
	double brakingFactor;
	double thrustFactor;

	// Simulation properties
	int currentIndex;			// Which Track section index the cart currently is in
	Track *track;				// Current main (center) track
	

};

