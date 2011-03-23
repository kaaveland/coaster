#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "Vector3d.h"
#include "Track.h"
#include "PhysicsCart.h"

using namespace std;

void static printVector(Vector3d v) {
	cout << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
}

double static inline vectorDot(Vector3d v0, Vector3d v1)
{
	return (v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2]);
}

static void test4() {

	vector<Vector3d> points;
	points.push_back(Vector3d(0,0,0));
	points.push_back(Vector3d(0.4,0,0));
	points.push_back(Vector3d(0.8,0,0));
	points.push_back(Vector3d(1.0,0,0));
	Track track(points, points);
	Vector3d punkt = track.getPos(0.5);
	printf("%lf %lf %lf\n", punkt.x, punkt.y, punkt.z);
	
	char c;
	cin >> c;
	system("pause");
}

static Track test3() {

	vector<Vector3d> points(6);
	points.clear();

	points.push_back(Vector3d(0,400,0));
	points.push_back(Vector3d(50,320,10));
	points.push_back(Vector3d(200,120,40));
	points.push_back(Vector3d(300,150,50));
	points.push_back(Vector3d(450,240,90));
	points.push_back(Vector3d(400,410,200));

	vector<Vector3d> up(6);
	up.clear();

	for (int i=0; i <= 5; i++) up.push_back(Vector3d(0,1,0));
	

	return Track(points, up);
}

static Track* test1() {

	// Generate test track 1 [OK]
	// Half-sine wave.
	
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
	Track* track = new Track(points, up);
	Vector3d temp, norm, tang;
	
	temp = track->getPos(index);
	cout << "Pos: "; printVector(temp); cout << endl;
		
	norm = track->getNormalVector(index);
	cout << "Normal: ";  printVector(norm); cout << endl;

	tang = track->getTangentVector(index);
	cout << "Tangent: ";  printVector(tang);; cout << endl;
	cout << "Curvature: " << track->getCurvature(index) << endl;

	cout << "N dot T: " << vectorDot(norm, tang) << endl;

	temp = track->getUp(index);
	cout << "Up: "; printVector(temp); cout << endl;

	return track;
	
	// Create parallel track test [OK]
	/* Track parallel(nPoints);
	track->getParallelTrack(1->0, parallel);

	temp = parallel->getPos(index);
	cout << "Pos: "; printVector(temp); cout << endl;
	temp = parallel->getPos(0);
	cout << "Pos(i=0): "; printVector(temp); cout << endl;

	temp = parallel->getNormalVector(index);
	cout << "Normal: "; printVector(temp); cout << endl;

	temp = parallel->getTangentVector(index);
	cout << "Tangent: "; printVector(temp); cout << endl;
	cout << "Curvature: " << track->getCurvature(index) << endl;

	temp = parallel->getUp(index);
	cout << "Up: "; printVector(temp); cout << endl;

	cout << "\nEND TRACK TESTS\n";
	*/

	// Cart test 1
	
//	PhysicsCart cart;
	//cart.setTrack(&track);

	
	//cart.moveTo(0.0);
	//cart.setSpeed(4.0);
	
	
	/*for (int i=0; i < 1000; i++) {	// 10 seconds of simulation

		cart.nextStep(0.01);
		cout << cart.toString() << endl;
		//char c;
		//cin.get(c);
		//if (tolower(c) == 'r') goto restart;
		//else if (tolower(c) == 'q') return;
	}*/
	
		
	//cin.get();


}

static void test2(Track &track) {
	// Generate test track 2
	// Loop/circle radius 1.0 with center in origo.
	const int nPoints = 1000;
	const double PI = acos(-1.0);

	vector<Vector3d> points;
	points.resize(nPoints);
	vector<Vector3d> up;
	up.resize(nPoints);

	for (int i = 0; i < nPoints; i++) {
		Vector3d pos(1.0*cos(1.0*i/nPoints *2*PI - PI/2), 1.0*sin(1.0*i/nPoints *2*PI - PI/2), 0.0);
		points[i] = pos;
		up[i] = -pos;
	}

	double index = 0.0;//nPoints/2;
		
	// Track tests
	track = Track(points, up);
	Vector3d temp, norm, tang;
	
	temp = track.getPos(index);
	cout << "Pos: "; printVector(temp); cout << endl;
		
	norm = track.getNormalVector(index);
	cout << "Normal: ";  printVector(norm); cout << endl;

	tang = track.getTangentVector(index);
	cout << "Tangent: ";  printVector(tang);; cout << endl;
	cout << "Curvature: " << track.getCurvature(index) << endl;

	cout << "N dot T: " << vectorDot(norm, tang) << endl;

	temp = track.getUp((int)index);
	cout << "Up: "; printVector(temp); cout << endl;
	
	
	// Cart test 1
	
	PhysicsCart cart;
	cart.setTrack(&track);
	//track.setTrackLength(PI);
	cart.moveTo(0.0);
	cart.setSpeed(20.0);
	
	/*for (int i=0; i < 100000; i++) {

		cart.nextStep(0.001);
		//cout << cart.toString() << endl;
		//cin.get();
	}*/
	
		
	//cin.get();



}

static void testVector3d() {
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
