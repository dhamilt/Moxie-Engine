#include "glPCH.h"
#include "StandaloneWindow.h"

void StandaloneWindow::Paint()
{
	// Create the Standalone Window
	ImGui::Begin(windowType.c_str(), &isOpen);
	// Fill the inner ui of the standalone window
	FillWindow();

}
