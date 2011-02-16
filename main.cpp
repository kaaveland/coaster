#include <cmath>
#include <iostream>
#include "Vector3d.h"
#include "Track.h"
#include "PhysicsCart.h"

using namespace std;

void printVector(double *v) {
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

double inline vectorDot(double const *v0, double const *v1)
{
	return (v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2]);
}

int main() {
	
	// Generate test track 1 [OK]
	const int nPoints = 1000000;
	const double PI = acos(-1.0);

	double *pointsx = new double[nPoints];
	double *pointsy = new double[nPoints];
	double *pointsz = new double[nPoints];
	double *upx = new double[nPoints];
	double *upy = new double[nPoints];
	double *upz = new double[nPoints];
	for (int i = 0; i < nPoints; i++) {
		pointsx[i] = PI*i/(nPoints-1);
		pointsy[i] = sin(PI * i/(nPoints-1));
		pointsz[i] = 0.0;
		upx[i] = 0.0;
		upy[i] = 0.0;
		upz[i] = 1.0;
	}

	double up[3], tang[3], norm[3], temp[3];
	int index = 1;
		
	// Track tests
	Track track(pointsx, pointsy, pointsz, upx, upy, upz, nPoints, 1.0);
	
	track.getPos3d(index, temp);
	cout << "Pos: "; printVector(temp); cout << endl;
		
	track.getNormalVector3d(index, norm);
	cout << "Normal: "; printVector(norm); cout << endl;

	track.getTangentVector3d(index, tang);
	cout << "Tangent: "; printVector(tang); cout << endl;

	cout << "N dot T: " << vectorDot(norm, tang) << endl;

	track.getUp3d(index, up);
	cout << "Up: "; printVector(up); cout << endl;
	
	// Create parallel track test [OK]
	Track parallel(nPoints);
	track.getParallelTrack(1.0, parallel);

	parallel.getPos3d(index, temp);
	cout << "Pos: "; printVector(temp); cout << endl;
	parallel.getPos3d(0, temp);
	cout << "Pos(i=0): "; printVector(temp); cout << endl;

	parallel.getNormalVector3d(index, norm);
	cout << "Normal: "; printVector(norm); cout << endl;

	parallel.getTangentVector3d(index, tang);
	cout << "Tangent: "; printVector(tang); cout << endl;

	parallel.getUp3d(index, up);
	cout << "Up: "; printVector(up); cout << endl;

	
	// Cart test 1
	double initpos[] = {0,2,0};
	double initup[] = {0,0,1};

	PhysicsCart cart(initpos, initup);
	Vector3d norm0 = cart.getNorm3d();
	
	
	cin.get();

	return 0;
}