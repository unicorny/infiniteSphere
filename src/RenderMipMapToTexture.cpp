#include "main.h"

RenderMipMapToTexture::RenderMipMapToTexture(const char* textureName)
{
	mTexture = DRTextureManager::Instance().getTexture(textureName);
}

RenderMipMapToTexture::~RenderMipMapToTexture()
{


}

DRReturn RenderMipMapToTexture::renderStuff()
{
	return DR_OK;
}