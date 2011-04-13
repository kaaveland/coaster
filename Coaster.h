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
#include <OgreTextAreaOverlayElement.h>

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"


using namespace std;
 
class Coaster : public BaseApplication
{
public:
    Coaster(void);
    virtual ~Coaster(void);

	Ogre::Quaternion generateRotationFromDirectionVector(Ogre::Vector3 vDirection);
 
	enum QueryFlags
	{
		IGLO_MASK = 1<<0,
		HYTTE_MASK = 1<<1,
		RAIL_MASK  = 1<<2,
		CART_MASK  = 1<<3,
		ISLAND_MASK = 1<<4,
		CAMERA_MASK = 1<<5,
		STONE12_MASK = 1<<6,
		STONE17_MASK = 1<<7,
		STONE117_MASK = 1<<8,
		PALM_TREE_MASK = 1<<9,
		SUPPORT_ELEMENT_MASK = 1<<10,
		YELLOW_SUB_MASK = 1<<11,
		END_MASK = 1<<12
	};
	std::map<int, std::string> queryFlagMap;

	//end mask sier hvilket objekt som er siste i listen, increase!

	int objectToBePlaced;

	void prevObject(void);
	void nextObject(void);

	#define _def_SkyBoxNum 3

	// Hydrax pointer
	Hydrax::Hydrax *mHydrax;

	Ogre::String mSkyBoxes[_def_SkyBoxNum];
	Ogre::Vector3 mSunPosition[_def_SkyBoxNum];
	Ogre::Vector3 mSunColor[_def_SkyBoxNum];

	int mCurrentSkyBox;
	bool finished;

	// Just to wshow skyboxes information
	Ogre::TextAreaOverlayElement* mTextArea;
	static float rnd_(const float& min, const float& max);
	void changeSkyBox();
	void createTextArea();
	Ogre::ParticleSystem* jetParticleRight;
	Ogre::ParticleSystem* jetParticleLeft;
 
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
 
	bool editorMode;				//if true we place robots in the world

	void showWin32Console(void);
	Track track;
	Ogre::SceneNode *railNode;
	Ogre::SceneNode *cartNode;

	Ogre::Real highscore_time;
	Ogre::Real max_fuel;
	Ogre::Real fuel; // seconds of fuel active

	PhysicsCart *physicsCart;

	Ogre::String cameraName;
	void changeViewPoint(void);
	std::vector<Ogre::String> placedObjects;
	void generateTrack(void);

	Ogre::MeshManager *meshManager;
	bool adjustHeight;
	int controlPointSelected;

	void resetRail(void);
	void rotateObject(Ogre::Radian rad);
	void scaleObject(float scale);

	bool objectRotatingRight;
	bool objectRotatingLeft;
	bool objectScalingUp;
	bool objectScalingDown;
	bool addNewThing;

	
	//void createRailMesh(const Track track, const bool export_mesh);

	SoundEngine *soundEngine;
	Ogre::SceneNode *testNode;
};

 
#endif // #ifndef __Coaster_h_