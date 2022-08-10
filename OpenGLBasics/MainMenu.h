#pragma once
#include "GUI_Base.h"
class WMainMenu :
    public GUI_Base
{
public:
    WMainMenu();
    virtual void Paint() override;
    virtual void CreateMenuBar();
    virtual bool operator==(GUI_Base& other) override;
    bool IsExitingApplication();
private:
    bool exitApp = false;
};

