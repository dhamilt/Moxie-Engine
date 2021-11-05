#include "glPCH.h"
#include "GUI_Base.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
int GUI_Base::_id;
GUI_Base::GUI_Base()
{
	id = GUI_Base::_id;
	GUI_Base::_id++;
	GGLSPtr->AddUIElement(this);
}


GUI_Base::~GUI_Base()
{
	GGLSPtr->RemoveUIElement(this);
}

std::string GUI_Base::GetName()
{
	return name;
}
