///////////////////////////////////////////////////////////////////////////////
// Filename: Sound.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Sound.h"


Sound::Sound()
{
	this->m_directSound = nullptr;
	this->m_primaryDirectSoundBuffer = nullptr;
	this->directSound3DListener = nullptr;
	this->m_secondaryDirectSoundBuffer = nullptr;
	this->m_secondaryDirectSound3DBuffer = nullptr;
}

Sound::Sound(const Sound& other)
{
}


Sound::~Sound()
{
}

bool Sound::Initialize(HWND hwnd)
{
	bool result;

	//Initialize direct sound and the primary sound buffer
	result = Sound::InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	//Load a wave audio file onto a secondary buffer
	result = Sound::LoadWaveFile("sound02.wav", &this->m_secondaryDirectSoundBuffer, &this->m_secondaryDirectSound3DBuffer);
	if (!result)
	{
		return false;
	}

	//Play the wave file now that it has been loaded
	result = Sound::PlayWaveFile();
	if (!result)
	{
		return false;
	}
	return true;
}

void Sound::Shutdown()
{
	//Release the secondary buffer
	Sound::ShutdownWaveFile(&this->m_secondaryDirectSoundBuffer, &this->m_secondaryDirectSound3DBuffer);

	//Shutdown the Direct Sound API
	Sound::ShutdownDirectSound();
}

bool Sound::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;

	//Initialize the direct sound interface pointer for the default sound device
	result = DirectSoundCreate8(nullptr, &this->m_directSound, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Set the cooperative level to priority so the format of the primary sound buffer can be modified
	result = this->m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));

	//Setup the primary buffer description
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwReserved = 0;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	bufferDesc.lpwfxFormat = nullptr;

	//Get control of the primary sound buffer on the default sound device
	result = this->m_directSound->CreateSoundBuffer(&bufferDesc, &this->m_primaryDirectSoundBuffer, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));

	//Setup the format of the primary sound buffer
	//In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (CD audio format)
	waveFormat.cbSize = 0;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;

	//Set the primary buffer to be the wave format specified
	result = this->m_primaryDirectSoundBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	// Obtain a listener interface.
	result = this->m_primaryDirectSoundBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&this->directSound3DListener);
	if (FAILED(result))
	{
		return false;
	}

	// Set the initial position of the listener to be in the middle of the screen.
	this->directSound3DListener->SetPosition(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
	
	return true;
}

void Sound::ShutdownDirectSound()
{
	// Release the IDirectSound3DListener pointer.
	if (this->directSound3DListener)
	{
		this->directSound3DListener->Release();
		this->directSound3DListener = nullptr;
	}

	// Release the primary DirectSoundBuffer pointer.
	if (this->m_primaryDirectSoundBuffer)
	{
		this->m_primaryDirectSoundBuffer->Release();
		this->m_primaryDirectSoundBuffer = nullptr;
	}

	// Release the DirectSound Interface pointer.
	if (this->m_directSound)
	{
		this->m_directSound->Release();
		this->m_directSound = nullptr;
	}
}

bool Sound::LoadWaveFile(char* fileName, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	HRESULT result;

	//Open the wave file in binary
	FILE* filePtr;
	int error = fopen_s(&filePtr, fileName, "rb");
	if (error != 0)
	{
		return false;
	}

	//Read in the wave file header
	WaveHeaderType waveFileHeader;
	UINT count = fread(&waveFileHeader, sizeof(WaveHeaderType), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//Check that the chunk ID is the RIFF format
	if (
		(waveFileHeader.chunkId[0] != 'R') ||
		(waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') ||
		(waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	//Check if the file format is the WAVE format
	if (
		(waveFileHeader.format[0] != 'W') ||
		(waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') ||
		(waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	//Check that the sub chunk ID is the fmt format
	if (
		(waveFileHeader.subChunkId[0] != 'f') ||
		(waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') ||
		(waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	//Check that the audio format is WAVE_FORMAT_PCM
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	//Check that the wave file was recorded in stereo format
	if (waveFileHeader.numChannels != 1)
	{
		return false;
	}

	//Check that the wave file was recorded at a sample rate of 44.1KHz
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	//Ensure that the wave file was recorded in 16 bit format
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	//Check for the data chunk header
	if (
		(waveFileHeader.dataChunkId[0] != 'd') ||
		(waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') ||
		(waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));

	//Set the wave format of secondary buffer that this wave file will be loaded onto
	waveFormat.cbSize = 0;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));

	//Set the buffer description of the secondary sound buffer that the wave file will be loaded onto
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwReserved = 0;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	bufferDesc.lpwfxFormat = &waveFormat;

	//Create a temporary sound buffer with the specific buffer settings
	IDirectSoundBuffer* tempBuffer;
	result = this->m_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Test the buffer format against the direct sound 8 interface and create the secondary buffer
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the temporary buffer
	tempBuffer->Release();
	tempBuffer = nullptr;

	//Move to the beginning of the wave data which starts at the end of the data chunk header
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	//Create a temporary buffer to hold the wave file data
	UCHAR* waveData = new UCHAR[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	//Read in the wave file data into the newly created buffer
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	//Close the file once done reading
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//Lock the secondary buffer to write wave data into it
	UCHAR* bufferPtr = nullptr;
	ULONG bufferSize;
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, nullptr, nullptr, 0);
	if (FAILED(result))
	{
		return false;
	}

	//Copy the wave data into the buffer
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//Unlock the secondary buffer after the data has been written to it
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, nullptr, 0);
	if (FAILED(result))
	{
		return false;
	}

	//Release the wave data since it was copied into the secondary buffer
	delete[] waveData;
	waveData = nullptr;

	// Get the 3D interface to the secondary sound buffer
	result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Sound::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	// Release the IDirectSound3DBuffer pointer.
	if (*secondary3DBuffer)
	{
		(*secondary3DBuffer)->Release();
		(*secondary3DBuffer) = nullptr;
	}
	// Release the IDirectSoundBuffer pointer.
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		(*secondaryBuffer) = nullptr;
	}
}

bool Sound::PlayWaveFile()
{
	HRESULT result;

	D3DXVECTOR3 position = D3DXVECTOR3(-2.0f, 0.0f, 0.0f);

	//Set position at the beginning of the sound buffer
	result = this->m_secondaryDirectSoundBuffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	//Set volume of the buffer to 100%
	result = this->m_secondaryDirectSoundBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	// Set the 3D position of the sound
	this->m_secondaryDirectSound3DBuffer->SetPosition(position.x, position.y, position.z, DS3D_IMMEDIATE);

	//Play the contents of the secondary sound buffer
	result = this->m_secondaryDirectSoundBuffer->Play(0, 0, 0); //Set last value to DSBPLAY_LOOPING for looping.
	if (FAILED(result))
	{
		return false;
	}
	return true;
}