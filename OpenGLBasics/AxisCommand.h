#pragma once
#include "Command.h"
#include "EventHandler.h"

typedef EventHandler<void, int> AxisSignature;
class AxisCommand :
    public Command<int>
{
public:
    /// <summary>
    /// Subscribe function pointers to the axisEvent delegate
    /// </summary>
    void Subscribe(std::function<void(int)> callback);


    /// <summary>
    /// Unsubscribe function pointers from the axisEvent delegate
    /// </summary>
    void Unsubscribe(std::function<void(int)> callback);

    /// <summary>
    /// Broadcasts axis event to all subscribed functions
    /// </summary>
    /// <param name="axisVal">param that will be passed into the broadcast</param>
    void Execute(int axisVal);
private:
    /// <summary>
    /// Event delegate for the axis event
    /// </summary>
    AxisSignature axisEvent;
};

