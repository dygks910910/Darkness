#pragma once
#include "SoundClass.h"
class Sound3D :
    public SoundClass
{
public:
    Sound3D();
    Sound3D(const Sound3D&);
    virtual ~Sound3D();

    // SoundClass을(를) 통해 상속됨
    virtual bool Initialize(HWND) override;
    virtual void Shutdown() override;
private:
    bool InitializeDirectSound(HWND);
    void ShutdownDirectSound();

    bool LoadWaveFile(const char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
    void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);

    bool PlayWaveFile();

private:
    IDirectSound3DListener8* m_listener = nullptr;
    IDirectSound3DBuffer8* m_secondary3DBuffer1 = nullptr;


};

