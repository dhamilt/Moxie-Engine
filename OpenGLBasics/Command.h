#pragma once
template<typename T>
class Command
{
public:
	~Command() {};	
	virtual void Execute(T param) = 0;
};

