#ifndef __Coaster_h_
#define __Coaster_h_

#include <iostream>
#include <string>
#include <fstream>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include "BaseApplication.h"
#include "Track.h"
#include "Vector3d.h"
#include "GraphicTrack.h"
#include "PhysicsCart.h"
#include "SoundEngine.h"
 
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

using namespace std;
 
class Coaster : public BaseApplication
{
public:
    Coaster(void);
    virtual ~Coaster(void);

	Ogre::Quaternion generateRotationFromDirectionVector(Ogre::Vector3 vDirection);
 
	enum QueryFlags
	{
		NINJA_MASK = 1<<0,
		ROBOT_MASK = 1<<1,
		RAIL_MASK  = 1<<2,
		CART_MASK  = 1<<3,
		ISLAND_MASK = 1<<4,
		CAMERA_MASK = 1<<5
	};
 
protected:
	virtual void exportOgreEntity(Ogre::SceneNode *scene, Ogre::Entity *ent, std::ostream &out);
	Ogre::SceneNode *readOgreSceneNode(std::istream &in);

	virtual void exportScene(std::vector<Ogre::SceneNode *> nodes, std::ostream &out);
	virtual std::vector<Ogre::SceneNode *> importScene(std::istream &in);
	void debugExport();
	void debugImport();

    virtual void createScene(void);
 
	virtual void chooseSceneManager(void);
	virtual void createFrameListener(void);
 
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& arg);
 
	virtual bool mouseMoved(const OIS::MouseEvent& arg);
	virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent& arg);
	virtual bool keyReleased( const OIS::KeyEvent& evt );
 
	Ogre::SceneNode *mCurrentObject;	//pointer to our currently selected object
	Ogre::RaySceneQuery* mRayScnQuery;	//pointer to our ray scene query
	CEGUI::Renderer* mGUIRenderer;		//our CEGUI renderer
 
	bool bLMouseDown, bRMouseDown;	//true if mouse buttons are held down
	int mCount;						//number of objects created
	int mControllPointCount;
	float mRotateSpeed;				//the rotation speed for the camera
 
	bool bRobotMode;				//if true we place robots in the world

	void showWin32Console(void);
	Track track;
	Ogre::SceneNode *railNode;
	Ogre::SceneNode *cartNode;

	Ogre::Real highscore_time;

	PhysicsCart *physicsCart;

	Ogre::String cameraName;
	void changeViewPoint(void);
	std::vector<Ogre::String> placedObjects;
	void Coaster::generateTrack(void);

	Ogre::MeshManager *meshManager;
	bool adjustHeight;
	int controlPointSelected;

	void resetRail(void);
	//void createRailMesh(const Track track, const bool export_mesh);

	SoundEngine *soundEngine;
	Ogre::SceneNode *testNode;
};

 
#endif // #ifndef __Coaster_h_