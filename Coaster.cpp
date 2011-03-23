#include "Coaster.h"
//#include "tests.h"

//-------------------------------------------------------------------------------------
Coaster::Coaster(void):
mCount(0),
mCurrentObject(0),
bLMouseDown(false),
bRMouseDown(false),
mRotateSpeed(0.1f),
bRobotMode(true),
track()
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
	/*
	// make graphical track mesh
	GraphicTrack::createRailMesh(&track, false);
	// add rails to scene
	Ogre::Entity* rail = mSceneMgr->createEntity("Rails","RailMesh");
	*/

	physicsCart = PhysicsCart();

    railNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RailsNode", Ogre::Vector3(0, 0, 0));
    //railNode->attachObject(rail);
	
	//Add cart 
	cartNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cartNode", Ogre::Vector3(0, 0, 0));
    
	Ogre::Entity* cartEnt = mSceneMgr->createEntity("Cart", "minecart.mesh");
				  cartEnt->setQueryFlags(CART_MASK);
	cartNode->attachObject(cartEnt);
	cartNode->setScale(0.1f, 0.1f, 0.1f);

	cameraName = "PlayerCam";
	// create the camera
    pCamera = mSceneMgr->createCamera("CartCam");
	cartNode->attachObject(pCamera);

	/*
	// create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
	cartNode->attachObject(mCamera);
    // set its position, direction  
    mCamera->setPosition(Ogre::Vector3(0,10,500));
    mCamera->lookAt(Ogre::Vector3(0,0,0));
    // set the near clip distance
    mCamera->setNearClipDistance(5);
	*/
 
	/*
	cam = mCamera;
	vp = mWindow->addViewport(cam, 1, 0.5, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	*/
	
	//Scene setup
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
 
	//world geometry
	mSceneMgr->setWorldGeometry("island.cfg");

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.9, 0.9, 0.9));
 
    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);
 
	//camera change setup
	mCamera->setPosition(670, 860, 4570);
	mCamera->pitch(Ogre::Degree(-30));
	mCamera->yaw(Ogre::Degree(-45));
	mCamera->setNearClipDistance(0.5f);
 
	//CEGUI setup
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
 
	//show the CEGUI cursor
	CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
}

void Coaster::changeViewPoint(void){

	if(cameraName == "PlayerCam"){
		cameraName = "CartCam";
	} else {
		cameraName = "PlayerCam";
	}

	mWindow->removeAllViewports();
	Ogre::Viewport *vp = 0;
	Ogre::Camera *cam = mSceneMgr->getCamera(cameraName);
	vp = mWindow->addViewport(cam, 0, 0, 0, 1, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

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
	//delta time
	Ogre::Real dt = arg.timeSinceLastFrame;
	
	if(physicsCart.hasTrack()){
		Vector3d pos = physicsCart.getPos();
		Ogre::Vector3 ogre_pos= Ogre::Vector3(pos.x, pos.y, pos.z);
		cartNode->setPosition(ogre_pos);

		printf("Forward: x%f y%f z%f \n", physicsCart.getForward().x, physicsCart.getForward().y, physicsCart.getForward().z);

		//rotate cart
		Ogre::Vector3 mDirection = Ogre::Vector3(physicsCart.getForward().x, physicsCart.getForward().y, physicsCart.getForward().z);
		Ogre::Vector3 src = cartNode->getOrientation() * Ogre::Vector3::UNIT_X;
		Ogre::Quaternion quat = src.getRotationTo(mDirection);
		cartNode->rotate(quat);

		//next step
		physicsCart.nextStep(dt);
	}
	

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
		//this->track = test3();
		//track.setTrackLength(1000);
		//physicsCart.setTrack(&track);
		//physicsCart.moveTo(1.0);
			

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

		Ogre::MeshManager *meshManager = mRoot->getMeshManager();

		bool position_added = false;
 
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

				//add track 120 over the ground
				track.addPos(Vector3d(iter->worldFragment->singleIntersection.x, iter->worldFragment->singleIntersection.y+120, iter->worldFragment->singleIntersection.z),
					Vector3d(0,1,0));
				position_added = true;
 
				//lets shrink the object, only because the terrain is pretty small
				//mCurrentObject->setScale(0.1f, 0.1f, 0.1f);
				break;
			}
		}
		if(position_added && track.getNumberOfPoints() > 3){

			railNode->detachAllObjects();
			if(mSceneMgr->hasEntity("Rails")){
				mSceneMgr->destroyEntity("Rails");
				mSceneMgr->destroyManualObject("RailMesh");
				meshManager->remove("RailMesh");
			}

			// make graphical track mesh
			GraphicTrack::createRailMesh(&track, false);
			
			physicsCart.setTrack(&track);

			// add rails to scene
			Ogre::Entity* rail = mSceneMgr->createEntity("Rails","RailMesh");
			rail->setQueryFlags(RAIL_MASK);
			railNode->attachObject(rail);

			//start cart from start
			physicsCart.moveTo(0.0);
			Vector3d start_pos = physicsCart.getPos();
			Ogre::Vector3 start_pos_ogre = Ogre::Vector3(start_pos.x, start_pos.y, start_pos.z);
			cartNode->setPosition(start_pos_ogre);

			printf("Rail generated\n");
			
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
		changeViewPoint();
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