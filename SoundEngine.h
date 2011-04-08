#pragma once

#include "Vector3d.h"
#include "PhysicsCart.h"
#include "Ogre.h"
#include "Track.h"
#include "SoundManager.h"
#include <map>

using std::map;

class SoundEngine
{
	
public:
	SoundEngine(PhysicsCart *cart, Ogre::SceneNode *cartNode);
	~SoundEngine(void);

	enum SOUNDCLIP
	{
		BLIZZARD01,
		WIND1,
		WIND2,
		WIND3,
		WIND4,
		SCREAM1,
		SCREAM2,
		SCREAM3,
		SCREAM4,
		SCREAM5,
		CRASH1,
		TRAIN_SLOW,
		TRAIN_FAST,
		LAUGH1
	};

	int addSound(SOUNDCLIP clip, Ogre::SceneNode *node);
	void frameStarted(Ogre::SceneNode *listener, Ogre::Real timeElapsed);
	void playBackgroundSounds(bool play);
	
private:

	
	PhysicsCart *physicsCart;
	int trainSlowChannel;
	int trainFastChannel;
	int blizzard1Channel;
	int wind1Channel;
	int wind2Channel;
	int wind3Channel;

	bool hasScreamed1; 
	bool hasScreamed2;
	double timeSinceLastScream;
	bool isPlayingBackgroundSounds;

	Ogre::SceneNode *cartNode;

	bool loadFiles();

	std::map<SOUNDCLIP, const char *> soundFiles;
	std::map<SOUNDCLIP, int> soundIndexes;
	SoundManager *soundManager;

//map<Vector3d, SOUNDCLIP> soundPositions;

};

