#include "AMC4030_UI.h"

#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

#include <userint.h>

int main()
{
	int error = UIENoError;
	AMC4030_UI_Object* obj = AMC4030_UI_Object_Create("AMC4030_Panel.uir", 1, &error);
	if (!obj) {
		goto Error;
	}
	errChk(AMC4030_UI_Object_StartUI(obj));
	AMC4030_UI_Object_Delete(obj);
	CloseCVIRTE();
Error:
	return error;
}