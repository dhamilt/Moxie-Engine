#pragma once
#include <imgui.h>
#include <string>

class GUI_Base
{
public:
	GUI_Base();
	~GUI_Base();
	virtual void Paint() = 0;
	virtual bool operator==(GUI_Base& other) = 0;
	std::string GetName();
public:
	int id;
protected:
	std::string name;
private:
	static int _id;

};

