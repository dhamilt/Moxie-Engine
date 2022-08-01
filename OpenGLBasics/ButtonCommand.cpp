#include "glPCH.h"
#include "ButtonCommand.h"
#include "SDL.h"

ButtonCommand::ButtonCommand()
{
	pressed = ButtonPressedSignature();
	released = ButtonReleasedSignature();
	held = ButtonHeldSignature();
}

ButtonCommand::~ButtonCommand()
{
	pressed.UnsubscribeAll();
	released.UnsubscribeAll();
	held.UnsubscribeAll();
}

void ButtonCommand::Subscribe(std::function<void()> callback, int eventType)
{
	if (eventType == MOX_PRESSED)
		pressed += callback;

	if (eventType == MOX_RELEASED)
		released += callback;

	if (eventType == MOX_HELD)
		held += callback;
}

void ButtonCommand::Unsubscribe(std::function<void()> callback, int eventType)
{
	if (eventType == MOX_PRESSED)
		pressed -= callback;

	if (eventType == MOX_RELEASED)
		released -= callback;

	if (eventType == MOX_HELD)
		held -= callback;
}

void ButtonCommand::OnPressed()
{	
	isHeld = true;
	Execute(MOX_PRESSED);
}

bool ButtonCommand::IsBeingHeld()
{
	return isHeld;
}

void ButtonCommand::OnHeld()
{
	Execute(MOX_HELD);
}

void ButtonCommand::OnReleased()
{
	// Once released, the button is no longer being held
	isHeld = false;
	Execute(MOX_RELEASED);
}

void ButtonCommand::Execute(int param)
{
	if (param == MOX_PRESSED)
		pressed.Broadcast();

	if (param == MOX_RELEASED)
		released.Broadcast();

	if (param == MOX_HELD)
		held.Broadcast();
}
