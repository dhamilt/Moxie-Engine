#pragma once
template<typename T>
class MathLibrary
{
public:

	static T Lerp(T min, T max, float delta);
};

template<typename T>
inline T MathLibrary<T>::Lerp(T min, T max, float delta)
{
	return min + (max - min) * delta;
}
