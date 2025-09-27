#pragma once

#include <vector>   // to store raw frame data as a dynamic array of bytes
#include <cstdint>  // for fixed-width integer types like uint8_t
#include <string>   // (reserved) in case we want to add frame labels, metadata, or logging
#include <chrono>   // to timestamp frames using system clock
#include <iostream> // for logging/debug output with logInfo()

class Frame
{
public:
    // Constructor: creates an empty frame
    Frame();
};