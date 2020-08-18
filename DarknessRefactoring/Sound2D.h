#pragma once
#include "SoundClass.h"
class Sound2D :
    public SoundClass
{
public:
	virtual bool Initialize(HWND)	override;
	virtual void Shutdown()			override;

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(const char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool PlayWaveFile();
};

