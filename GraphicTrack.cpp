#include "GraphicTrack.h"

/*
GraphicTrack::GraphicTrack(void)
{
}
//-------------------------------------------------------------------------------------
GraphicTrack::~GraphicTrack(void)
{
}
*/

// generate and export railmesh (only run when rail has changed)
void GraphicTrack::createRailMesh(Track* track2, const bool export_mesh)
{

	Track track = *track2;

	const float width = 0.15f;
	const float height = 0.3f;
	const float length_between_rails = 1.0f;

	Ogre::ManualObject rail("RailObject");
	Ogre::Vector3 size(width / 2, height/2, 0);

	size_t index_count =  track.getSmoothedDelta()*(8*track.getNumberOfPoints())+2;
	size_t ver_count = 4*6*track.getNumberOfPoints()*track.getSmoothedDelta();

	rail.estimateIndexCount(index_count);
	rail.estimateVertexCount(ver_count);
	//start defining our manual object
	rail.begin("Rails/RailMetal", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	if(track.getNumberOfPoints() > 3){
 
	Vector3d vec_math, vec_math2;
	Ogre::Vector3 vec, next_vec;

	printf("Points: %d \n", track.getNumberOfPoints());

	for(int h=0; h<track.getNumberOfPoints(); h++){
		printf("h:%d x:%f y:%f z:%f \n", h, track.getControlPoint(h).x, track.getControlPoint(h).y, track.getControlPoint(h).z);
	}

	double td = track.getSmoothedDelta();
	double t = td;

	Vector3d last_pos = track.getPos(0);
	Vector3d cur_pos = track.getPos(0);
	Vector3d next_pos = track.getPos(t);
	
	Vector3d diff, norm, last_norm, tangent, last_tangent, up, last_up, norm_parallel;
	double norm_length;

	//normal in crosspoint
	norm = Vector3d(1,0,0);
	tangent = Vector3d(0,0,1);
	up = Vector3d(0, 1, 0);
	norm_parallel = norm*length_between_rails;

	//temp var
	Ogre::Vector3 
		left_front_top_right,
		left_front_top_left,
		left_front_bottom_right,
		left_front_bottom_left,
		left_back_top_right,
		left_back_top_left,
		left_back_bottom_right,
		left_back_bottom_left,

		right_front_top_right,
		right_front_top_left,
		right_front_bottom_right,
		right_front_bottom_left,
		right_back_top_right,
		right_back_top_left,
		right_back_bottom_right,
		right_back_bottom_left;

	//first point calculation
	tangent = track.getTangentVector(0);
	up = track.getUp(0);
	norm = up.cross(tangent);

	// Divide by current length and multiply by the offset to get correct length
	norm_length = norm.length();
	norm *= size.x/norm_length;
	norm.y = 0;
		
	norm_parallel = norm*length_between_rails;

	left_front_top_right	= Ogre::Vector3(cur_pos.x - norm.x + norm_parallel.x, cur_pos.y + size.y, cur_pos.z - norm.z + norm_parallel.z);
	left_front_top_left		= Ogre::Vector3(cur_pos.x + norm.x + norm_parallel.x, cur_pos.y + size.y, cur_pos.z + norm.z + norm_parallel.z);
	left_front_bottom_right	= Ogre::Vector3(cur_pos.x - norm.x + norm_parallel.x, cur_pos.y - size.y, cur_pos.z - norm.z + norm_parallel.z);
	left_front_bottom_left	= Ogre::Vector3(cur_pos.x + norm.x + norm_parallel.x, cur_pos.y - size.y, cur_pos.z + norm.z + norm_parallel.z);

	right_front_top_right		= Ogre::Vector3(cur_pos.x - norm.x - norm_parallel.x, cur_pos.y + size.y, cur_pos.z - norm.z - norm_parallel.z);
	right_front_top_left		= Ogre::Vector3(cur_pos.x + norm.x - norm_parallel.x, cur_pos.y + size.y, cur_pos.z + norm.z - norm_parallel.z);
	right_front_bottom_right	= Ogre::Vector3(cur_pos.x - norm.x - norm_parallel.x, cur_pos.y - size.y, cur_pos.z - norm.z - norm_parallel.z);
	right_front_bottom_left		= Ogre::Vector3(cur_pos.x + norm.x - norm_parallel.x, cur_pos.y - size.y, cur_pos.z + norm.z - norm_parallel.z);

	/*
	printf("LFTR: x: %f y:%f z:%f \n", left_front_top_right.x, left_front_top_right.y, left_front_top_right.z);
	printf("LFTL: x: %f y:%f z:%f \n", left_front_top_left.x, left_front_top_left.y, left_front_top_left.z);
	printf("LFBR: x: %f y:%f z:%f \n", left_front_bottom_right.x, left_front_bottom_right.y, left_front_bottom_right.z);
	printf("LFBL: x: %f y:%f z:%f \n", left_front_bottom_left.x, left_front_bottom_left.y, left_front_bottom_left.z);

	printf("RFTR: x: %f y:%f z:%f \n", right_front_top_right.x, left_front_top_right.y, left_front_top_right.z);
	printf("RFTL: x: %f y:%f z:%f \n", right_front_top_left.x, left_front_top_left.y, left_front_top_left.z);
	printf("RFBR: x: %f y:%f z:%f \n", right_front_bottom_right.x, left_front_bottom_right.y, left_front_bottom_right.z);
	printf("RFBL: x: %f y:%f z:%f \n", right_front_bottom_left.x, left_front_bottom_left.y, left_front_bottom_left.z);
	*/

	t += td;

	for(int i=0; i<(track.getNumberOfPoints()-1)*track.getSmoothingValue(); i++){
	
		//printf("T: %f\n", t);
		last_pos = cur_pos;
		cur_pos = next_pos;
		if(t > 1) t = 1;
		next_pos = track.getPos(t);
		last_norm = norm;
		last_tangent = tangent;
		last_up = up;

		tangent = track.getTangentVector(t);
		printf("Tangent: x:%f y:%f z:%f \n", tangent.x, tangent.y, tangent.z);
		up = track.getUp(t);
		norm = up.cross(tangent);

		// Divide by current length and multiply by the offset to get correct length
		norm_length = norm.length();
		norm *= size.x/norm_length;
		norm.y = 0;
		t += td;
		
		norm_parallel = norm*5;

		//left rail
		left_back_top_left		= left_front_top_left;
		left_back_bottom_left	= left_front_bottom_left;
		left_back_bottom_right	= left_front_bottom_right;

		/*
		printf("LFTR: next_pos.x: %f, norm.x %f, norm_parallel.x %f, next_pos.y %f, size.y :%f next_pos.z %f norm.z %f norm_parallel.z %f \n", next_pos.x, norm.x, norm_parallel.x, next_pos.y ,size.y, next_pos.z , norm.z , norm_parallel.z);
		printf("LFTR: x: %f y:%f z:%f \n", next_pos.x - norm.x + norm_parallel.x, next_pos.y + size.y, next_pos.z - norm.z + norm_parallel.z);
		printf("LFTL: x: %f y:%f z:%f \n", next_pos.x + norm.x + norm_parallel.x, next_pos.y + size.y, next_pos.z + norm.z + norm_parallel.z);
		printf("LFBR: x: %f y:%f z:%f \n", next_pos.x - norm.x + norm_parallel.x, next_pos.y - size.y, next_pos.z - norm.z + norm_parallel.z);
		printf("LFBL: x: %f y:%f z:%f \n", next_pos.x + norm.x + norm_parallel.x, next_pos.y - size.y, next_pos.z + norm.z + norm_parallel.z);
		*/
		
		left_front_top_right	= Ogre::Vector3(next_pos.x - norm.x + norm_parallel.x, next_pos.y + size.y+200, next_pos.z - norm.z + norm_parallel.z);
		left_front_top_left		= Ogre::Vector3(next_pos.x + norm.x + norm_parallel.x, next_pos.y + size.y, next_pos.z + norm.z + norm_parallel.z);
		left_front_bottom_right	= Ogre::Vector3(next_pos.x - norm.x + norm_parallel.x, next_pos.y - size.y, next_pos.z - norm.z + norm_parallel.z);
		left_front_bottom_left	= Ogre::Vector3(next_pos.x + norm.x + norm_parallel.x, next_pos.y - size.y, next_pos.z + norm.z + norm_parallel.z);

		//printf("Making track %d \n", i);

		//top
		rail.position(left_front_top_right); // 0
		rail.textureCoord(1, 0);
		rail.position(left_front_top_left); // 1
		rail.textureCoord(0, 1);
		rail.position(left_back_top_right); // 2
		rail.textureCoord(1, 1);
		rail.position(left_back_top_left); // 3
		rail.textureCoord(0, 0);

		//bottom
		rail.position(left_front_bottom_right); // 4
		rail.textureCoord(1, 0);
		rail.position(left_front_bottom_left); // 5
		rail.textureCoord(0, 1);
		rail.position(left_back_bottom_right); // 6
		rail.textureCoord(1, 1);
		rail.position(left_back_bottom_left); // 7
		rail.textureCoord(0, 0);

		//int offset = i*20;
		int offset = i*16;
		//top
		//rail.triangle(offset+0, offset+1, offset+2);	rail.triangle(offset+1, offset+3, offset+2);
		
		//bottom
		//rail.triangle(offset+4, offset+6, offset+5);	rail.triangle(offset+5, offset+6, offset+7);

		//right
		//rail.triangle(offset+0, offset+2, offset+4);	rail.triangle(offset+2, offset+6, offset+4);

		//left
		//rail.triangle(offset+1, offset+5, offset+3);	rail.triangle(offset+3, offset+5, offset+7);
		
		//right rail
		right_back_top_right	= right_front_top_right;
		right_back_top_left		= right_front_top_left;
		right_back_bottom_left	= right_front_bottom_left;
		right_back_bottom_right	= right_front_bottom_right;
		
		right_front_top_right		= Ogre::Vector3(next_pos.x - norm.x - norm_parallel.x, next_pos.y + size.y, next_pos.z - norm.z - norm_parallel.z);
		right_front_top_left		= Ogre::Vector3(next_pos.x + norm.x - norm_parallel.x, next_pos.y + size.y, next_pos.z + norm.z - norm_parallel.z);
		right_front_bottom_right	= Ogre::Vector3(next_pos.x - norm.x - norm_parallel.x, next_pos.y - size.y, next_pos.z - norm.z - norm_parallel.z);
		right_front_bottom_left		= Ogre::Vector3(next_pos.x + norm.x - norm_parallel.x, next_pos.y - size.y, next_pos.z + norm.z - norm_parallel.z);

		//top
		rail.position(right_front_top_right); // 0
		rail.textureCoord(1, 0);
		rail.position(right_front_top_left); // 1
		rail.textureCoord(0, 1);
		rail.position(right_back_top_right); // 2
		rail.textureCoord(1, 1);
		rail.position(right_back_top_left); // 3
		rail.textureCoord(0, 0);

		//bottom
		rail.position(right_front_bottom_right); // 4
		rail.textureCoord(1, 0);
		rail.position(right_front_bottom_left); // 5
		rail.textureCoord(0, 1);
		rail.position(right_back_bottom_right); // 6
		rail.textureCoord(1, 1);
		rail.position(right_back_bottom_left); // 7
		rail.textureCoord(0, 0);

		
		//int offset = i*20;
		offset = i*16+8;
		//top
		rail.triangle(offset+0, offset+1, offset+2);	rail.triangle(offset+1, offset+3, offset+2);
		
		//bottom
		rail.triangle(offset+4, offset+6, offset+5);	rail.triangle(offset+5, offset+6, offset+7);

		//right
		rail.triangle(offset+0, offset+2, offset+4);	rail.triangle(offset+2, offset+6, offset+4);

		//left
		rail.triangle(offset+1, offset+5, offset+3);	rail.triangle(offset+3, offset+5, offset+7);
		
		/*

	printf("LFTR: x: %f y:%f z:%f \n", left_front_top_right.x, left_front_top_right.y, left_front_top_right.z);
	printf("LFTL: x: %f y:%f z:%f \n", left_front_top_left.x, left_front_top_left.y, left_front_top_left.z);
	printf("LFBR: x: %f y:%f z:%f \n", left_front_bottom_right.x, left_front_bottom_right.y, left_front_bottom_right.z);
	printf("LFBL: x: %f y:%f z:%f \n", left_front_bottom_left.x, left_front_bottom_left.y, left_front_bottom_left.z);

	printf("RFTR: x: %f y:%f z:%f \n", right_front_top_right.x, left_front_top_right.y, left_front_top_right.z);
	printf("RFTL: x: %f y:%f z:%f \n", right_front_top_left.x, left_front_top_left.y, left_front_top_left.z);
	printf("RFBR: x: %f y:%f z:%f \n", right_front_bottom_right.x, left_front_bottom_right.y, left_front_bottom_right.z);
	printf("RFBL: x: %f y:%f z:%f \n", right_front_bottom_left.x, left_front_bottom_left.y, left_front_bottom_left.z);
	*/
	}


	} // more than 3 points

	

	rail.end();
 
	if(export_mesh){
		//create an actual mesh out of this object
		Ogre::MeshPtr ptr = rail.convertToMesh("RailMesh");
		Ogre::MeshSerializer ser;
		ser.exportMesh(ptr.getPointer(), "rails.mesh");
	} else {
		rail.convertToMesh("RailMesh");
	}
}