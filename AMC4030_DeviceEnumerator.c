#include "AMC4030_DeviceEnumerator.h"

const SerialDevice_Enumerator_Vtable ch340_usbserial_port_enumerator = {
	AMC4030_DeviceEnumerator_Create,
	AMC4030_DeviceEnumerator_Destroy,
	AMC4030_DeviceEnumerator_Reset,
	AMC4030_DeviceEnumerator_Next,
	AMC4030_DeviceEnumerator_GetCurrentFriendlyName
};
