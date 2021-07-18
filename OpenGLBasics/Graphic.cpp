#include "glPCH.h"
#include "Graphic.h"
#include "GLSetup.h"
extern GLSetup* GGLSPtr;

int Graphic::_id;
Graphic::Graphic()
{
	id = Graphic::_id;
	Graphic::_id++;
	 GGLSPtr->AddRenderObject(this); 
}

Graphic::~Graphic()
{
	GGLSPtr->RemoveRenderObject(this);
}