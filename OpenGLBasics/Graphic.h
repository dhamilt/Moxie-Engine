#pragma once
class Graphic
{
public:
	Graphic();
	virtual void Draw(mat4 projection, mat4 view) =0;
	

	bool operator==(const Graphic& other) { return memcmp(this, &other, sizeof(other)); }
};

