#include "Coaster.h"
 
//-------------------------------------------------------------------------------------
Coaster::Coaster(void):
mCount(0),
mCurrentObject(0),
bLMouseDown(false),
bRMouseDown(false),
mRotateSpeed(0.1f),
bRobotMode(true)
{
}
//-------------------------------------------------------------------------------------
Coaster::~Coaster(void)
{
	mSceneMgr->destroyQuery(mRayScnQuery);
}
 
//-------------------------------------------------------------------------------------
void Coaster::createScene(void)
{


	cout << "=================== Make track " << endl;
	Track track(10);
	for(int i = 0; i<track.getNumberOfPoints()-1; i++){
		Vector3d pos = track.getPos(i);
		printf("i: %d   x:%f, y:%f, z:%f \n",i, pos.x, pos.y, pos.z);
	}

	// make graphical track mesh
	this->createRailMesh(track, false);
	// add rails to scene
	Ogre::Entity* ent = mSceneMgr->createEntity("Rails","RailMesh");
    Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("RailsNode",
        Ogre::Vector3(125, 40, 500));
    node->attachObject(ent);

	
	//Scene setup
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
 
	//World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");
 
	//camera setup
	mCamera->setPosition(40, 100, 580);
	mCamera->pitch(Ogre::Degree(-30));
	mCamera->yaw(Ogre::Degree(-45));
 
	//CEGUI setup
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
 
	//show the CEGUI cursor
	CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
}


// generate and export railmesh (only run when rail has changed)
void Coaster::createRailMesh(const Track track, const bool export_mesh)
{

	if(track.getNumberOfPoints() < 2) return;

	const float width = 0.5f;
	const float height = 1.0f;

	Ogre::ManualObject rail("RailObject");
	Ogre::Vector3 size(width / 2, height/2, 0);

	size_t index_count = 8*track.getNumberOfPoints()+2;
	size_t ver_count = 4*6*track.getNumberOfPoints();

	rail.estimateIndexCount(index_count);
	rail.estimateVertexCount(ver_count);
	//start defining our manual object
	rail.begin("Rails/RailMetal", Ogre::RenderOperation::OT_TRIANGLE_LIST);
 
	Vector3d vec_math;
	Ogre::Vector3 vec, next_vec;

	printf("Points: %d \n", track.getNumberOfPoints());

	Vector3d cur_pos;
	Vector3d next_pos = track.getPos(0);

	for(int i=0; i<track.getNumberOfPoints()-1; i++){

		cur_pos = next_pos;
		next_pos = track.getPos(i+1);

		printf("i: %d\n", i);
		vec_math = vectorDiff(next_pos, cur_pos);

		vec.x = cur_pos.x;
		vec.y = cur_pos.y;
		vec.z = cur_pos.z;

		next_vec.x = cur_pos.x + vec_math.x;
		next_vec.y = cur_pos.y + vec_math.y;
		next_vec.z = cur_pos.z + vec_math.z;

		printf("Bottom left i: %d, x:%f, y:%f, z:%f \n", i, vec.x + size.x, vec.y - size.y, vec.z);
		//bottom
		rail.position(vec.x + size.x, vec.y - size.y, vec.z); // 0
		rail.textureCoord(1, 0);
		rail.position(next_vec.x - size.x, next_vec.y - size.y, next_vec.z); // 1
		rail.textureCoord(0, 1);
		rail.position(next_vec.x + size.x, next_vec.y - size.y, next_vec.z); // 2
		rail.textureCoord(1, 1);
		rail.position(vec.x - size.x, vec.y - size.y, vec.z); // 3
		rail.textureCoord(0, 0);

		//front
		rail.position(vec.x + size.x, vec.y + size.y, vec.z); // 4
		rail.textureCoord(1, 0);
		rail.position(vec.x - size.x, vec.y - size.y, vec.z); // 5
		rail.textureCoord(0, 1);
		rail.position(vec.x + size.x, vec.y - size.y, vec.z); // 6
		rail.textureCoord(1, 1);
		rail.position(vec.x - size.x, vec.y + size.y, vec.z); // 7
		rail.textureCoord(0, 0);

		//left
		rail.position(next_vec.x - size.x, next_vec.y + size.y, next_vec.z); // 8 
		rail.textureCoord(0, 1);
		rail.position(next_vec.x - size.x, next_vec.y - size.y, next_vec.z); // 9
		rail.textureCoord(1, 1);
		rail.position(vec.x - size.x, vec.y - size.y, vec.z); // 10
		rail.textureCoord(1, 0);
		rail.position(next_vec.x + size.x, next_vec.y - size.y, next_vec.z); // 11
		rail.textureCoord(0, 1);

		//right
		rail.position(next_vec.x + size.x, next_vec.y + size.y, next_vec.z); // 12
		rail.textureCoord(1, 1);
		rail.position(vec.x + size.x, vec.y - size.y, vec.z); // 13
		rail.textureCoord(0, 0);
		rail.position(next_vec.x + size.x, next_vec.y - size.y, next_vec.z); // 14
		rail.textureCoord(1, 0);
		rail.position(next_vec.x - size.x, next_vec.y - size.y, next_vec.z); // 15
		rail.textureCoord(0, 0);

		//back
		rail.position(vec.x - size.x, vec.y + size.y, vec.z); // 16
		rail.textureCoord(1, 0);
		rail.position(next_vec.x + size.x, next_vec.y + size.y, next_vec.z); // 17
		rail.textureCoord(0, 1);
		rail.position(next_vec.x - size.x, next_vec.y + size.y, next_vec.z); // 18
		rail.textureCoord(1, 1);
		rail.position(vec.x + size.x, vec.y + size.y, vec.z); // 19
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
		rail.triangle(offset+16, offset+17, offset+18);	rail.triangle(offset+16, offset+19, offset+17);
		//bottom
		rail.triangle(offset+0, offset+1, offset+2);	rail.triangle(offset+3, offset+1, offset+0);

		//left
		rail.triangle(offset+8, offset+9, offset+10);	rail.triangle(offset+10, offset+7, offset+8);
		//right
		rail.triangle(offset+4, offset+11, offset+12);	rail.triangle(offset+4, offset+13, offset+11);

		//front
		//rail.triangle(offset+4, offset+5, offset+6);	rail.triangle(offset+4, offset+7, offset+5);
		//back
		//rail.triangle(offset+14, offset+8, offset+12);	rail.triangle(offset+14, offset+15, offset+8);

	}

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
 
void Coaster::chooseSceneManager(void)
{
	//create a scene manager that is meant for handling outdoor scenes
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}
 
void Coaster::createFrameListener(void)
{
	//we still want to create the frame listener from the base app
	BaseApplication::createFrameListener();
 
	//but we also want to set up our raySceneQuery after everything has been initialized
	mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray());
}
 
bool Coaster::frameRenderingQueued(const Ogre::FrameEvent& arg)
{
	//we want to run everything in the previous frameRenderingQueued call
	//but we also want to do something afterwards, so lets  start off with this
	if(!BaseApplication::frameRenderingQueued(arg))
	{
		return false;
	}
 
	/*
	This next big chunk basically sends a raycast straight down from the camera's position
	It then checks to see if it is under world geometry and if it is we move the camera back up
	*/
	Ogre::Vector3 camPos = mCamera->getPosition();
	Ogre::Ray cameraRay(Ogre::Vector3(camPos.x, 5000.0f, camPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);
 
	mRayScnQuery->setRay(cameraRay);
 
	/*
	here we tell it not to sort the raycast results	world geometry would be 
	at the end of the list so sorting would be bad in this case since we are iterating through everything
	*/
	mRayScnQuery->setSortByDistance(false);
	Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
	Ogre::RaySceneQueryResult::iterator iter = result.begin();
 
	for(iter; iter != result.end(); iter++)
	{
		if(iter->worldFragment)
		{
			//gets the results, fixes camera height and breaks the loop
			Ogre::Real terrainHeight = iter->worldFragment->singleIntersection.y;
 
			if((terrainHeight + 10.0f) > camPos.y)
			{
				mCamera->setPosition(camPos.x, terrainHeight + 10.0f, camPos.z);
			}
			break;
		}
	}
 
	return true;
}
 
bool Coaster::mouseMoved(const OIS::MouseEvent& arg)
{
	//updates CEGUI with mouse movement
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
 
	//if the left mouse button is held down
	if(bLMouseDown)
	{
		//find the current mouse position
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
 
		//create a raycast straight out from the camera at the mouse's location
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(false);	//world geometry is at the end of the list if we sort it, so lets not do that
 
		Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter = result.begin();
 
		//check to see if the mouse is pointing at the world and put our current object at that location
		for(iter; iter != result.end(); iter++)
		{
			if(iter->worldFragment)
			{
				mCurrentObject->setPosition(iter->worldFragment->singleIntersection);
				break;
			}	
		}
	}
	else if(bRMouseDown)	//if the right mouse button is held down, be rotate the camera with the mouse
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}
 
	return true;
}
 
bool Coaster::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		//show that the current object has been deselected by removing the bounding box visual
		if(mCurrentObject)
		{
			mCurrentObject->showBoundingBox(false);
		}
 
		//find the current mouse position
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
 
		//then send a raycast straight out from the camera at the mouse's position
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);
		mRayScnQuery->setQueryMask(bRobotMode ? ROBOT_MASK : NINJA_MASK);	//will return objects with the query mask in the results
 
		/*
		This next chunk finds the results of the raycast
		If the mouse is pointing at world geometry we spawn a robot at that position
		*/
		Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter = result.begin();
 
		for(iter; iter != result.end(); iter++)
		{
			//if you clicked on a robot or ninja it becomes selected
			if(iter->movable && iter->movable->getName().substr(0, 5) != "tile[")
			{
				mCurrentObject = iter->movable->getParentSceneNode();
				break;
			}
			//otherwise we spawn a new one at the mouse location
			else if(iter->worldFragment)
			{
				char name[16];
				Ogre::Entity* ent;
 
				//if we are in robot mode we spawn a robot at the mouse location
				if(bRobotMode)
				{
					sprintf(name, "Robot%d", mCount++);
					ent = mSceneMgr->createEntity(name, "robot.mesh");
					ent->setQueryFlags(ROBOT_MASK);
				}
				//otherwise we spawn a ninja
				else
				{
					sprintf(name, "Ninja%d", mCount++);
					ent = mSceneMgr->createEntity(name, "ninja.mesh");
					ent->setQueryFlags(NINJA_MASK);
 
				}
				//attach the object to a scene node
				mCurrentObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(std::string(name) + "Node", iter->worldFragment->singleIntersection);
				mCurrentObject->attachObject(ent);
 
				//lets shrink the object, only because the terrain is pretty small
				mCurrentObject->setScale(0.1f, 0.1f, 0.1f);
				break;
			}
		}
 
		//now we show the bounding box so the user can see that this object is selected
		if(mCurrentObject)
		{
			mCurrentObject->showBoundingBox(true);
		}
 
		bLMouseDown = true;
	}
	else if(id == OIS::MB_Right)	// if the right mouse button is held we hide the mouse cursor for view mode
	{
		CEGUI::MouseCursor::getSingleton().hide();
		bRMouseDown = true;
	}
 
	return true;
}
 
bool Coaster::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(id  == OIS::MB_Left)
	{
		bLMouseDown = false;
	}
	else if(id == OIS::MB_Right)	//when the right mouse is released we then unhide the cursor
	{
		CEGUI::MouseCursor::getSingleton().show();
		bRMouseDown = false;
	}
	return true;
}
 
bool Coaster::keyPressed(const OIS::KeyEvent& arg)
{
	//check and see if the spacebar was hit, and this will switch which mesh is spawned
	if(arg.key == OIS::KC_SPACE)
	{
		bRobotMode = !bRobotMode;
	}
 
	//then we return the base app keyPressed function so that we get all of the functionality
	//and the return value in one line
	return BaseApplication::keyPressed(arg);
}
 

void showWin32Console()
{
    static const WORD MAX_CONSOLE_LINES = 500;
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
    // allocate a console for this app
    AllocConsole();
    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
    coninfo.dwSize);
    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );
    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );
    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();
}
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char *argv[])
#endif
	{

		showWin32Console();

		// Create application object
		Coaster app;

		try {
			app.go();
		} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		FreeConsole();
 
		return 0;
	}
 
#ifdef __cplusplus
}
#endif