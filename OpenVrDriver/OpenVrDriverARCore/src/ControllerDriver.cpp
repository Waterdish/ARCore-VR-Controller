#include <ControllerDriver.h>
#include <rapidxml.hpp>
#include <thread>

using namespace rapidxml;

//SocketClass* SocketObj;

//void GetSensorData();

void ControllerDriver::ReadBuffer(char* buffer) {
	if (buffer[1] == ((ControllerIndex==1)?'L':'R')) {
		try {
			xml_document<> doc;
			doc.parse<0>(buffer);
			xml_node<>* node = doc.first_node("Position");
			xml_node<>* camx = node->first_node("x");
			xml_node<>* camy = node->first_node("y");
			xml_node<>* camz = node->first_node("z");
			xml_node<>* node1 = doc.first_node("Rotation");
			xml_node<>* camrotx = node1->first_node("x");
			xml_node<>* camroty = node1->first_node("y");
			xml_node<>* camrotz = node1->first_node("z");
			xml_node<>* camrotw = node1->first_node("w");
			xml_node<>* triggernode = doc.first_node("Trig");
			xml_node<>* gripnode = doc.first_node("Grip");
			xml_node<>* joynode = doc.first_node("Joy");
			xml_node<>* joyxnode = joynode->first_node("x");
			xml_node<>* joyynode = joynode->first_node("y");
			xml_node<>* anode = doc.first_node("A");
			xml_node<>* bnode = doc.first_node("B");
			xml_node<>* sysnode = doc.first_node("S");

			sscanf_s(camx->value(), "%f", &posx);
			sscanf_s(camy->value(), "%f", &posy);
			sscanf_s(camz->value(), "%f", &posz);
			sscanf_s(camrotx->value(), "%f", &rotx);
			sscanf_s(camroty->value(), "%f", &roty);
			sscanf_s(camrotz->value(), "%f", &rotz);
			sscanf_s(camrotw->value(), "%f", &rotw);
			sscanf_s(triggernode->value(), "%f", &trigger);
			sscanf_s(gripnode->value(), "%f", &grip);
			sscanf_s(joyxnode->value(), "%f", &joyx);
			sscanf_s(joyynode->value(), "%f", &joyy);
			sscanf_s(anode->value(), "%f", &abutton);
			sscanf_s(bnode->value(), "%f", &bbutton);
			sscanf_s(sysnode->value(), "%f", &sys);
			//std::cout << posx << " " << posy << " " << posz << "\n";
		}
		catch (...) {

		}
	}
}

void ControllerDriver::SetControllerIndex(int32_t CtrlIndex)
{
	ControllerIndex = CtrlIndex;
}

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
	//SocketObj = new SocketClass();
	//std::thread getSensorDatathread(GetSensorData);
	//getSensorDatathread.detach();
	pose = { 0 };

	driverId = unObjectId; //unique ID for your driver

	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId); //this gets a container object where you store all the information about your driver

	VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "{arcore}/input/controller_profile.json"); //tell OpenVR where to get your driver's Input Profile
	VRProperties()->SetInt32Property(props, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_Controller); //tells OpenVR that it's a controller

	switch (ControllerIndex) {
	case 1:
		vr::VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, TrackedControllerRole_LeftHand);
		VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 10001);
		break;
	case 2:
		vr::VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);
		VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 10000);
		break;
	}
	//VRProperties()->SetInt32Property(props, vr::Prop_ControllerHandSelectionPriority_Int32, 10000);
	
	
	// Set up a render model path
	VRProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String, "{htc}/rendermodels/vr_tracker_vive_1_0");
	//set tracking image
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String, "{htc}/icons/tracker_status_ready.png");
	
	//set all other tracking images
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String, "{htc}/icons/tracker_status_off.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String, "{htc}/icons/tracker_status_searching.gif");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{htc}/icons/tracker_status_searching_alert.gif");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{htc}/icons/tracker_status_ready_alert.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String, "{htc}/icons/tracker_status_off.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String, "{htc}/icons/tracker_status_standby.png");
	VRProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String, "{htc}/icons/tracker_status_ready_low.png");

	vr::VRDriverInput()->CreateBooleanComponent(props, "/input/grip/click", &GripHandle);
	vr::VRDriverInput()->CreateBooleanComponent(props, "/input/trigger/click", &TriggerClickHandle);
	vr::VRDriverInput()->CreateBooleanComponent(props, "/input/system/click", &SystemHandle);


	vr::VRDriverInput()->CreateScalarComponent(
		props, "/input/trigger/value", &TriggerHandle,
		vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided
	);

	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

	switch (ControllerIndex) {
	case 1:
		vr::VRDriverInput()->CreateBooleanComponent(props, "/input/x/click", &XHandle);
		vr::VRDriverInput()->CreateBooleanComponent(props, "/input/y/click", &YHandle);
		break;
	case 2:
		vr::VRDriverInput()->CreateBooleanComponent(props, "/input/a/click", &AHandle);
		vr::VRDriverInput()->CreateBooleanComponent(props, "/input/b/click", &BHandle);
		break;
	}



	/*
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send joystick commands to OpenVR with, in the Y direction (forward/backward)
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send trackpad commands to OpenVR with, in the Y direction
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarType_Absolute? Take a look at the comments on EVRScalarType.
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarUnits_NormalizedTwoSided? Take a look at the comments on EVRScalarUnits.
	*/
	//The following properites are ones I tried out because I saw them in other samples, but I found they were not needed to get the sample working.
	//There are many samples, take a look at the openvr_header.h file. You can try them out.

	//VRProperties()->SetUint64Property(props, Prop_CurrentUniverseId_Uint64, 2);
	//VRProperties()->SetBoolProperty(props, Prop_HasControllerComponent_Bool, true);
	//VRProperties()->SetBoolProperty(props, Prop_NeverTracked_Bool, true);
	//VRProperties()->SetInt32Property(props, Prop_Axis0Type_Int32, k_eControllerAxis_TrackPad);
	//VRProperties()->SetInt32Property(props, Prop_Axis2Type_Int32, k_eControllerAxis_Joystick);
	//VRProperties()->SetStringProperty(props, Prop_SerialNumber_String, "example_controler_serial");
	//VRProperties()->SetStringProperty(props, Prop_RenderModelName_String, "vr_controller_vive_1_5");
	//uint64_t availableButtons = ButtonMaskFromId(k_EButton_SteamVR_Touchpad) |
	//	ButtonMaskFromId(k_EButton_IndexController_JoyStick);
	//VRProperties()->SetUint64Property(props, Prop_SupportedButtons_Uint64, availableButtons);

	return VRInitError_None;
}



/*void GetSensorData() {
	switch (ControllerIndex) {
	case 1:
		SocketObj->Connect(55355);
		break;
	case 2:
		SocketObj->Connect(55455);
		break;
	}
	while (SocketObj->GetStatus()) {
		char buffer[2048];
		SocketObj->Receive(buffer);
		if (buffer[0] == '<') {
			try {
				xml_document<> doc;
				doc.parse<0>(buffer);
				xml_node<>* node = doc.first_node("Position");
				xml_node<>* camx = node->first_node("x");
				xml_node<>* camy = node->first_node("y");
				xml_node<>* camz = node->first_node("z");
				xml_node<>* node1 = doc.first_node("Rotation");
				xml_node<>* camrotx = node1->first_node("x");
				xml_node<>* camroty = node1->first_node("y");
				xml_node<>* camrotz = node1->first_node("z");
				xml_node<>* camrotw = node1->first_node("w");
				xml_node<>* triggernode = doc.first_node("Trig");
				xml_node<>* gripnode = doc.first_node("Grip");
				xml_node<>* joynode = doc.first_node("Joy");
				xml_node<>* joyxnode = joynode->first_node("x");
				xml_node<>* joyynode = joynode->first_node("y");
				xml_node<>* anode = doc.first_node("A");
				xml_node<>* bnode = doc.first_node("B");

				sscanf_s(camx->value(), "%f", &posx);
				sscanf_s(camy->value(), "%f", &posy);
				sscanf_s(camz->value(), "%f", &posz);
				sscanf_s(camrotx->value(), "%f", &rotx);
				sscanf_s(camroty->value(), "%f", &roty);
				sscanf_s(camrotz->value(), "%f", &rotz);
				sscanf_s(camrotw->value(), "%f", &rotw);
				sscanf_s(triggernode->value(), "%f", &trigger);
				sscanf_s(gripnode->value(), "%f", &grip);
				sscanf_s(joyxnode->value(), "%f", &joyx);
				sscanf_s(joyynode->value(), "%f", &joyy);
				sscanf_s(anode->value(), "%f", &abutton);
				sscanf_s(bnode->value(), "%f", &bbutton);
				//std::cout << posx << " " << posy << " " << posz << "\n";
			}
			catch (...) {

			}
		}
	}
}*/

DriverPose_t ControllerDriver::GetPose()
{

	 //This example doesn't use Pose, so this method is just returning a default Pose.

	//comes from https://github.com/SlimeVR/SlimeVR-OpenVR-Driver/blob/main/src/IVRDevice.hpp make default pose
	
	pose.deviceIsConnected = true;
	pose.poseIsValid = true;
	pose.result = true ? vr::ETrackingResult::TrackingResult_Running_OK : vr::ETrackingResult::TrackingResult_Running_OutOfRange;
	pose.willDriftInYaw = false;
	pose.shouldApplyHeadModel = false;
	pose.qDriverFromHeadRotation.w = pose.qWorldFromDriverRotation.w = pose.qRotation.w = 1.0;

	pose.vecPosition[0] = posx;
	pose.vecPosition[1] = posy;
	pose.vecPosition[2] = posz;

	pose.qRotation.w = rotw;
	pose.qRotation.x = rotx;
	pose.qRotation.y = roty;
	pose.qRotation.z = rotz;

	return pose;
}

void ControllerDriver::RunFrame()
{
	//Since we used VRScalarUnits_NormalizedTwoSided as the unit, the range is -1 to 1.
	/*VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0); //move forward
	VRDriverInput()->UpdateScalarComponent(trackpadYHandle, 0.95f, 0); //move foward
	VRDriverInput()->UpdateScalarComponent(joystickXHandle, 0.0f, 0); //change the value to move sideways
	VRDriverInput()->UpdateScalarComponent(trackpadXHandle, 0.0f, 0); */
	VRServerDriverHost()->TrackedDevicePoseUpdated(this->driverId, GetPose(), sizeof(vr::DriverPose_t));
	VRDriverInput()->UpdateBooleanComponent(GripHandle, grip == 1, 0);
	VRDriverInput()->UpdateScalarComponent(TriggerHandle, trigger, 0);
	VRDriverInput()->UpdateBooleanComponent(TriggerClickHandle, trigger==1, 0);
	VRDriverInput()->UpdateScalarComponent(joystickYHandle, joyy, 0);
	VRDriverInput()->UpdateScalarComponent(joystickXHandle, joyx, 0);
	VRDriverInput()->UpdateBooleanComponent(SystemHandle, sys == 1, 0);
	switch (ControllerIndex) {
	case 1:
		vr::VRDriverInput()->UpdateBooleanComponent(XHandle, abutton == 1, 0);
		vr::VRDriverInput()->UpdateBooleanComponent(YHandle, bbutton == 1, 0);
		break;
	case 2:
		vr::VRDriverInput()->UpdateBooleanComponent(AHandle, abutton == 1, 0);
		vr::VRDriverInput()->UpdateBooleanComponent(BHandle, bbutton == 1, 0);
		break;
	}
	//change the value to move sideways
	//this is where i will want to update the pose.
}

void ControllerDriver::Deactivate()
{
	driverId = k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	//I found that if this method just returns null always, it works fine. But I'm leaving the if statement in since it doesn't hurt.
	//Check out the IVRDriverInput_Version declaration in openvr_driver.h. You can search that file for other _Version declarations 
	//to see other components that are available. You could also put a log in this class and output the value passed into this 
	//method to see what OpenVR is looking for.
	if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
	{
		return this;
	}
	return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) 
{
	if (unResponseBufferSize >= 1)
	{
		pchResponseBuffer[0] = 0;
	}
}