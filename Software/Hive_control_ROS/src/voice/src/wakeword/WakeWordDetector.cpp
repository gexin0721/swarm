#include "WakeWordDetector.h"
#include <iostream>

WakeWordDetector::WakeWordDetector(const std::string& modelPath)
    : modelPath_(modelPath), detectorHandle_(nullptr), isRunning_(false) {
}

WakeWordDetector::~WakeWordDetector() {
    stop();
}

void WakeWordDetector::start() {
    std::cout << "Starting wake word detector..." << std::endl;
    isRunning_ = true;
    // TODO: Initialize wake word detection
}

void WakeWordDetector::stop() {
    if (isRunning_) {
        std::cout << "Stopping wake word detector..." << std::endl;
        isRunning_ = false;
        // TODO: Cleanup wake word detector
    }
}

void WakeWordDetector::setCallback(WakeWordCallback callback) {
    callback_ = callback;
}
