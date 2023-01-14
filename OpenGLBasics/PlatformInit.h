#pragma once
class GameLoop;
class PPlatformInit
{
protected:
	virtual bool IsSupported() = 0;
	virtual bool InitializePlatform() = 0;
	friend class GameLoop;
};

