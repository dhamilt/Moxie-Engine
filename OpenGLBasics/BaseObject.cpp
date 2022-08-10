#include "glPCH.h"
#include "BaseObject.h"
#include "GameLoop.h"

extern GameLoop* GGLPtr;
BaseObject::BaseObject()
{
	transform = Transform();
	(*GGLPtr) += this;
}

BaseObject::BaseObject(DVector3 pos)
{
	transform = Transform(pos);
	(*GGLPtr) += this;
}

BaseObject::BaseObject(DVector3 pos, DVector3 scale)
{
	transform = Transform(pos, scale);
	(*GGLPtr) += this;
}

BaseObject::BaseObject(DVector3 pos, DVector3 aor, float angle, DVector3 scale)
{
	transform = Transform(pos, aor, angle, scale);
	(*GGLPtr) += this;
}
