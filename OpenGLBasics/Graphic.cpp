#include "glPCH.h"
#include "Graphic.h"
#include "GLSetup.h"
extern GLSetup* GGLSPtr;
Graphic::Graphic()
{
	 (*GGLSPtr) += this; 
}
