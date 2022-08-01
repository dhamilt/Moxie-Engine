#include "glPCH.h"
#include "MainMenu.h"

MainMenu::MainMenu()
{
	name = typeid(*this).name();
}

void MainMenu::Paint()
{
	if (ImGui::BeginMainMenuBar())
	{
		CreateMenuBar();
		ImGui::EndMainMenuBar();
	}
	else
		ImGui::EndMainMenuBar();

}

void MainMenu::CreateMenuBar()
{
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("Exit Application", NULL, &exitApp);
		ImGui::EndMenu();
	}

}

bool MainMenu::operator==(GUI_Base& other)
{
	return GetName() == other.GetName();
}

bool MainMenu::IsExitingApplication()
{
	return exitApp;
}
