#pragma once

#include "Track.h"
#include "Vector3d.h"
#include <string>

using std::string;

class PhysicsCart
{
public:

	PhysicsCart();

	~PhysicsCart(void);
	
	// Tells the simulation to increase the clock by dt, and such calculate updated properties
	// for the cart.
	//
	void nextStep(double dt);

	// Assign a track to this cart. 
	//
	void setTrack(Track *track);

	void moveTo(double distance);

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

	void setSpeed(double v);


	Vector3d getPos() const;
	Vector3d getUp() const;
	Vector3d getForward() const;

	// Debugging/testing functions
	string toString() const;	

	bool hasTrack() const;

private:
	// Cart "constants"
	double mass;				// Cart mass
	double Ix;					// Moment of inertia
	double friction_static; 	// Static coefficient of friction
	double friction_glide;		// Gliding friction coefficient
	double C_x, C_y, C_z;		// Displacements of CG
	
	double maxThrust;			// Maximum forward thrust [force]
	double wheelsOffsety;		// Offset sideways of wheels (along y-axis)
	double wheelsOffsetx;		// Offset of wheels along x axis

	// Cart properties
	Vector3d vPos;				// Position vector, origin is lower back center (is OK??)
	Vector3d vUp;				// Normal/"up" vector/z-axis
	double v;					// Track speed
	Vector3d vVelocity;			// Direction of travel
	Vector3d vAccel;				// Acceleration vector
	Vector3d vAngularVelocity;	// Angular velocities
	double brakingFactor;
	double thrustFactor;
	
	// Simulation "constants"
	Vector3d gvector;

	// Simulation properties
	double currentDistance;			// Which distance the cart has travelled
	double current_t;				// jepp (JEPP igjen)
	
	// Helper functions
	double calculate_a_T(double deltaDistance) const;
	double calculate_a_N(double deltaDistance) const;

	void calculateNextStep(double dT);

	Track *track;					// Current main (center) track
	bool isFreefalling;

	// Debugging
	int step;


};

