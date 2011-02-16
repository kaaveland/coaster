#pragma once

#include "Track.h"
#include "Vector3d.h"

class PhysicsCart
{
public:
	PhysicsCart(double* initpos3d, double* initnorm3d);
	~PhysicsCart(void);
	
	void nextStep(double dt);
	void setTrack(Track &track);

	Vector3d getPos3d() const;
	Vector3d getNorm3d() const;



private:
	// Cart "constants"
	double mass;				// Cart mass
	double Ix;					// Moment of inertia
	double friction_static; 	// Static coefficient of friction
	double friction_glide;		// Gliding friction coefficient
	double C_x, C_y, C_z;		// Displacements of CG
	double gravityAccel;

	// Cart properties
	Vector3d pos;				// Position vector, origin is lower back center (is OK??)
	Vector3d norm;				// Normal/"up" vector/z-axis
	double v;					// Track speed
	Vector3d velocity;			// Velocity vector
	Vector3d angularVelocity;	// Angular velocities
	

};

