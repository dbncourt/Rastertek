///////////////////////////////////////////////////////////////////////////////
// Filename: Sound.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Sound.h"


Sound::Sound()
{
	this->m_directSound = nullptr;
	this->m_primaryBuffer = nullptr;
	this->m_secondaryBuffer = nullptr;
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
	result = LoadWaveFile("sound01.wav", &this->m_secondaryBuffer);
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
	Sound::ShutdownWaveFile(&this->m_secondaryBuffer);

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
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwReserved = 0;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	bufferDesc.lpwfxFormat = nullptr;

	//Get control of the primary sound buffer on the default sound device
	result = this->m_directSound->CreateSoundBuffer(&bufferDesc, &this->m_primaryBuffer, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));
	
	//Setup the format of the primary sound buffer
	//In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (CD audio format)
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//Set the primary buffer to be the wave format specified
	result = this->m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Sound::ShutdownDirectSound()
{
	//Release the primary sound buffer pointer
	if (this->m_primaryBuffer)
	{
		this->m_primaryBuffer->Release();
		this->m_primaryBuffer = nullptr;
	}

	//Release the direct sound interface pointer
	if (this->m_directSound)
	{
		this->m_directSound->Release();
		this->m_directSound = nullptr;
	}
}

bool Sound::LoadWaveFile(char* fileName, IDirectSoundBuffer8** secondaryBuffer)
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
	if (waveFileHeader.numChannels != 2)
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
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));

	//Set the buffer description of the secondary sound buffer that the wave file will be loaded onto
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

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
	
	return true;
}

void Sound::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	//Release the secondary sound buffer
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}
}

bool Sound::PlayWaveFile()
{
	HRESULT result;

	//Set position at the beginning of the sound buffer
	result = this->m_secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	//Set volume of the buffer to 100%
	result = this->m_secondaryBuffer->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	//Play the contents of the secondary sound buffer
	result = this->m_secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING); //Set last value to DSBPLAY_LOOPING for looping.
	if (FAILED(result))
	{
		return false;
	}
	return true;
}