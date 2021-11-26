#pragma once
#include <string>
#include "Renderer.h"
class Texture
{
public:
	Texture(const std::string &path);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void unBind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

private:
	unsigned int m_RendererID;
	std::string m_Filepath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};