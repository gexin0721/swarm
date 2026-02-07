#include "AudioCapture.h"
#include <iostream>

AudioCapture::AudioCapture(int sampleRate, int channels)
    : sampleRate_(sampleRate), channels_(channels), audioHandle_(nullptr), isRunning_(false) {
}

AudioCapture::~AudioCapture() {
    stop();
}

void AudioCapture::start() {
    std::cout << "Starting audio capture..." << std::endl;
    isRunning_ = true;
    // TODO: Initialize audio capture (ALSA/PortAudio)
}

void AudioCapture::stop() {
    if (isRunning_) {
        std::cout << "Stopping audio capture..." << std::endl;
        isRunning_ = false;
        // TODO: Cleanup audio resources
    }
}

void AudioCapture::setCallback(AudioCallback callback) {
    callback_ = callback;
}
