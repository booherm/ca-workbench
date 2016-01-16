#include "AudioManager.hpp"

using namespace std;

SampleData AudioManager::inputSampleData;
SampleData AudioManager::outputSampleData;

AudioManager::AudioManager() {

	PaError paStatus = Pa_Initialize();
	if (paStatus != 0)
		throw string("PortAudio initialization exception: " + string(Pa_GetErrorText(paStatus)));

	// initialize input sample data structure
	inputSampleData.frameIndex = 0;
	inputSampleData.maxFrameIndex = 5 * SAMPLE_RATE; // 5 seconds at sample rate (hz)
	inputSampleData.playbackFrameIndex = 0;
	inputSampleData.leftChannelSample.resize(inputSampleData.maxFrameIndex);
	inputSampleData.rightChannelSample.resize(inputSampleData.maxFrameIndex);

	// setup input stream
	PaStreamParameters inputParameters;
	inputParameters.device = Pa_GetDefaultInputDevice();  // 0 for windows sound as input
	if(inputParameters.device == paNoDevice)
		throw string("PortAudio get default input device exception");
	inputParameters.channelCount = 2;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	paStatus = Pa_OpenStream(
		&inputStream,
		&inputParameters,
		NULL,          // output parameters
		SAMPLE_RATE,
		512,           // frames per buffer
		paClipOff,     // no clipping
		inputCallback,
		&inputSampleData
	);
	if (paStatus != 0)
		throw string("PortAudio open input stream exception: " + string(Pa_GetErrorText(paStatus)));

	// initialize output sample data structure
	outputSampleData.frameIndex = 0;
	outputSampleData.maxFrameIndex = 5 * SAMPLE_RATE; // 5 seconds at sample rate
	outputSampleData.leftChannelSample.resize(outputSampleData.maxFrameIndex);
	outputSampleData.rightChannelSample.resize(outputSampleData.maxFrameIndex);

	// setup output stream
	PaStreamParameters outputParameters;
	outputParameters.device = Pa_GetDefaultOutputDevice();  // 5 to force desktop speakers
	if (outputParameters.device == paNoDevice)
		throw string("PortAudio get default output device exception");
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	paStatus = Pa_OpenStream(
		&outputStream,
		NULL,              // input parameters
		&outputParameters,
		SAMPLE_RATE,
		512,               // frames per buffer
		paClipOff,         // no clipping
		outputCallback,
		&inputSampleData //&outputSampleData
	);
	if (paStatus != 0)
		throw string("PortAudio open output stream exception: " + string(Pa_GetErrorText(paStatus)));

}

void AudioManager::startInputStream() {
	PaError paStatus = Pa_StartStream(inputStream);
	if (paStatus != 0)
		throw std::string("PortAudio start input stream exception: " + std::string(Pa_GetErrorText(paStatus)));

	cout << "recording for 5 seconds..." << endl;
	while(Pa_IsStreamActive(inputStream) == 1){
		Pa_Sleep(500);
		cout << "frameIndex = " << inputSampleData.frameIndex << endl;
	}
	cout << "done recording" << endl;
}

void AudioManager::startOutputStream() {
	PaError paStatus = Pa_StartStream(outputStream);
	if (paStatus != 0)
		throw std::string("PortAudio start output stream exception: " + std::string(Pa_GetErrorText(paStatus)));

	cout << "waiting for playback to finish..." << endl;
	while (Pa_IsStreamActive(outputStream) == 1) {
		Pa_Sleep(500);
		cout << "frameIndex = " << outputSampleData.frameIndex << endl;
	}
	cout << "done playing back" << endl;
}

void AudioManager::stopInputStream() {
	PaError paStatus = Pa_StopStream(inputStream);
	if (paStatus != 0)
		throw std::string("PortAudio stop input stream exception: " + std::string(Pa_GetErrorText(paStatus)));

	cout << "dumping recorded vector left channel to file" << endl;
	ofstream outputFile("c:\\ca_workbench_screenshots\\dump.txt");
	for (unsigned long i = 0; i < inputSampleData.maxFrameIndex; i++) {
		outputFile << inputSampleData.leftChannelSample[i] << endl;
	}
	outputFile.close();
}

void AudioManager::stopOutputStream() {
	PaError paStatus = Pa_StopStream(outputStream);
	if (paStatus != 0)
		throw std::string("PortAudio stop output stream exception: " + std::string(Pa_GetErrorText(paStatus)));
}

void AudioManager::shutdown() {
	PaError paStatus = Pa_CloseStream(inputStream);
	if (paStatus != 0)
		throw std::string("PortAudio close input stream exception: " + std::string(Pa_GetErrorText(paStatus)));

	paStatus = Pa_CloseStream(outputStream);
	if (paStatus != 0)
		throw std::string("PortAudio close output stream exception: " + std::string(Pa_GetErrorText(paStatus)));

	paStatus = Pa_Terminate();
	if (paStatus != 0)
		throw std::string("PortAudio termination exception: " + std::string(Pa_GetErrorText(paStatus)));
}

void AudioManager::printDeviceInformation() {
	cout << "PortAudio version: " << Pa_GetVersionText() << endl;

	int deviceCount = Pa_GetDeviceCount();
	cout << "Devices found: " << deviceCount << endl;

	for (int i = 0; i < deviceCount; i++) {
		const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
		cout << "-----------------------------------------------------------------------" << endl;
		cout << "Device Number " << i << ":" << endl;
		cout << "                       Name: " << deviceInfo->name << endl;
		const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
		cout << "    Is Default Input Device: " << (Pa_GetDefaultInputDevice() == i || hostApiInfo->defaultInputDevice == i ? "true" : "false") << endl;
		cout << "   Is Default Output Device: " << (Pa_GetDefaultOutputDevice() == i || hostApiInfo->defaultOutputDevice == i ? "true" : "false") << endl;
		cout << "                   Host API: " << hostApiInfo->name << endl;
		cout << "             Struct Version: " << deviceInfo->structVersion << endl;
		cout << "         Max Input Channels: " << deviceInfo->maxInputChannels << endl;
		cout << "        Max Output Channels: " << deviceInfo->maxOutputChannels << endl;
		cout << "        Default Sample Rate: " << deviceInfo->defaultSampleRate << endl;
		cout << "  Default Low Input Latency: " << deviceInfo->defaultLowInputLatency << endl;
		cout << " Default High Input Latency: " << deviceInfo->defaultHighInputLatency << endl;
		cout << " Default Low Output Latency: " << deviceInfo->defaultLowOutputLatency << endl;
		cout << "Default High Output Latency: " << deviceInfo->defaultHighOutputLatency << endl << endl;
	}
}

// called when an input buffer has been filled and is ready for consumption by the client program
int AudioManager::inputCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData
) {
	SampleData* outputUserData = (SampleData*) userData;
	const float* recordedData = (const float*) inputBuffer;
	unsigned long framesLeft = outputUserData->maxFrameIndex - outputUserData->frameIndex;
	unsigned long framesToPull;
	int finished;
	
	if (framesLeft < framesPerBuffer) {
		framesToPull = framesLeft;
		finished = paComplete;
	}
	else {
		framesToPull = framesPerBuffer;
		finished = paContinue;
	}

	// left and right channel values are interleaved
	if (inputBuffer == NULL) {
		// inject silence
		for (unsigned long i = 0; i < framesToPull; i++) {
			outputUserData->leftChannelSample[outputUserData->frameIndex] = 0.0f;
			outputUserData->rightChannelSample[outputUserData->frameIndex++] = 0.0f;
		}
	}
	else {
		for (unsigned long i = 0; i < framesToPull * 2; i = i + 2) {
			outputUserData->leftChannelSample[outputUserData->frameIndex] = recordedData[i];
			outputUserData->rightChannelSample[outputUserData->frameIndex++] = recordedData[i + 1];
		}
	}

	return finished;
}

// called when the playback buffer has been consumed and it's ready for more client data to be published
int AudioManager::outputCallback(
	const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData
) {
	SampleData* outputUserData = (SampleData*) userData;
	float* outputBufferData = (float*) outputBuffer;
	unsigned long framesLeft = outputUserData->maxFrameIndex - outputUserData->playbackFrameIndex;
	unsigned long framesToPull;
	int finished;

	if (framesLeft < framesPerBuffer) {
		framesToPull = framesLeft;
		finished = paComplete;
	}
	else {
		framesToPull = framesPerBuffer;
		finished = paContinue;
	}

	// left and right channels are interleaved
	for (unsigned long i = 0; i < framesToPull * 2; i = i + 2) {
		*outputBufferData++ = outputUserData->leftChannelSample[outputUserData->playbackFrameIndex];
		*outputBufferData++ = outputUserData->rightChannelSample[outputUserData->playbackFrameIndex++];
	}

	return finished;
}
