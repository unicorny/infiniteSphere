#include "main.h"

GenerateTexture::GenerateTexture(RenderToTexture* frameBuffer)
: mStepSize(0), mIndexStepMode(0), mFrameBuffer(frameBuffer)
{
    
}

GenerateTexture::~GenerateTexture()
{
    
}

DRReturn GenerateTexture::init(float stepSize, DRVector2 textureSize, float clippingBorder[4])
{
    mStepSize = stepSize;
    mTextureSize = textureSize;
    mCursorIndex = mTextureSize/2.0f - stepSize/2.0f;
    mIndexStepMode = 0;
    mCursorMaxCount = 1;
    mCursorCurrentCount = 0;
    memcpy(mClippingBorder, clippingBorder, sizeof(float)*4);
    
    //mFrameBuffer->bindToRender();
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //mFrameBuffer->unbind();    
    
    return DR_OK;
}

DRReturn GenerateTexture::step()
{
     if(mCursorIndex > mTextureSize || mCursorIndex < DRVector2(0.0f))
        return DR_NOT_ERROR;
    
    mFrameBuffer->bindToRender();
    
    //render stuff
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // glClearColor(0.0, 1.0, 1.0, 0);
    
    glColor3f(1.0f, 0.0f, 0.0f);    
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
           
    //mCursorIndex / mTextureSize
    DRVector2 clippingSize = DRVector2(mClippingBorder[1]-mClippingBorder[0],
                                       mClippingBorder[3]-mClippingBorder[2]);
    DRVector2 cursor = mCursorIndex / mTextureSize * clippingSize + DRVector2(mClippingBorder[0], mClippingBorder[2]);
    DRVector2 halfStepSize = (DRVector2(mStepSize) / mTextureSize * clippingSize) * 0.5f;
    
    //Reseten der Matrixen
    glViewport(mCursorIndex.x-mStepSize/2.0f, mCursorIndex.y-mStepSize/2.0f, mStepSize, mStepSize);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
       
    // left, right, bottom, top, near, far    
    glOrtho(cursor.x-halfStepSize.x, cursor.x+halfStepSize.x, cursor.y-halfStepSize.y, cursor.y+halfStepSize.y, -1.0, 1.0);
  //  printf("left: %f, right: %f, bottom: %f, top: %f, pixel cursor: %f, %f, stepSize: %f\n", 
    //        cursor.x-halfStepSize.x, cursor.x+halfStepSize.x, cursor.y-halfStepSize.y, cursor.y+halfStepSize.y, mCursorIndex.x, mCursorIndex.y, mStepSize);
    //glOrtho(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix
    glLoadIdentity();                    // Reset (init) the modelview matrix
    
    DRReturn ret = renderStuff();
    
    //clean up
    mFrameBuffer->unbind();    
    if(ret) LOG_ERROR("Fehler bei GenerateTexture::renderStuff call", DR_ERROR);
    DRGrafikError("Fehler bei render to texture");
    
    switch(mIndexStepMode)
    {
    case 0: mCursorIndex.x += mStepSize; break;//rechts
    case 1: mCursorIndex.y += mStepSize; break;//unten
    case 2: mCursorIndex.x -= mStepSize; break;//links
    case 3: mCursorIndex.y -= mStepSize; break;//oben
    }
    mCursorCurrentCount++;
    if(mCursorCurrentCount >= mCursorMaxCount)
    {
        mIndexStepMode++;
        if(mIndexStepMode >= 4) mIndexStepMode = 0;
        mCursorCurrentCount = 0;
        if(2 == mIndexStepMode || 0 == mIndexStepMode)
            mCursorMaxCount++;
    }
    
    return DR_OK;
}