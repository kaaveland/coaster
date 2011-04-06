#pragma once

#include "Vector3d.h"
#include "PhysicsCart.h"
#include "Track.h"
#include <conio.h>
#include <stdlib.h>
#include <al.h>
#include <alc.h>
//#include <alu.h>
#include <AL/alut.h>
#include <map>
#include "Framework.h"

using std::map;



class SoundEngine
{
	
public:
	SoundEngine(void);
	~SoundEngine(void);

	enum SOUNDCLIP
	{
		BLIZZARD01,
		CRASH01
	};

	void addSound(SOUNDCLIP clip, Vector3d pos);
	void setListenerPosition(Vector3d pos);
	void setListenerUp(Vector3d up);
	void setListenerVelocity(Vector3d velocity);

private:


map<SOUNDCLIP, const char *> soundFiles;
map<SOUNDCLIP, ALuint> soundBuffers;
vector<ALuint> sources;

map<Vector3d, SOUNDCLIP> soundPositions;

ALboolean LoadALData();
bool loadFiles();
	// Buffers hold sound data.
//ALuint Buffer;

// Sources are points emitting sound.
//ALuint Source;

// Position of the source sound.
//ALfloat SourcePos[3];

// Velocity of the source sound.
//ALfloat SourceVel[3];


// Position of the listener.
Vector3d listenerPos, listenerVel, listenerUp;

// Velocity of the listener.
//ALfloat ListenerVel[3];

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
//ALfloat ListenerOri[6];



//= { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

};

