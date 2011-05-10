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
	
	// Tells the simulation to increase the time by dt, and such calculate updated properties
	// for the cart.
	void nextStep(double dt);

	// Assign a track to this cart. 
	//
	void setTrack(Track *track);

	// Move the cart to the given distance along its track. Positioning is approximate.
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
	double getThrust(void);

	// Set the cart's track speed.
	void setSpeed(double v);
	double getSpeed(void);

	// Returns the current (positive) g factor. A factor of 0 means the cart is free-falling, a factor of 1.0 means 
	// constant velocity or at rest.
	double getGFactor() const;

	// Returns true if the cart is free-falling, false if the cart is on the track.
	bool isFreeFalling() const;

	// Returns the cart's current position and orientation
	Vector3d getPos() const;
	Vector3d getUp() const;
	Vector3d getForward() const;

	// Returns the cart's current value of the track's curve parameter, t.
	double getCurrentT(void) const;
	
	// Debugging/testing functions
	string toString() const;	

	// Returns true if the cart has a track set.
	bool hasTrack() const;

private:
	// Cart "constants"
	double mass;				// Cart mass
	double Ix;					// Moment of inertia
	double friction_static; 	// Static coefficient of friction
	double friction_glide;		// Gliding friction coefficient
	double C_x, C_y, C_z;		// Displacements of CG
	
	double maxThrust;			// Maximum forward thrust [force]
	double maxBreak;			// Maximum braking [force]
	double wheelsOffsety;		// Offset sideways of wheels (along y-axis)
	double wheelsOffsetx;		// Offset of wheels along x axis

	// Cart properties
	Vector3d vPos;				// Position vector, origin is lower back center (is OK??)
	Vector3d vUp;				// Normal/"up" vector/z-axis
	Vector3d vForward;
	double v;					// Track speed
	Vector3d vVelocity;			// Direction of travel
	Vector3d vAngularVelocity;	// Angular velocities
	
	double brakingFactor;
	double thrustFactor;
	double airResistanceFactor;
	double airResistanceExponent;
	
	// Simulation "constants"
	const Vector3d gvector;

	// Simulation properties
	double currentDistance;			// Which distance the cart has travelled
	double current_t;				// The cart's current t value on the track
	
	// Helper functions
	double calculate_a_T(double deltaDistance) const;
	double calculate_a_N(double deltaDistance) const;

	void calculateNextStep(double dT);

	Track *track;					// Current main (center) track
	bool isFreefalling;
};

