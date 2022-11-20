#pragma once
#include "SocketClass.h"
#include <openvr_driver.h>
#include <windows.h>


using namespace vr;

/**
This class controls the behavior of the controller. This is where you 
tell OpenVR what your controller has (buttons, joystick, trackpad, etc.).
This is also where you inform OpenVR when the state of your controller 
changes (for example, a button is pressed).

For the methods, take a look at the comment blocks for the ITrackedDeviceServerDriver 
class too. Those comment blocks have some good information.

This example driver will simulate a controller that has a joystick and trackpad on it.
It is hardcoded to just return a value for the joystick and trackpad. It will cause 
the game character to move forward constantly.
**/
class ControllerDriver : public ITrackedDeviceServerDriver
{
public:

	void ReadBuffer(char* buffer);

	void SetControllerIndex(int32_t CtrlIndex);
	/**
	Initialize your controller here. Give OpenVR information 
	about your controller and set up handles to inform OpenVR when 
	the controller state changes.
	**/
	EVRInitError Activate(uint32_t unObjectId);

	/**
	Un-initialize your controller here.
	**/
	void Deactivate();

	/**
	Tell your hardware to go into stand-by mode (low-power).
	**/
	void EnterStandby();

	/**
	Take a look at the comment block for this method on ITrackedDeviceServerDriver. So as far 
	as I understand, driver classes like this one can implement lots of functionality that 
	can be categorized into components. This class just acts as an input device, so it will 
	return the IVRDriverInput class, but it could return other component classes if it had 
	more functionality, such as maybe overlays or UI functionality.
	**/
	void* GetComponent(const char* pchComponentNameAndVersion);

	/**
	Refer to ITrackedDeviceServerDriver. I think it sums up what this does well.
	**/
	void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);

	/**
	Returns the Pose for your device. Pose is an object that contains the position, rotation, velocity, 
	and angular velocity of your device.
	**/
	DriverPose_t GetPose();

	/**
	You can retrieve the state of your device here and update OpenVR if anything has changed. This 
	method should be called every frame.
	**/
	void RunFrame();


private:
	DriverPose_t pose;
	int32_t ControllerIndex;
	uint32_t driverId;
	VRInputComponentHandle_t joystickYHandle;
	VRInputComponentHandle_t trackpadYHandle;
	VRInputComponentHandle_t joystickXHandle;
	VRInputComponentHandle_t trackpadXHandle;
	VRInputComponentHandle_t TriggerHandle;
	VRInputComponentHandle_t TriggerClickHandle;
	VRInputComponentHandle_t GripHandle;
	VRInputComponentHandle_t AHandle;
	VRInputComponentHandle_t BHandle;
	VRInputComponentHandle_t XHandle;
	VRInputComponentHandle_t YHandle;
	VRInputComponentHandle_t SystemHandle;

	float posx = 0;
	float posy = 0;
	float posz = 0;
	float rotx = 0;
	float roty = 0;
	float rotz = 0;
	float rotw = 0;
	float trigger = 0;
	float grip = 0;
	float joyx = 0;
	float joyy = 0;
	float abutton = 0;
	float bbutton = 0;
	float sys = 0;

};