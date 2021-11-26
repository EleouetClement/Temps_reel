#include "Texture.h"




Texture::Texture(const std::string& path)
	: m_RendererID(0), m_Filepath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

Texture::~Texture()
{

}

void Texture::Bind(unsigned int slot /*= 0*/) const
{

}

void Texture::unBind() const
{

}

