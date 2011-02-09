#pragma once
class PhysicsCart
{
public:
	PhysicsCart(double* initpos3d, double* initnorm3d);
	~PhysicsCart(void);
	void nextStep(double dt);
	
	double* getPos3d();
	double* getNorm3d();

	void setTrack(double *x, double *y, double *z,
					double *normx, double *normy, double *normz,
					int nElements, double trackLength);


private:
	// Cart "constants"
	double mass;				// Cart mass
	double Ix;					// Moment of inertia
	double friction_static; 	// Static coefficient of friction
	double friction_glide;		// Gliding friction coefficient
	
	// Cart properties
	double *pos3d;		// Position vector
	double *norm3d;		// Normal/"up" vector
	double v;			// Track speed
	double omegax;		// Angular velocity


	// Track properties
	int nPoints;									// Number of discrete points along track
	double trackLength;								// Track length
	double *trackposx, *trackposy, trackposz;		// Coordinates to point along track
	double *tracknormx, *tracknormy, tracknormz;	// Coordinates to normal/"up" vectors along track


};

