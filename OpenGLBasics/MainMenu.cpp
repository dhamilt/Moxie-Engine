#include "glPCH.h"
#include "MainMenu.h"

WMainMenu::WMainMenu()
{
	name = typeid(*this).name();
}

void WMainMenu::Paint()
{
	if (ImGui::BeginMainMenuBar())
	{
		CreateMenuBar();
		ImGui::EndMainMenuBar();
	}
	else
		ImGui::EndMainMenuBar();

}

void WMainMenu::CreateMenuBar()
{
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("Exit Application", NULL, &exitApp);
		ImGui::EndMenu();
	}

}

bool WMainMenu::operator==(GUI_Base& other)
{
	return GetName() == other.GetName();
}

bool WMainMenu::IsExitingApplication()
{
	return exitApp;
}
