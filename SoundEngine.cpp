#include "SoundEngine.h"
#include "SoundManager.h"
#include <iostream>
#include <string>
#include <set>
#include <cassert>

using std::cout;

// Value for stepping up wind noise
#define SPEEDSTEP 50.0


SoundEngine::SoundEngine(PhysicsCart *physicsCart, Ogre::SceneNode *cartNode)
{
	hasScreamed1 = false;
	hasScreamed2 = false;
	timeSinceLastScream = 10e6;
	isPlayingBackgroundSounds = false;
	wind1Channel = INVALID_SOUND_CHANNEL;
	wind2Channel = INVALID_SOUND_CHANNEL;
	wind3Channel = INVALID_SOUND_CHANNEL;
	blizzard1Channel = INVALID_SOUND_CHANNEL;

	soundFiles[BLIZZARD01]	= "Blizzard 01.wav";
	soundFiles[WIND1]		= "Wind 01.wav";
	soundFiles[WIND2]		= "Wind 02.wav";
	soundFiles[WIND3]		= "Wind 03.wav";
	soundFiles[WIND4]		= "Wind 04.wav";
	soundFiles[SCREAM1]		= "Screams 01.wav";
	soundFiles[SCREAM2]		= "Screams 02.wav";
	soundFiles[SCREAM3]		= "Screams 03.wav";
	soundFiles[SCREAM4]		= "Screams 04.wav";
	soundFiles[SCREAM5]		= "Screams 05.wav";
	soundFiles[TRAIN_SLOW]	= "Train 01.wav";
	soundFiles[TRAIN_FAST]	= "Train 02.wav";	
	soundFiles[CRASH1]		= "Crash 01.wav";
	soundFiles[LAUGH1]		= "Laughs 01.wav";

	soundManager = new SoundManager();
	soundManager->Initialize();

	this->physicsCart = physicsCart;
	this->cartNode = cartNode;
	
	loadFiles();
}


SoundEngine::~SoundEngine(void)
{

}

bool SoundEngine::loadFiles() {

	std::set<SOUNDCLIP> loopedSounds;
	loopedSounds.insert(BLIZZARD01); loopedSounds.insert(TRAIN_FAST); loopedSounds.insert(TRAIN_SLOW); 
	loopedSounds.insert(WIND1); loopedSounds.insert(WIND2); loopedSounds.insert(WIND3);
	
	

	for (map<SOUNDCLIP, const char*>::const_iterator it = soundFiles.begin(); it != soundFiles.end(); ++it) {

		// Only loop some sounds
		SOUND_TYPE type = (loopedSounds.find(it->first) != loopedSounds.end()) ? SOUND_TYPE_2D_SOUND_LOOPED : SOUND_TYPE_2D_SOUND;	

		// Create sound and store its index
		soundIndexes[it->first]	= soundManager->CreateSound(Ogre::String(it->second), type);
	}
			
	return true;
}

int SoundEngine::addSound(SoundEngine::SOUNDCLIP clip, Ogre::SceneNode *soundNode) 
{
	int channelIndex = INVALID_SOUND_CHANNEL;
	soundManager->PlaySound(soundIndexes[clip], soundNode, &channelIndex);
	return channelIndex;
}

void SoundEngine::playBackgroundSounds(bool play)
{
	if (play == isPlayingBackgroundSounds) return;

	

	if (play) {
		isPlayingBackgroundSounds = true;

		soundManager->PlaySound(soundIndexes[SOUNDCLIP::BLIZZARD01], cartNode, &blizzard1Channel);
		soundManager->GetSoundChannel(blizzard1Channel)->setVolume(0.2);

		soundManager->PlaySound(soundIndexes[SOUNDCLIP::WIND1], cartNode, &wind1Channel);
		soundManager->GetSoundChannel(wind1Channel)->setVolume(0.02);

		soundManager->PlaySound(soundIndexes[SOUNDCLIP::WIND2], cartNode, &wind2Channel);
		soundManager->GetSoundChannel(wind2Channel)->setVolume(0.02);
	} else {
		soundManager->StopSound(&wind1Channel);
		soundManager->StopSound(&wind2Channel);
	}
}

double minmax(double x) { if (x<0.0) return 0.0; if(x > 1.0) return 1.0; return x; }

void SoundEngine::frameStarted(Ogre::SceneNode *listener, Ogre::Real timeElapsed)
{
	int channelIndex = INVALID_SOUND_CHANNEL;
	timeSinceLastScream += timeElapsed;
	
	if (isPlayingBackgroundSounds) {
		soundManager->GetSoundChannel(wind1Channel)->setVolume(0.2 * minmax(physicsCart->getSpeed() / SPEEDSTEP) + 0.01);
		soundManager->GetSoundChannel(wind2Channel)->setVolume(0.2 * minmax(physicsCart->getSpeed() - SPEEDSTEP*0.9 / SPEEDSTEP));
	}


	double gFactor = physicsCart->getGFactor();

	// Do some magic
	if (gFactor >= 2.0 && gFactor < 4.0 && timeSinceLastScream > 10.0) {
		int random = rand() % 100;
		if (random < 20)
			soundManager->PlaySound(soundIndexes[SCREAM1], cartNode, &channelIndex);
		else if (random > 80)
			soundManager->PlaySound(soundIndexes[SCREAM2], cartNode, &channelIndex);

		timeSinceLastScream = 0;
	
	} else if (gFactor > 4.0 && gFactor < 15.0 && timeSinceLastScream > 10.0) {
		int random = rand() % 100;
		if (random < 30)
			soundManager->PlaySound(soundIndexes[SCREAM3], cartNode, &channelIndex);
			
		else if (random > 70)
			
			soundManager->PlaySound(soundIndexes[SCREAM4], cartNode, &channelIndex);
		timeSinceLastScream = 0;

	} else if (gFactor > 15.0 && physicsCart->getSpeed() > 80 && timeSinceLastScream > 5.0) {
		soundManager->PlaySound(soundIndexes[SCREAM5], cartNode, &channelIndex);
		timeSinceLastScream = 0;

	} else if (physicsCart->getSpeed() > 80 && timeSinceLastScream > 10.0) {
		int random = rand() % 100;
		if (random < 50) 
			soundManager->PlaySound(soundIndexes[LAUGH1], cartNode, &channelIndex);

		timeSinceLastScream = 0;
	}
		//soundManager->GetSoundInstance(1)->fmodSound->

 	soundManager->FrameStarted(listener, timeElapsed);
}
