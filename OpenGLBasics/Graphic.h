#pragma once
#include <typeinfo>
class Graphic
{
public:
	Graphic();
	~Graphic();
	virtual void Draw(mat4 projection, mat4 view) =0;
	virtual bool operator==(const Graphic& other) = 0;
	void GetCurrentRenderPass(uint8_t& currentPass);
	void SetMultipass(bool isMultipassRendering);
	bool IsMultipass();
private:
	void SetRenderingPass(const uint8_t& currentPass);
public:
	int id;

	std::string type;
private:
	static int _id;
	uint8_t currentRenderingPass;
	bool multipassRendering;
	friend class GLSetup;
};

