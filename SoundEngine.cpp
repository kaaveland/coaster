#include "SoundEngine.h"
#include <iostream>
#include <string>
#include <cassert>
#include "Framework.h"

using std::cout;

#define SOUNDFOLDER "sound/"
#define NUM_BUFFERS 2
#define NUM_SOURCES 2


SoundEngine::SoundEngine()
{
	ALFWInit();

	listenerPos = Vector3d(0,0,0);
	listenerUp = Vector3d(0,1,0);
	listenerVel = Vector3d(0,0,0);
	//soundPositions();

	soundFiles[BLIZZARD01] = "c:/blizzard01.wav";
	//soundFiles[CRASH01] = "nocrash01.wav";

	ALfloat pos_[3] = {listenerPos.x, listenerPos.y, listenerPos.z};
	ALfloat vel_[3] = {0,0,0};
	ALfloat ori_[6] = {0,0,0,0,1,0};

	alListenerfv(AL_POSITION,pos_);
   	alListenerfv(AL_VELOCITY,vel_);
   	alListenerfv(AL_ORIENTATION,ori_);
    
    alGetError(); // clear any error messages

	//alutInit(0,0);
	
	// Load the sound files into buffers
	//loadFiles();

}


SoundEngine::~SoundEngine(void)
{
	alDeleteSources(sources.size(), (ALuint*)sources[0]);
	alDeleteBuffers(soundBuffers.size(), (ALuint*)soundBuffers[SOUNDCLIP(0)]);
	ALFWShutdownOpenAL();
	ALFWShutdown();
}

bool SoundEngine::loadFiles() {

	ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;

	ALuint Buffer;
	alGenBuffers(1, &Buffer);

	if (alGetError() != AL_NO_ERROR)
		cout << "Error generating sound buffers!\n";

	// Load the files into buffers
	for (map<SOUNDCLIP, const char *>::const_iterator it = soundFiles.begin(); it != soundFiles.end(); ++it) {
			
		if (alGetError() != AL_NO_ERROR)
			cout << "Error loading audio file " << it->second << ".\n";

		if (!ALFWLoadWaveToBuffer((char*)it->second, Buffer))
		{
			ALFWprintf("Failed to load %s\n", it->second);
		}

		//alutLoadWAVFile("c:\blizzard01.wav", &format, &data, &size, &freq, &loop);
		//alBufferData(Buffer, format, data, size, freq);
		//alutUnloadWAV(format, data, size, freq);

		// Create and store buffer
		soundBuffers[it->first] = Buffer; //alutCreateBufferFromFile("c:/blizzard01.wav");
		
	}

	return true;
}

ALboolean SoundEngine::LoadALData()
{
    // Variables to load into.

	return false;

  
}

void SoundEngine::addSound(SoundEngine::SOUNDCLIP clip, Vector3d pos) 
{
	loadFiles();

	ALuint Source;

	// Bind buffer with a source.
    alGenSources(1, &Source);
	sources.push_back(Source);

    if (alGetError() != AL_NO_ERROR)
        cout << "Error generating source for sound clip.\n";

    alSourcei (Source, AL_BUFFER,   soundBuffers[clip]   );
    alSourcef (Source, AL_PITCH,    1.0f     );
    alSourcef (Source, AL_GAIN,     1000.0f     );

	ALfloat pos_[3] = {pos.x, pos.y, pos.z};
	ALfloat vel_[3] = {0,0,0};	// The sources are stationary
	alSourcefv(Source, AL_POSITION, pos_);
    alSourcefv(Source, AL_VELOCITY, vel_);
    alSourcei (Source, AL_LOOPING,  AL_TRUE);


	alSourcePlay(Source);
	//alSourc

	cout << "Error in openAL: " << alGetError() << "\n";

}

void SoundEngine::setListenerPosition(Vector3d pos)
{
	listenerPos = pos;
}

void SoundEngine::setListenerUp(Vector3d up)
{
	listenerUp = up;
}

void SoundEngine::setListenerVelocity(Vector3d velocity)
{
	listenerVel = velocity;
}
