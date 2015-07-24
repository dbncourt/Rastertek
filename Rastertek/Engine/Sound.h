///////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Class name: Sound
///////////////////////////////////////////////////////////////////////////////
class Sound
{

private:
	struct WaveHeaderType
	{
		char chunkId[4];
		ULONG chunkSize;
		char format[4];
		char subChunkId[4];
		ULONG subChunkSize;
		USHORT audioFormat;
		USHORT numChannels;
		ULONG sampleRate;
		ULONG bytesPerSecond;
		USHORT blockAlign;
		USHORT bitsPerSample;
		char dataChunkId[4];
		ULONG dataSize;
	};

	IDirectSound8* m_directSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer;

public:
	Sound();
	Sound(const Sound& other);
	~Sound();

	bool Initialize(HWND hwnd);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND hwnd);
	void ShutdownDirectSound();

	bool LoadWaveFile(char* fileName, IDirectSoundBuffer8** secondaryBuffer);
	void ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer);

	bool PlayWaveFile();
};

#endif