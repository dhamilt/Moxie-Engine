#pragma once
#include "glPCH.h"
#include <functional>

template<typename functionType, typename... params>
class EventHandler
{
public:
	typedef std::function<functionType(params...)> FunctionSignature;
	void Broadcast(params...);
	void UnsubscribeAll();
	void operator+=(FunctionSignature& fs);
	void operator-=(FunctionSignature& fs);
	int subscriberCount = 0;
private:
	std::vector<FunctionSignature> events;
};

template<typename functionType, typename ...params>
inline void EventHandler<functionType, params...>::Broadcast(params... args)
{	
		for (int i = 0; i < events.size(); i++)
			events[i](args...);
}

template<typename functionType, typename ...params>
inline void EventHandler<functionType, params...>::UnsubscribeAll()
{
	if(events.empty())
		events.clear();
}

template<typename functionType, typename ...params>
inline void EventHandler<functionType, params...>::operator+=(FunctionSignature& fs)
{
	events.push_back(fs);

	subscriberCount++;
}

template<typename functionType, typename ...params>
inline void EventHandler<functionType, params...>::operator-=(FunctionSignature& fs)
{
	int eraseIndex = -1;
	int currentIndex = 0;
	while(currentIndex < events.size())
	{
			if (fs.target_type().name() == events[currentIndex].target_type().name())
			{
				eraseIndex = currentIndex;
				break;
			}
			currentIndex++;		
	}
	

	if (eraseIndex > -1)
	{
		events.erase(events.begin() + eraseIndex);
		subscriberCount--;
	}	
}
