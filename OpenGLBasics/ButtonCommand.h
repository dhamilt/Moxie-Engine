#pragma once
#include "Command.h"
#include "EventHandler.h"

typedef EventHandler<void> ButtonPressedSignature;
typedef EventHandler<void> ButtonReleasedSignature;
typedef EventHandler<void> ButtonHeldSignature;

#define MOX_PRESSED		0
#define MOX_RELEASED	1
#define MOX_HELD		2

class ButtonCommand :
    public Command<int>
{

public:
    ButtonCommand();
	~ButtonCommand();
    
	/// <summary>
	/// Subscribes events to their appropriate delegates
	/// params: callback is the function pointer,
	/// eventType is the numeric value representing the Event Signature
	/// the function pointer is subscribing to
	/// </summary>
	void Subscribe(std::function<void()> callback, int eventType);

	/// <summary>
	/// Unsubscribes events from their appropriate delegates
	/// params: callback is the function pointer,
	/// eventType is the numeric value representing the Event Signature
	/// the function pointer is unsubscribing from
	/// </summary>
	void Unsubscribe(std::function<void()> callback, int eventType);
	void OnPressed();
	bool IsBeingHeld();
	void OnHeld();
	void OnReleased();
	

private:
    virtual void Execute(int param) override;

	/// <summary>
	/// Event delegate for the button press
	 /// </summary>
	ButtonPressedSignature pressed;

	/// <summary>
	/// Event delegate for the button release
	/// </summary>
	ButtonReleasedSignature released;

	/// <summary>
	/// Event delegate for the button being held
	/// </summary>
	ButtonHeldSignature held;

	bool isHeld = false;
};

