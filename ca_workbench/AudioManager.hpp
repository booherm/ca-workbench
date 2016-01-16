#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "portaudio.h"

const int SAMPLE_RATE = 8000;  // telephone quality
//const int SAMPLE_RATE = 32000;  // fm radio quality
//const int SAMPLE_RATE = 44100;  // cd quality
//const int SAMPLE_RATE = 192000; // blueray quality

typedef struct {
	unsigned long frameIndex;
	unsigned long maxFrameIndex;
	unsigned long playbackFrameIndex;
	std::vector<float> leftChannelSample;
	std::vector<float> rightChannelSample;
} SampleData;

class AudioManager {

public:
	AudioManager();
	void printDeviceInformation();
	void startInputStream();
	void stopInputStream();
	void startOutputStream();
	void stopOutputStream();
	void shutdown();

private:

	PaStream* inputStream;
	PaStream* outputStream;
	static SampleData inputSampleData;
	static SampleData outputSampleData;

	static int inputCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData
	);

	static int outputCallback(
		const void *inputBuffer,
		void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData
	);

};

#endif
