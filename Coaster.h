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
#include "VectorTools.h"
#include "Vector3d.h"
#include "GraphicTrack.h"
 
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

using namespace std;
 
class Coaster : public BaseApplication
{
public:
    Coaster(void);
    virtual ~Coaster(void);
 
	enum QueryFlags
	{
		NINJA_MASK = 1<<0,
		ROBOT_MASK = 1<<1
	};
 
protected:
    virtual void createScene(void);
 
	virtual void chooseSceneManager(void);
	virtual void createFrameListener(void);
 
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& arg);
 
	virtual bool mouseMoved(const OIS::MouseEvent& arg);
	virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent& arg);
 
	Ogre::SceneNode *mCurrentObject;	//pointer to our currently selected object
	Ogre::RaySceneQuery* mRayScnQuery;	//pointer to our ray scene query
	CEGUI::Renderer* mGUIRenderer;		//our CEGUI renderer
 
	bool bLMouseDown, bRMouseDown;	//true if mouse buttons are held down
	int mCount;						//number of objects created
	float mRotateSpeed;				//the rotation speed for the camera
 
	bool bRobotMode;				//if true we place robots in the world

	void showWin32Console(void);

	//void createRailMesh(const Track track, const bool export_mesh);
};
 
#endif // #ifndef __Coaster_h_