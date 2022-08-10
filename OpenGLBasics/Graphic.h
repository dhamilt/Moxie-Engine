#pragma once
#include <typeinfo>
class Graphic
{
public:
	Graphic();
	~Graphic();
	virtual void Draw(DMat4x4 projection, DMat4x4 view) =0;
	

	virtual bool operator==(const Graphic& other) = 0;
public:
	int id;
	std::string type;
private:
	static int _id;
};

