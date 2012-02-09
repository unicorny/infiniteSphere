#include "main.h"

DRCubeTexture::DRCubeTexture()
: mTextureID(0), mSucessfullLoaded(false)
{
    
}

DRReturn DRCubeTexture::load(const char* pathName, const char* endung/* = "jpg"*/,
                             GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter/* = GL_LINEAR*/)
{
    std::string fileName[] = {"right", "left", "top", "bottom", "front", "back"};
    
    return DR_OK;
}

