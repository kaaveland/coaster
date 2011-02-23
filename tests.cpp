#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
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

void test1() {

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

	cout << "\nEND TRACK TESTS\n";
	
	// Cart test 1
	
	PhysicsCart cart;
	cart.setTrack(&track);
	cart.setTrackIndex(nPoints/2+10);
	for (int i=0; i < 100; i++) {
		cart.nextStep(0.01);
		printVector(cart.getPos());
		cin.get();
	}
	
		
	cin.get();


}

void testVector3d() {
	Vector3d x(1,0,0);
	Vector3d y(0,1,0);
	cout << "Dot: " << x*y << endl;
	cout << "Cross: "; printVector(x.cross(y));
	cout << "\nSum: "; printVector(x+y);
	cout << "\nDiff: "; printVector(x-y);
	cout << "\nx * 2.5 = "; printVector(x * 2.5);
	cout << "\n2.5 * x = "; printVector(2.5 * x);
	cin.get();
	
}