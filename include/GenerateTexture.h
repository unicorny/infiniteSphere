#ifndef __SC_GENERATE_TEXTURE__
#define __SC_GENERATE_TEXTURE__

class GenerateTexture
{
public:
    // frame buffer, will be called in init and in step
    GenerateTexture(RenderToTexture* frameBuffer);
    ~GenerateTexture();
    
    // reset values, set step size
    DRReturn init(float stepSize, DRVector2 textureSize, float clippingBorder[4]);
    // render with current step, add step
    DRReturn step();
    
    // called every step
    virtual DRReturn renderStuff() = 0;
    
    void setFrameBuffer(RenderToTexture* newFrameBuffer) {mFrameBuffer = mFrameBuffer;}
    RenderToTexture* getFrameBuffer() {return mFrameBuffer;}
    
    void bindTexture() {if(mFrameBuffer) mFrameBuffer->bindTexture();}
    
protected:
    float       mClippingBorder[4]; //left, rigt, bottom, top
    DRVector2   mTextureSize;
    RenderToTexture* mFrameBuffer;
    
private:
    DRVector2   mCursorIndex;
    float       mStepSize;
    u8          mIndexStepMode; 
    short       mCursorMaxCount;
    short       mCursorCurrentCount;
};

#endif //__SC_GENERATE_TEXTURE__