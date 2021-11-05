#include "glPCH.h"
#include "AxisCommand.h"

void AxisCommand::Subscribe(std::function<void(int)> callback)
{
	axisEvent += callback;
}

void AxisCommand::Unsubscribe(std::function<void(int)> callback)
{
	axisEvent -= callback;
}

void AxisCommand::Execute(int axisVal)
{
	axisEvent.Broadcast(axisVal);
}
