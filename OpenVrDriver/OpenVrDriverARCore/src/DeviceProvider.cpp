#include <DeviceProvider.h>
#include <thread>

void GetSensorData(SocketClass* SocketObja, ControllerDriver* left, ControllerDriver* right);
SocketClass* SocketObj;

EVRInitError DeviceProvider::Init(IVRDriverContext* pDriverContext)
{
    EVRInitError initError = InitServerDriverContext(pDriverContext);
    if (initError != EVRInitError::VRInitError_None)
    {
        return initError;
    }

    
    VRDriverLog()->Log("Initializing ARCore controller"); //this is how you log out Steam's log file.


    controllerDriverR = new ControllerDriver();
    controllerDriverR->SetControllerIndex(2);
    VRServerDriverHost()->TrackedDeviceAdded("arcore_controllerR", TrackedDeviceClass_Controller, controllerDriverR); //add all your devices like this.

    controllerDriverL = new ControllerDriver();
    controllerDriverL->SetControllerIndex(1);
    VRServerDriverHost()->TrackedDeviceAdded("arcore_controllerL", TrackedDeviceClass_Controller, controllerDriverL); //add all your devices like this.

    SocketObj = new SocketClass();
    std::thread getSensorDatathread(GetSensorData,SocketObj,controllerDriverL,controllerDriverR);
    getSensorDatathread.detach();

    return vr::VRInitError_None;
}

void GetSensorData(SocketClass* SocketObja,ControllerDriver* left, ControllerDriver* right) {
    SocketObja->Connect(55455);
    while (SocketObja->GetStatus()) {
        char buffer[2048];
        SocketObja->Receive(buffer);
        left->ReadBuffer(buffer);
        right->ReadBuffer(buffer);
    }
}

void DeviceProvider::Cleanup()
{
    SocketObj->CloseSocket();
    delete SocketObj;
    delete controllerDriverL;
    delete controllerDriverR;
    controllerDriverL = NULL;
    controllerDriverR = NULL;
}
const char* const* DeviceProvider::GetInterfaceVersions()
{
    return k_InterfaceVersions;
}

void DeviceProvider::RunFrame()
{
    controllerDriverL->RunFrame();
    controllerDriverR->RunFrame();
}

bool DeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}