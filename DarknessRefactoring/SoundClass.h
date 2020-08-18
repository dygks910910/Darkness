#pragma once

class SoundClass
{
protected:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundClass() {};
	SoundClass(const SoundClass&) {};
	virtual ~SoundClass() {};

	virtual bool Initialize(HWND)PURE;
	virtual void Shutdown()PURE;


protected:
	IDirectSound8* m_DirectSound = nullptr;
	IDirectSoundBuffer* m_primaryBuffer = nullptr;
	IDirectSoundBuffer8* m_secondaryBuffer1 = nullptr;
};

