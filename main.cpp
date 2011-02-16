#include <cmath>
#include <iostream>
#include <vector>
#include "Vector3d.h"
#include "Track.h"
#include "PhysicsCart.h"

using namespace std;

void printVector(Vector3d v) {
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

double inline vectorDot(Vector3d v0, Vector3d v1)
{
	return (v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2]);
}

int main() {
	
	// Generate test track 1 [OK]
	const int nPoints = 1000;
	const double PI = acos(-1.0);


	vector<Vector3d> points;
	points.resize(nPoints);
	vector<Vector3d> up;
	up.resize(nPoints);

	for (int i = 0; i < nPoints; i++) {
		points[i].x = PI*i/(nPoints-1);
		points[i].y = sin(PI* 1.0*i/(nPoints-1));
		points[i].z = 0.0;
		up[i].x = 0.0;
		up[i].y = 0.0;
		up[i].z = 1.0;
	}

	int index = 1;//nPoints/2;
		
	// Track tests
	Track track(points, up);
	Vector3d temp, norm, tang;
	
	temp = track.getPos(index);
	cout << "Pos: "; printVector(temp); cout << endl;
		
	norm = track.getNormalVector(index);
	cout << "Normal: ";  printVector(norm); cout << endl;

	tang = track.getTangentVector(index);
	cout << "Tangent: ";  printVector(tang);; cout << endl;
	cout << "Curvature: " << track.getCurvature(index) << endl;

	cout << "N dot T: " << vectorDot(norm, tang) << endl;

	temp = track.getUp(index);
	cout << "Up: "; printVector(temp); cout << endl;
	
	// Create parallel track test [OK]
	Track parallel(nPoints);
	track.getParallelTrack(1.0, parallel);

	temp = parallel.getPos(index);
	cout << "Pos: "; printVector(temp); cout << endl;
	temp = parallel.getPos(0);
	cout << "Pos(i=0): "; printVector(temp); cout << endl;

	temp = parallel.getNormalVector(index);
	cout << "Normal: "; printVector(temp); cout << endl;

	temp = parallel.getTangentVector(index);
	cout << "Tangent: "; printVector(temp); cout << endl;
	cout << "Curvature: " << track.getCurvature(index) << endl;

	temp = parallel.getUp(index);
	cout << "Up: "; printVector(temp); cout << endl;

	
	// Cart test 1
	Vector3d initpos = track.getPos(0);
	Vector3d initup = track.getUp(0);

	PhysicsCart cart(initpos, initup);
		
	cin.get();
//
	return 0;
}