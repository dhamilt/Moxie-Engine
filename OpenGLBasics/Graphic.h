#pragma once
#include <typeinfo>
class Graphic
{
public:
	Graphic();
	~Graphic();
	virtual void Draw(mat4 projection, mat4 view) =0;
	

	virtual bool operator==(const Graphic& other) = 0;
public:
	int id;
	std::string type;
private:
	static int _id;
};

