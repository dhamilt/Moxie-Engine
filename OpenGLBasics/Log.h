#pragma once
#include "glPCH.h"
#include <string>
#include <imgui.h>
#include "GUI_Base.h"
#include <typeinfo>

enum class MessageTypes
{
	Message,
	Warning,
	Error,
	Fatal
};

struct MessageInfo
{
	MessageTypes messageType = MessageTypes::Message;
	std::string message = "Default Message";
};

class Log : public GUI_Base
{
public:
	
	Log() {	name = typeid(*this).name(); Message(MessageTypes::Message, "Window created.");};
		

	
	static MessageTypes MessageType;
	void Message(MessageTypes type, const char* message, ...);
	void Message(MessageTypes type, const char* message, const char* fileName, int line, const char* functionName, ...);
	virtual void Paint()override;
	virtual bool operator==( GUI_Base& other)override;
private:
	//void CreateWindow();
	
	Log(const Log&) { name = typeid(*this).name(); }
	void operator=(const Log&) { name = typeid(*this).name(); };
public:
	bool isWindowOpen;
	 std::vector<MessageInfo> logMessages;
	 
};

