#pragma once

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>
#include <cstdint>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING
// A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

struct ControllerState {
    int16_t leftThumb;
    int16_t rightThumb;

    bool operator==(const ControllerState& other)
    {
        return (leftThumb == other.leftThumb && rightThumb == other.rightThumb);
    }
    bool operator!=(const ControllerState& other)
    {
        return !operator==(other);
    }
};

// XBOX Controller Class Definition
class CXBOXController
{
private:
    XINPUT_STATE _controllerState;
    int _controllerNum;
public:
    CXBOXController(int playerNumber);
    XINPUT_STATE GetState();
    bool IsConnected();
    void Vibrate(int leftVal = 0, int rightVal = 0);
};