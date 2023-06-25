#pragma once
template<typename T>
class MathLibrary
{
public:
	static T Clamp(T min, T max, T val);
	static T Lerp(T min, T max, float delta);
};

template<typename T>
inline T MathLibrary<T>::Lerp(T min, T max, float delta)
{
	return min + (max - min) * delta;
}


template<typename T>
inline T MathLibrary<T>::Clamp(T min, T max, T val)
{
	if (val > max)
		return max;
	if (val < min)
		return min;
	return val;
}