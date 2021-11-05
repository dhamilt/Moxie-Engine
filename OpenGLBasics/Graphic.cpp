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
	currentRenderingPass = 0;
	SetMultipass(false);
}

Graphic::~Graphic()
{
	GGLSPtr->RemoveRenderObject(this);
}

void Graphic::GetCurrentRenderPass(uint8_t& currentPass)
{
	currentPass = currentRenderingPass;
}

void Graphic::SetMultipass(bool isMultipassRendering)
{
	multipassRendering = isMultipassRendering;
}

bool Graphic::IsMultipass()
{
	return multipassRendering;
}

void Graphic::SetRenderingPass(const uint8_t& currentPass)
{
	currentRenderingPass = currentPass;
}
