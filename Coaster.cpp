#include "Coaster.h"
//#include "tests.h"


//-------------------------------------------------------------------------------------
Coaster::Coaster(void):
mCount(0),
mCurrentObject(0),
bLMouseDown(false),
bRMouseDown(false),
mRotateSpeed(0.1f),
editorMode(false),
track(),
mControllPointCount(0),
adjustHeight(false),
physicsCart(new PhysicsCart()),
highscore_time(0),
controlPointSelected(0),
objectToBePlaced(SUPPORT_ELEMENT_MASK),
objectRotatingRight(false),
objectRotatingLeft(false),
objectScalingUp(false),
objectScalingDown(false),
addNewThing(false),
max_fuel(3),
fuel(max_fuel), // 3 seconds of fuel is max
queryFlagMap(),
mCurrentSkyBox(0),
finished(false)
{
		
		//BILLBOARD_MASK = 1<<0,
		//HYTTE_MASK = 1<<1,
		//RAIL_MASK  = 1<<2,
		//CART_MASK  = 1<<3,
		//ISLAND_MASK = 1<<4,
		//CAMERA_MASK = 1<<5,
		//STONE12_MASK = 1<<6,
		//STONE17_MASK = 1<<7,
		//STONE117_MASK = 1<<8,
		//PALM_TREE_1_MASK = 1<<9,
		//PALM_TREE_2_MASK = 1<<10,
		//PALM_TREE_3_MASK = 1<<11,
		//SUPPORT_ELEMENT_MASK = 1<<12,
		//YELLOW_SUB_MASK = 1<<13,
		//END_MASK = 1<<14
	queryFlagMap[1 << 0] = "iglo.mesh";
	queryFlagMap[1 << 1] = "hytte.mesh";
	queryFlagMap[1 << 2] = "railMesh.mesh";
	queryFlagMap[1 << 3] = "cart.mesh";
	queryFlagMap[1 << 4] = "island.mesh";
	queryFlagMap[1 << 5] = "camera.mesh";
	queryFlagMap[1 << 6] = "stone12.mesh";
	queryFlagMap[1 << 7] = "stone17.mesh";
	queryFlagMap[1 << 8] = "stone117.mesh";
	queryFlagMap[1 << 9] = "Palm.mesh";
	queryFlagMap[1 << 10] = "support_element.mesh";
	queryFlagMap[1 << 11] = "yellow_sub.mesh";
	queryFlagMap[1 << 12] = "end_mask";

	mSkyBoxes[0] = "Sky/ClubTropicana";
	mSkyBoxes[1] = "Sky/EarlyMorning";
	mSkyBoxes[2] = "Sky/Clouds";

	mSunPosition[0] = Ogre::Vector3(0,10000,0);
	mSunPosition[1] = Ogre::Vector3(0,10000,90000);
	mSunPosition[2] = Ogre::Vector3(0,10000,0);

	mSunColor[0] = Ogre::Vector3(1, 0.9, 0.6);
	mSunColor[1] = Ogre::Vector3(1,0.6,0.4);
	mSunColor[2] = Ogre::Vector3(0.45,0.45,0.45);
}
//----------------------------------------------------------------------%---------------
Coaster::~Coaster(void)
{
	// Kommentert ut av Per Ivar. Denne linja krasjer
	if (mSceneMgr) mSceneMgr->destroyQuery(mRayScnQuery);
	
	//delete physicsCart;
	//delete soundEngine;

}



//-------------------------------------------------------------------------------------
void Coaster::createScene(void)
{
	delete physicsCart;
	physicsCart = new PhysicsCart();

    railNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RailsNode", Ogre::Vector3(0, 0, 0));
    //railNode->attachObject(rail);
	
	//Add cart 
	cartNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cartNode", Ogre::Vector3(0, 0, 0));
	// For all "creatable" entities
	mSceneMgr->getRootSceneNode()->createChildSceneNode("world", Ogre::Vector3(0, 0, 0));
    
	Ogre::Entity* cartEnt = mSceneMgr->createEntity("Cart", "cart.mesh");
				  cartEnt->setQueryFlags(CART_MASK);
	cartNode->attachObject(cartEnt);
	cartNode->setScale(0.05f, 0.05f, 0.05f);


	placedObjects = std::vector<Ogre::String>(0);
	
	cameraName = "PlayerCam";
	// create the camera
    pCamera = mSceneMgr->createCamera("CartCam");
	pCamera->setNearClipDistance(0.5f);
	pCamera->setQueryFlags(CAMERA_MASK);
	Ogre::SceneNode* cameraNode = cartNode->createChildSceneNode("cameraNode", Ogre::Vector3(0, 80, 180));
	//pCamera->pitch(Ogre::Degree(-5));
	cameraNode->attachObject(pCamera);

	// water and island and light
	// Set default ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

    // Create the SkyBox
    mSceneMgr->setSkyBox(true, mSkyBoxes[mCurrentSkyBox], 99999*3, true);

	// Set some camera params
    mCamera->setFarClipDistance(99999*6);
    pCamera->setFarClipDistance(99999*6);

	// Light
	Ogre::Light *mLight = mSceneMgr->createLight("Light0");
	mLight->setPosition(mSunPosition[mCurrentSkyBox]);
	mLight->setDiffuseColour(1, 1, 1);
	mLight->setSpecularColour(mSunColor[mCurrentSkyBox].x,
			                    mSunColor[mCurrentSkyBox].y,
								mSunColor[mCurrentSkyBox].z);

	// Hydrax initialization code ---------------------------------------------
	// ------------------------------------------------------------------------

    // Create Hydrax object
	mHydrax = new Hydrax::Hydrax(mSceneMgr, pCamera, mWindow->getViewport(0));

	// Create our projected grid module  
	Hydrax::Module::ProjectedGrid *mModule 
		= new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
			                                mHydrax,
											// Noise module
											new Hydrax::Noise::Perlin(/*Generic one*/),
											// Base plane
			                                Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Real(0.0f)),
											// Normal mode
											Hydrax::MaterialManager::NM_VERTEX,
											// Projected grid options
										    Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

	// Set our module
	mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

	// Load all parameters from config file
	// Remarks: The config file must be in Hydrax resource group.
	// All parameters can be set/updated directly by code(Like previous versions),
	// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
	mHydrax->loadCfg("Tropical.hdx");

    // Create water
    mHydrax->create();

	// Hydrax initialization code end -----------------------------------------
	// ------------------------------------------------------------------------

	// Load island
 
	//world geometry
	mSceneMgr->setWorldGeometry("island.cfg");

    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.9f, 0.9f, 0.9f));
 
    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);
 
	//camera change setup
	mCamera->setPosition(2080, 573, 300);
	mCamera->pitch(Ogre::Degree(-15));
	mCamera->yaw(Ogre::Degree(90));
	mCamera->setNearClipDistance(0.5f);
 
	//CEGUI setup
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();


	//show the CEGUI cursor
	CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

	// Setup sound
	//Ogre::Vector3 middle(track.getPos(0.5).x, track.getPos(0.5).y, track.getPos(0.5).z);
	Ogre::Vector3 middle(0,0,0);
	//testNode = this->mSceneMgr->getRootSceneNode()->createChildSceneNode("Testnode", middle);
				
	soundEngine = new SoundEngine(physicsCart, cartNode);
	soundEngine->playBackgroundSounds(true);
	
	//soundEngine->addSound(SoundEngine::BLIZZARD01, cartNode);
	
	debugImport();
	changeViewPoint();

	/*
	jetParticleRight = mSceneMgr->createParticleSystem("fire", "Examples/JetEngine1");
	jetParticleLeft = mSceneMgr->createParticleSystem("fire2", "Examples/JetEngine1");
	
	Ogre::SceneNode* particleNodeRight = cartNode->createChildSceneNode("Particle");
	particleNodeRight->attachObject(jetParticleRight);
	particleNodeRight->setScale(0.1, 0.1, 0.1);
	particleNodeRight->setPosition(0, 5, -5);

	Ogre::SceneNode* particleNodeLeft = cartNode->createChildSceneNode("Particle2");
	particleNodeLeft->attachObject(jetParticleLeft);
	particleNodeLeft->setScale(0.1, 0.1, 0.1);
	particleNodeLeft->setPosition(0, 5, 5);
	*/
}

void Coaster::changeViewPoint(void){

	if(!editorMode){
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

	if(track.getNumberOfPoints() > 3){
		for (vector<Ogre::String>::iterator it = placedObjects.begin(); it!=placedObjects.end(); ++it) {
			int pos = ((string)(*it)).find("Controll");

			//Controll point
			if(pos != string::npos){
				mSceneMgr->getSceneNode(*it)->setVisible(editorMode);
			}
		}
	}

	changeCameraMovement(editorMode);
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

	mHydrax->update(dt);

	if(!finished){
		highscore_time += dt;
	}
	if(physicsCart->getThrust() > 0){
		// if full thrust (1) then burn full fuel
		fuel -= physicsCart->getThrust()*dt;
		if(fuel < 0){
			//no fuel left, stop accelerating
			physicsCart->setThrust(0);
			fuel = 0;
		}
	} else {
		fuel += dt;
		if(fuel > max_fuel){
			fuel = max_fuel;
		}
	}

	//rotation and scaling
	if(objectRotatingRight){
		if(physicsCart->hasTrack() && objectToBePlaced==SUPPORT_ELEMENT_MASK){
			printf("Turn track right\n");
			track.setTrackRotation(controlPointSelected, track.getTrackRotation(controlPointSelected)+(3.14/128));
			generateTrack();
		} else {
			this->rotateObject(Ogre::Radian(Ogre::Degree(15*dt)));
		}
	}
	if(objectRotatingLeft){
		if(physicsCart->hasTrack() && objectToBePlaced==SUPPORT_ELEMENT_MASK){
			printf("Turn track left\n");
			track.setTrackRotation(controlPointSelected, track.getTrackRotation(controlPointSelected)-(3.14/128));
			generateTrack();
		} else {
			this->rotateObject(Ogre::Radian(Ogre::Degree(-15*dt)));
		}
	}
	if(objectScalingUp){
		this->scaleObject(1.001);
	}
	if(objectScalingDown){
		this->scaleObject(0.999);
	}

	//physics cart:
	if(physicsCart->hasTrack()){
		Vector3d pos = physicsCart->getPos();
		cartNode->setPosition(Ogre::Vector3(pos.x, pos.y, pos.z));

		Ogre::Vector3 mForward = Ogre::Vector3(physicsCart->getForward().x, physicsCart->getForward().y, physicsCart->getForward().z).normalisedCopy();
		Ogre::Vector3 mUp = Ogre::Vector3(physicsCart->getUp().x, physicsCart->getUp().y, physicsCart->getUp().z).normalisedCopy();
		double dot = mForward.dotProduct(mUp);
		if (dot > 0.1) {
 			cout << dot << "! ";
		}

		//Radian yaw = Math::ACos(Vector3::UNIT_X.dotProduct(mForward));
		//Quaternion q3 (Radian(0), Vector3::NEGATIVE_UNIT_Z);
		Ogre::Vector3 mRight = mForward.crossProduct(mUp);
		Ogre::Quaternion tot(mRight, mUp, -mForward);
		cartNode->setOrientation(tot);		//rotate cart

		//jetParticleRight->getEmitter(0)->setDirection(-mForward);
		//jetParticleLeft->getEmitter(0)->setDirection(-mForward);

		//next step
		if(track.getNumberOfPoints() > 3){
			physicsCart->nextStep(dt);
		}

		double cur_t = physicsCart->getCurrentT();
		
		if(cur_t > 0.966){
			finished = true;
			highscore_obj->enterScore(highscore_time);
			physicsCart->setSpeed(0);
			physicsCart->setBraking(1);
		}

		// Update sound engine
		if (cameraName == "CartCam")
			soundEngine->frameStarted(cartNode, dt);
		else if (cameraName == "PlayerCam") {
			;//soundEngine->frameStarted(testNode, dt);	// TODO: insert a scene node for the 3rd person camera here
		}
	}
	
	if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
    {

		std::ostringstream strs;
		strs.precision(1);
		strs << fixed << physicsCart->getSpeed() << " km/t";
		std::string speed = strs.str();

		strs.str("");
		strs << fixed << highscore_time;
		std::string time = strs.str();

		mDetailsPanel->setParamValue(0, time);

		mSpeedBar->setComment(speed);
		mSpeedBar->setCaption("Fuel and Speed");
		
    }

	if(mHighscorePanel->isVisible()){
		std::ostringstream strs;
		strs << highscore_obj->getBest();
		std::string highscore_str = strs.str();

		mHighscorePanel->setParamValue(0, highscore_str);
	}

	if(mSpeedBar->isVisible()){
		mSpeedBar->setProgress(fuel/max_fuel);
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

Ogre::Quaternion Coaster::generateRotationFromDirectionVector(Ogre::Vector3 vDirection)
        {
            // Step 1. Setup basis vectors describing the rotation given the input vector and assuming an initial up direction of (0, 1, 0)
			Ogre::Vector3 vUp = Ogre::Vector3(0.0f, 1.0f, 0.0f);           // Y Up vector
			Ogre::Vector3 vRight = vUp.crossProduct(vDirection);    // The perpendicular vector to Up and Direction (feil, dette er venstre)
			vUp = vDirection.crossProduct(vRight);           // The actual up vector given the direction and the right vector (denne vil vel alltid bli lik opprinnelig vUp)
            

            // Step 2. Put the three vectors into the matrix to bulid a basis rotation matrix
            // This step isnt necessary, but im adding it because often you would want to convert from matricies to quaternions instead of vectors to quaternions
            // If you want to skip this step, you can use the vector values directly in the quaternion setup below
			Ogre::Matrix4 mBasis = Ogre::Matrix4(vRight.x, vRight.y, vRight.z, 0.0f,
												 vUp.x, vUp.y, vUp.z, 0.0f,
												 vDirection.x, vDirection.y, vDirection.z, 0.0f,
												 0.0f, 0.0f, 0.0f, 1.0f);

            return mBasis.extractQuaternion();
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

				if(iter->movable) mCurrentObject = iter->movable->getParentSceneNode();

				float mouseHeightDiff = 0;

				if(mCurrentObject){
				if(adjustHeight){
					CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
					mouseHeightDiff = (mousePos.d_y - CEGUI::MouseCursor::getSingleton().getPosition().d_y);
					//add just height
					Ogre::Vector3 railCurPos = mCurrentObject->getPosition();
					mCurrentObject->setPosition(railCurPos.x, railCurPos.y+mouseHeightDiff, railCurPos.z);
					//generateTrack();

				} else {
					mCurrentObject->setPosition(iter->worldFragment->singleIntersection);
				}

				if(track.getNumberOfPoints() > 3){
					for (vector<Ogre::String>::iterator it = placedObjects.begin(); it!=placedObjects.end(); ++it) {
						if(*it == mCurrentObject->getName()){

							string obj_name = string(*it);
							Ogre::String controll = "Controll";
							int pos = obj_name.find(controll);

							//Controll point clicked
							if(pos != string::npos){
								obj_name.replace(pos, controll.size(), ""); 
								int ctrl_point = std::atoi(obj_name.c_str());
								controlPointSelected = ctrl_point;

								Vector3d v(0,0,0);
								if(adjustHeight){
									Vector3d railCurPos = track.getControlPoint(ctrl_point);
									v = Vector3d(railCurPos.x, railCurPos.y+mouseHeightDiff, railCurPos.z);
								} else {
									v = Vector3d(iter->worldFragment->singleIntersection.x, iter->worldFragment->singleIntersection.y, iter->worldFragment->singleIntersection.z);
								}

								track.setControlPoint(ctrl_point, v);
								generateTrack();

							}
						}
					}
				}

				}
				
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
		//physicsCart->setTrack(&track);
		//physicsCart->moveTo(1.0);
			

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
		mRayScnQuery->setQueryMask(objectToBePlaced);	//will return objects with the query mask in the results
		mRayScnQuery->setQueryTypeMask(Ogre:: SceneManager::ENTITY_TYPE_MASK);
		mRayScnQuery->setSortByDistance(true);

		/*
		This next chunk finds the results of the raycast
		If the mouse is pointing at world geometry we spawn a robot at that position
		*/
		Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter = result.begin();

		bool position_added = false;
 
		for(iter; iter != result.end(); iter++)
		{
			//if you clicked on a robot or ninja it becomes selected
			if(iter->movable && iter->movable->getName().substr(0, 5) != "tile[")
			{
				mCurrentObject = iter->movable->getParentSceneNode();
				printf("Mask: %d \n", mCurrentObject->getAttachedObject(0)->getQueryFlags());

				for (vector<Ogre::String>::iterator it = placedObjects.begin(); it!=placedObjects.end(); ++it) {
					if(*it == mCurrentObject->getName()){

						string obj_name = string(*it);
						Ogre::String controll = "Controll";
						int pos = obj_name.find(controll);

						//Controll point clicked
						if(pos != string::npos){
							obj_name.replace(pos, controll.size(), ""); 
							int ctrl_point = std::atoi(obj_name.c_str());
							controlPointSelected = ctrl_point;
						}
					}
				}

				break;
			}
			//otherwise we spawn a new one at the mouse location
			else if(iter->worldFragment && addNewThing)
			{
				char name[16];
				Ogre::Entity* ent;
				
				switch(objectToBePlaced){
					case SUPPORT_ELEMENT_MASK:
						sprintf(name, "%dControll", mControllPointCount++);
						ent = mSceneMgr->createEntity(name, "support_element.mesh");
						ent->setQueryFlags(SUPPORT_ELEMENT_MASK);

						//add track 20 over the ground
						track.addPos(Vector3d(iter->worldFragment->singleIntersection.x, iter->worldFragment->singleIntersection.y+20, 
							iter->worldFragment->singleIntersection.z), 0);
						position_added = true;
						break;
					case IGLO_MASK:
						sprintf(name, "Iglo%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "iglo.mesh");
						ent->setQueryFlags(IGLO_MASK);
						break;
					case HYTTE_MASK:
						sprintf(name, "Hytte%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "hytte_v3.mesh");
						ent->setQueryFlags(HYTTE_MASK);
						break;
					case STONE12_MASK:
						sprintf(name, "Stone12%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "stone12.mesh");
						ent->setQueryFlags(STONE12_MASK);
						break;
					case STONE17_MASK:
						sprintf(name, "Stone17%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "stone17.mesh");
						ent->setQueryFlags(STONE17_MASK);
						break;
					case STONE117_MASK:
						sprintf(name, "Stone117%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "stone117.mesh");
						ent->setQueryFlags(STONE117_MASK);
						break;
					case PALM_TREE_MASK:
						sprintf(name, "PalmTree%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "Palm.mesh");
						ent->setQueryFlags(PALM_TREE_MASK);
						break;
					case YELLOW_SUB_MASK:
						sprintf(name, "YellowSub%dNode", mCount++);
						ent = mSceneMgr->createEntity(name, "yellow_sub_v8.mesh");
						ent->setQueryFlags(YELLOW_SUB_MASK);
						break;
				}
 
				//attach the object to a scene node
				mCurrentObject = ((Ogre::SceneNode*)(mSceneMgr->getRootSceneNode()->getChild("world")))->createChildSceneNode(std::string(name) , iter->worldFragment->singleIntersection);
				mCurrentObject->attachObject(ent);
 
				placedObjects.push_back(std::string(name));

				//lets shrink the object, only because the terrain is pretty small
				if(objectToBePlaced == PALM_TREE_MASK){
					mCurrentObject->pitch(Ogre::Degree(-90));
					mCurrentObject->setScale(1.5f, 1.5f, 1.5f);
				} else {
					mCurrentObject->setScale(0.1f, 0.1f, 0.1f);
				}
				break;
			}
		}
		if(position_added && track.getNumberOfPoints() > 3){

			generateTrack();
			
		}
 
		//now we show the bounding box so the user can see that this object is selected
		if(mCurrentObject && objectToBePlaced != RAIL_MASK)
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

void Coaster::nextObject(void){
	objectToBePlaced = objectToBePlaced<<1;
	if(objectToBePlaced == END_MASK){
		objectToBePlaced = IGLO_MASK;
	}
	if(objectToBePlaced == RAIL_MASK){
		nextObject();
	}
	printf("%s obj\n", queryFlagMap[objectToBePlaced].c_str());
}

void Coaster::prevObject(void){
	objectToBePlaced = objectToBePlaced>>1;
	if(objectToBePlaced == 0){
		objectToBePlaced = END_MASK<<1;
	}

	if(objectToBePlaced == RAIL_MASK){
		prevObject();
	}
	printf("%s obj\n", queryFlagMap[objectToBePlaced].c_str());
}

void Coaster::generateTrack(void){
	if(track.getNumberOfPoints() > 3){
	meshManager = mRoot->getMeshManager();

	railNode->detachAllObjects();
	if(mSceneMgr->hasEntity("Rails")){
		mSceneMgr->destroyEntity("Rails");
		mSceneMgr->destroyManualObject("RailMesh");
		meshManager->remove("RailMesh");
	}

	// make graphical track mesh
	GraphicTrack::createRailMesh(&track, false);
			
	physicsCart->setTrack(&track);

	// add rails to scene
	Ogre::Entity* rail = mSceneMgr->createEntity("Rails","RailMesh");
	rail->setQueryFlags(RAIL_MASK);
	railNode->attachObject(rail);

	//start cart from start
	physicsCart->moveTo(0.0);
	highscore_time = 0;
	Vector3d start_pos = physicsCart->getPos();
	Ogre::Vector3 start_pos_ogre = Ogre::Vector3(start_pos.x, start_pos.y, start_pos.z);
	cartNode->setPosition(start_pos_ogre);

	printf("Rail generated\n");
	}
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

void Coaster::rotateObject(Ogre::Radian rad){
	if(mCurrentObject){
		mCurrentObject->yaw(rad);
	}
}

void Coaster::scaleObject(float scaleRatio){
	if(mCurrentObject){
		Ogre::Vector3 scale = mCurrentObject->getScale();
		mCurrentObject->setScale(scale*scaleRatio);
	}
}

void Coaster::debugExport()
{
	std::vector<Ogre::SceneNode *> nodes;
	std::ofstream sceneData("scene");
	std::ofstream trackData("track");

	Ogre::SceneNode *root = (Ogre::SceneNode *)mSceneMgr->getRootSceneNode()->getChild("world");

	for (short node = 0; node < root->numChildren(); node++)
		nodes.push_back((Ogre::SceneNode *) root->getChild(node));
	
	track.dump(std::cout);
	track.dump(trackData);
	exportScene(nodes, std::cout);
	exportScene(nodes, sceneData);
}

void Coaster::debugImport()
{
		std::ifstream tData("track");
		std::ifstream sData("scene");
		track = Track();
		track.read(tData);
		importScene(sData);
		physicsCart->setTrack(&track);
		generateTrack();
		mCurrentObject = NULL;
}
bool Coaster::keyPressed(const OIS::KeyEvent& arg)
{
	if(editorMode){
	switch (arg.key) {
		case OIS::KC_F5:
		case OIS::KC_X:
			debugExport();
			break;
		case OIS::KC_F9:
		case OIS::KC_I:
			debugImport();
			break;
		case OIS::KC_SPACE:
			editorMode = !editorMode;
			if(editorMode){
				printf("Edit mode\n");
				}
			changeViewPoint();
			break;
		case OIS::KC_0: 
			this->physicsCart->moveTo(0);
			highscore_time = 0;
			break;
		case OIS::KC_1: this->physicsCart->moveTo(0.1*track.getTrackLength()); break;
		case OIS::KC_2: this->physicsCart->moveTo(0.2*track.getTrackLength()); break;
		case OIS::KC_3: this->physicsCart->moveTo(0.3*track.getTrackLength()); break;
		case OIS::KC_4: this->physicsCart->moveTo(0.4*track.getTrackLength()); break;
		case OIS::KC_5: this->physicsCart->moveTo(0.5*track.getTrackLength()); break;
		case OIS::KC_6: this->physicsCart->moveTo(0.6*track.getTrackLength()); break;
		case OIS::KC_7: this->physicsCart->moveTo(0.7*track.getTrackLength()); break;
		case OIS::KC_8: this->physicsCart->moveTo(0.8*track.getTrackLength()); break;
		case OIS::KC_9: this->physicsCart->moveTo(0.9*track.getTrackLength()); break;
		case OIS::KC_M:
			physicsCart->setBraking(0.0);
			this->physicsCart->setThrust(1.0); 
			cout << "Accelerating.\n";
			break;
		case OIS::KC_N:
			this->physicsCart->setThrust(0.0);
			this->physicsCart->setBraking(1.0);
			cout << "Breaking.\n";
			break;
		case OIS::KC_P:
			cout << physicsCart->toString(); break;

		case OIS::KC_Q:
			if(adjustHeight){
				objectScalingUp = true;
			} else {
				objectRotatingLeft = true;
			}
			break;
		case OIS::KC_E:
			if(adjustHeight){
				objectScalingDown = true;
			} else {
				objectRotatingRight = true;
			}
			break;
		case OIS::KC_R:
			this->resetRail();
			break;
		case OIS::KC_UP:
			this->nextObject();
			break;
		case OIS::KC_DOWN:
			this->prevObject();
			break;
		case OIS::KC_S:
			break;
		case OIS::KC_RSHIFT:
			physicsCart->setBraking(0);
			physicsCart->setThrust(0);
			cout << "Coasting.\n";
			break;
		case OIS::KC_LSHIFT:
			adjustHeight = true;
			cout << "Adjust height start.\n";
			break;
		case OIS::KC_LCONTROL:
			addNewThing = true;
			break;
	}

	} else {
	switch (arg.key) {
		case OIS::KC_SPACE:
			editorMode = !editorMode;
			if(editorMode){
				printf("Edit mode\n");
				}
			changeViewPoint();
			break;
		case OIS::KC_0: 
		case OIS::KC_R:
			this->physicsCart->moveTo(0);
			highscore_time = 0;
			break;
		case OIS::KC_5:
			this->physicsCart->moveTo(0.5*track.getTrackLength()); break;
		case OIS::KC_W:
			physicsCart->setBraking(0.0);
			physicsCart->setThrust(1.0); 
			cout << "Accelerating.\n";
			break;
		case OIS::KC_S:
			physicsCart->setThrust(0.0);
			physicsCart->setBraking(1.0);
			cout << "Breaking.\n";
			break;
		case OIS::KC_LSHIFT:
			physicsCart->setBraking(0);
			physicsCart->setThrust(0);
			cout << "Coasting.\n";
			break;
	}
	}
 
	//then we return the base app keyPressed function so that we get all of the functionality
	//and the return value in one line
	return BaseApplication::keyPressed(arg);
}

void Coaster::resetRail(void){
	meshManager = mRoot->getMeshManager();

	railNode->detachAllObjects();
	if(mSceneMgr->hasEntity("Rails")){
		mSceneMgr->destroyEntity("Rails");
		mSceneMgr->destroyManualObject("RailMesh");
		meshManager->remove("RailMesh");
	}

	vector<Ogre::String> temp = vector<Ogre::String>(0);

	for (vector<Ogre::String>::iterator it = placedObjects.begin(); it!=placedObjects.end(); ++it) {

		string obj_name = string(*it);
		Ogre::String controll = "Controll";
		int pos = obj_name.find(controll);

		//Controll point found
		if(pos != string::npos){
			if(mSceneMgr->hasEntity(obj_name)){
				mSceneMgr->destroyEntity(obj_name);
			}
		} else {
			temp.push_back(*it);
		}
	}
	placedObjects.clear();
	placedObjects = temp;

	if(mCurrentObject)
	{
		mCurrentObject->showBoundingBox(false);
	}

	track.initValues();

	finished = false;
}


bool Coaster::keyReleased( const OIS::KeyEvent& arg ){

	if(editorMode){
	switch (arg.key) {
		case OIS::KC_LSHIFT:
			adjustHeight = false;
			cout << "Adjust height end.\n";
			break;
		case OIS::KC_Q:
			if(adjustHeight){
				objectScalingUp = false;
			} else {
				objectRotatingLeft = false;
			}
			break;
		case OIS::KC_E:
			if(adjustHeight){
				objectScalingDown = false;
			} else {
				objectRotatingRight = false;
			}
			break;
		case OIS::KC_LCONTROL:
			addNewThing = false;
			break;
	}
	} else {
	switch (arg.key) {
		case OIS::KC_W:
			physicsCart->setThrust(0);
			break;
		case OIS::KC_S:
			physicsCart->setBraking(0);
			break;

	}
	}
	return BaseApplication::keyReleased(arg);
}


// Export scene/entity to &out in following format:
// {
// sceneName
// entityName
// meshName
// position
// rotation
// queryflags
// scale
// }

void Coaster::exportOgreEntity(Ogre::SceneNode *scene, Ogre::Entity *ent, std::ostream &out)
{
	// Values to export
	out << "{" << std::endl;
	out << scene->getName() << std::endl;
	out << ent->getName() << std::endl;
	out << ent->getMesh().get()->getName() << std::endl; // Thankyouogre
	Vector3d pos(scene->getPosition());
	pos.dump(out);
	Ogre::Quaternion rotation = scene->getOrientation();
	out << rotation.w << " " << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
	out << ent->getQueryFlags() << std::endl;
	Vector3d(scene->getScale()).dump(out);
	out << "}" << std::endl;
}

void Coaster::exportScene(std::vector<Ogre::SceneNode *> nodes, std::ostream &out)
{
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->numAttachedObjects() > 0)
			if (nodes[i]->getName() != "RailsNode" && nodes[i]->getName() != "cartNode")
				exportOgreEntity(nodes[i], (Ogre::Entity *) nodes[i]->getAttachedObject(0), out);
	}
}

Ogre::SceneNode *Coaster::readOgreSceneNode(std::istream &in)
{
	char verify;

	in >> verify;
	if (verify != '{') // Bad, do something about this?
		;

	std::string sName, eName, mName;
	Vector3d pos, scale;
	Ogre::Quaternion rotation;
	int queryFlags;

	in >> sName;
	in >> eName;
	in >> mName;
	pos.read(in);
	in >> rotation.w >> rotation.x >> rotation.y >> rotation.z;
	in >> queryFlags;
	scale.read(in);

	in >> verify;
	if (verify != '}') 
		;

	if(mSceneMgr->hasEntity(eName)){
		mSceneMgr->destroyEntity(eName);
	}
	placedObjects.push_back(eName);
	mCount++;

	if(eName.find("Controll") != string::npos){
		mControllPointCount++;
	}

	Ogre::SceneNode *node = ((Ogre::SceneNode *) mSceneMgr->getRootSceneNode()->getChild("world"))->createChildSceneNode(sName, pos.ogreVector());
	node->setOrientation(rotation);
	Ogre::Entity *ent = mSceneMgr->createEntity(eName, mName);
	ent->setQueryFlags(queryFlags);
	node->attachObject(ent);
	node->setScale(scale.ogreVector());

	return node;
}

std::vector<Ogre::SceneNode *> Coaster::importScene(std::istream &in)
{
	std::vector<Ogre::SceneNode *> sceneNodes;

	((Ogre::SceneNode *) mSceneMgr->getRootSceneNode()->getChild("world"))->removeAndDestroyAllChildren();

	for (vector<Ogre::String>::iterator it = placedObjects.begin(); it!=placedObjects.end(); ++it) {
		if(mSceneMgr->hasEntity(*it)){
			mSceneMgr->destroyEntity(*it);
		}
	}

	placedObjects.clear();
	mCount = 0;
	mControllPointCount = 0;

	while (in) {
		char term;
		while ((in >> term) && term != '{') ;
		if (term == '{')
			in.unget();
		 else
			return sceneNodes;
		if (in)
			sceneNodes.push_back(readOgreSceneNode(in));
	}
	return sceneNodes;
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
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
			MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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