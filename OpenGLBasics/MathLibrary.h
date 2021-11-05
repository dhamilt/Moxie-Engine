#pragma once
template<typename T>
class MathTemplateLibrary
{
public:
	static T Lerp(T min, T max, float delta);

};
class MathLibrary
{
public:

	static int Lerp(int min, int max, float delta);
	static float Lerp(float min, float max, float delta);
};

template<typename T>
inline T MathTemplateLibrary<T>::Lerp(T min, T max, float delta)
{
	return min + (max - min) * delta;
}

inline int MathLibrary::Lerp(int min, int max, float delta)
{
	return min + (int)((max - min) *delta);
}

inline float MathLibrary::Lerp(float min, float max, float delta)
{
	return min + (max - min) * delta;
}
