#pragma once
#include "GUI_Base.h"
class StandaloneWindow :
    public GUI_Base
{
    StandaloneWindow();
    ~StandaloneWindow();
    virtual void Paint() override;
    virtual void FillWindow() = 0;
    protected:
        std::string windowType = "Window";
        bool isOpen;
};

