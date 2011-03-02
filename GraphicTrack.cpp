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

	const float width = 10.0f;
	const float height = 2.0f;

	Ogre::ManualObject rail("RailObject");
	Ogre::Vector3 size(width / 2, height/2, 0);

	size_t index_count =  track.getSmoothValue()*(8*track.getNumberOfPoints())+2;
	size_t ver_count = 4*6*track.getNumberOfPoints()*track.getSmoothValue();

	rail.estimateIndexCount(index_count);
	rail.estimateVertexCount(ver_count);
	//start defining our manual object
	rail.begin("Rails/RailMetal", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	if(track.getNumberOfPoints() > 3){
 
	Vector3d vec_math, vec_math2;
	Ogre::Vector3 vec, next_vec;

	printf("Points: %d \n", track.getNumberOfPoints());

	double td = track.getDelta();
	double t = td;

	Vector3d last_pos = track.getPos(0);
	Vector3d cur_pos = track.getPos(0);
	Vector3d next_pos = track.getInterpolatedSplinePoint(t);
	
	Vector3d diff, norm, last_norm;
	double norm_scale, norm_length;

	//normal in crosspoint
	norm = track.getNormalVector(0);

	//temp var
	Ogre::Vector3 
		front_top_right,
		front_top_left,
		front_bottom_right,
		front_bottom_left,
		back_top_right,
		back_top_left,
		back_bottom_right,
		back_bottom_left;

	t += td;
	for(int i=0; i<(track.getNumberOfPoints()*track.getSmoothValue())-1; i++){
	
		last_pos = cur_pos;
		cur_pos = next_pos;
		if(t > 1) t = 1;
		next_pos = track.getInterpolatedSplinePoint(t);
		last_norm = norm;
		norm = track.getNormalVector(t);
		t += td;

		//scale up, so its wider
		norm *= size.x;
		norm_length = norm.length();

		printf("normal x:%f y:%f z:%f length: %f \n", norm.x, norm.y, norm.z, norm_length);

		front_top_right		= Ogre::Vector3(next_pos.x + norm.x, next_pos.y + size.y, next_pos.z + norm.z),
		front_top_left		= Ogre::Vector3(next_pos.x - norm.x, next_pos.y + size.y, next_pos.z - norm.z),
		front_bottom_right	= Ogre::Vector3(next_pos.x + norm.x, next_pos.y - size.y, next_pos.z + norm.z),
		front_bottom_left	= Ogre::Vector3(next_pos.x - norm.x, next_pos.y - size.y, next_pos.z - norm.z);
			
		back_top_right		= Ogre::Vector3(cur_pos.x + last_norm.x, cur_pos.y + size.y, cur_pos.z + last_norm.z),
		back_top_left		= Ogre::Vector3(cur_pos.x - last_norm.x, cur_pos.y + size.y, cur_pos.z - last_norm.z),
		back_bottom_right	= Ogre::Vector3(cur_pos.x + last_norm.x, cur_pos.y - size.y, cur_pos.z + last_norm.z),
		back_bottom_left	= Ogre::Vector3(cur_pos.x - last_norm.x, cur_pos.y - size.y, cur_pos.z - last_norm.z);


		//top
		rail.position(front_top_right); // 0
		rail.textureCoord(1, 0);
		rail.position(front_top_left); // 1
		rail.textureCoord(0, 1);
		rail.position(back_top_right); // 2
		rail.textureCoord(1, 1);
		rail.position(back_top_left); // 3
		rail.textureCoord(0, 0);


#if 0
		/*
		//printf("i: %d\n", i);
		vec_math = next_pos-cur_pos;
		vec_math2 = end_pos-next_pos;

		vec.x = cur_pos.x;
		vec.y = cur_pos.y;
		vec.z = cur_pos.z;

		next_vec.x = cur_pos.x + vec_math.x;
		next_vec.y = cur_pos.y + vec_math.y;
		next_vec.z = cur_pos.z + vec_math.z;
		*/

		Ogre::Real temp;
		bool flip = false;

		/*
		if( next_vec.z < vec.z ){
			temp = next_vec.x;
			next_vec.x = vec.x;
			vec.x = temp;
			
			temp = next_vec.y;
			next_vec.y = vec.y;
			vec.y = temp;

			temp = next_vec.z;
			next_vec.z = vec.z;
			vec.z = temp;
			flip = true;

		}
		*/

		// flip -/+ size?
		

		//printf("Vec i: %d, x:%f, y:%f, z:%f \n", i, vec.x, vec.y, vec.z);
		//printf("Next_vec left i: %d, x:%f, y:%f, z:%f \n", i, next_vec.x, next_vec.y, next_vec.z);
		
		Ogre::Real size_x_pos = (flip) ? -size.x : size.x;
		Ogre::Real size_x_neg = (flip) ? size.x : -size.x;

		Ogre::Real size_y_pos = (flip) ? -size.y : size.y;
		Ogre::Real size_y_neg = (flip) ? size.y : -size.y;

		//top
		rail.position(next_vec.x + size_x_pos, next_vec.y + size_y_neg, next_vec.z); // 0
		rail.textureCoord(1, 0);
		rail.position(vec.x + size_x_neg, vec.y + size_y_neg, vec.z); // 1
		rail.textureCoord(0, 1);
		rail.position(vec.x + size_x_pos, vec.y + size_y_neg, vec.z); // 2
		rail.textureCoord(1, 1);
		rail.position(next_vec.x + size_x_neg, next_vec.y + size_y_neg, next_vec.z); // 3
		rail.textureCoord(0, 0);

		//front
		rail.position(next_vec.x + size_x_pos, next_vec.y + size_y_pos, next_vec.z); // 4
		rail.textureCoord(1, 0);
		rail.position(next_vec.x + size_x_neg, next_vec.y + size_y_neg, next_vec.z); // 5
		rail.textureCoord(0, 1);
		rail.position(next_vec.x + size_x_pos, next_vec.y + size_y_neg, next_vec.z); // 6
		rail.textureCoord(1, 1);
		rail.position(next_vec.x + size_x_neg, next_vec.y + size_y_pos, next_vec.z); // 7
		rail.textureCoord(0, 0);

		//right
		rail.position(vec.x + size_x_neg, vec.y + size_y_pos, vec.z); // 8 
		rail.textureCoord(0, 1);
		rail.position(vec.x + size_x_neg, vec.y + size_y_neg, vec.z); // 9
		rail.textureCoord(1, 1);
		rail.position(next_vec.x + size_x_neg, next_vec.y + size_y_neg, next_vec.z); // 10
		rail.textureCoord(1, 0);
		rail.position(vec.x + size_x_pos, vec.y + size_y_neg, vec.z); // 11
		rail.textureCoord(0, 1);

		//left
		rail.position(vec.x + size_x_pos, vec.y + size_y_pos, vec.z); // 12
		rail.textureCoord(1, 1);
		rail.position(next_vec.x + size_x_pos, next_vec.y + size_y_neg, next_vec.z); // 13
		rail.textureCoord(0, 0);
		rail.position(vec.x + size_x_pos, vec.y + size_y_neg, vec.z); // 14
		rail.textureCoord(1, 0);
		rail.position(vec.x + size_x_neg, vec.y + size_y_neg, vec.z); // 15
		rail.textureCoord(0, 0);

		//back
		rail.position(next_vec.x + size_x_neg, next_vec.y + size_y_pos, next_vec.z); // 16
		rail.textureCoord(1, 0);
		rail.position(vec.x + size_x_pos, vec.y + size_y_pos, vec.z); // 17
		rail.textureCoord(0, 1);
		rail.position(vec.x + size_x_neg, vec.y + size_y_pos, vec.z); // 18
		rail.textureCoord(1, 1);
		rail.position(next_vec.x + size_x_pos, next_vec.y + size_y_pos, next_vec.z); // 19
		rail.textureCoord(0, 0);
		

		int offset = i*20;

		// 0 = 6 = 13
		// 3 = 5 = 10
		// 1 = 9 = 15
		// 8 = 18
		// 12 = 17
		// 2 = 11 = 14
		// 7 = 16
		// 4 = 19
		
		//top
		rail.triangle(offset+0, offset+1, offset+2);	rail.triangle(offset+3, offset+1, offset+0);
		
		//front
		//rail.triangle(offset+4, offset+5, offset+6);	rail.triangle(offset+4, offset+7, offset+5);
		
		//right
		rail.triangle(offset+8, offset+9, offset+10);	rail.triangle(offset+10, offset+7, offset+8);
		//left
		rail.triangle(offset+4, offset+11, offset+12);	rail.triangle(offset+4, offset+13, offset+11);
		
		//back
		//rail.triangle(offset+14, offset+8, offset+12);	rail.triangle(offset+14, offset+15, offset+8);

		//bottom
		//rail.triangle(offset+16, offset+17, offset+18);	rail.triangle(offset+16, offset+19, offset+17);
#endif

		//int offset = i*20;
		int offset = i*4;
		//top
		rail.triangle(offset+2, offset+0, offset+1);	rail.triangle(offset+3, offset+2, offset+1);
		
		//front
		//rail.triangle(offset+4, offset+5, offset+6);	rail.triangle(offset+4, offset+7, offset+5);
		
		//right
		//rail.triangle(offset+8, offset+9, offset+10);	rail.triangle(offset+10, offset+7, offset+8);
		//left
		//rail.triangle(offset+4, offset+11, offset+12);	rail.triangle(offset+4, offset+13, offset+11);
		
		//back
		//rail.triangle(offset+14, offset+8, offset+12);	rail.triangle(offset+14, offset+15, offset+8);

		//bottom
		//rail.triangle(offset+16, offset+17, offset+18);	rail.triangle(offset+16, offset+19, offset+17);


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