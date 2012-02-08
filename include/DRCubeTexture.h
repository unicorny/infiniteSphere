#ifndef __DR_CUBE_TEXTURE__
#define __DR_CUBE_TEXTURE__

class DRCubeTexture
{
public:
    DRCubeTexture();
    
    DRReturn load(const char* pathName, const char* endung = "jpg", GLint glMinFilter = GL_LINEAR, GLint glMagFilter = GL_LINEAR);
private:
    GLuint mTextureID;
    
    bool mSucessfullLoaded;
};

#endif //__DR_CUBE_TEXTURE__