#ifndef __SC_RENDER_NOISE_PLANET_TO_TEXTURE__
#define __SC_RENDER_NOISE_PLANET_TO_TEXTURE__

class RenderNoisePlanetToTexture : public RenderInStepsToTexture
{
public:
    RenderNoisePlanetToTexture(const char* vertexShaderFile, const char* fragmentShaderFile, DRVector2 textureSize);
    ~RenderNoisePlanetToTexture();
    
    void update(float theta, float h) {mTheta = theta; mH = h;}
    DRReturn init(float stepSize, float theta, float h, const DRMatrix& rotation, GLuint &texture);
    virtual DRReturn renderStuff();
    
    __inline__ const DRMatrix& getRotationsMatrix() {return mRotation;}
    __inline float getTheta() {return mTheta;}
    
protected:
private:
    ShaderProgram mShader;
    InfiniteSphere* mRenderSphere;
    float         mTheta;
    float         mH;
    DRMatrix      mRotation;
};

#endif //__SC_RENDER_NOISE_PLANET_TO_TEXTURE__