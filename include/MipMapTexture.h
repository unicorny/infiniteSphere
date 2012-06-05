/*
 * File:   MipMapTexture.h
 * Author: Dario
 *
 * Created on 29. Mai 2012, 17:53
 */

#ifndef SPACE_CRAFT_MIP_MAP_TEXTURE_H
#define	SPACE_CRAFT_MIP_MAP_TEXTURE_H

class Camera;

class MipMapTexture
{
public:
    MipMapTexture(DRVector3 center, const char* texture);
    __inline__ void calculateDistanceToCamera(DRVector3 cameraPosition)
		{mDistanceToCamera = (mEbene.pointPlaneDistance(cameraPosition));}
    __inline__ float getDistanceToCamera() const {return mDistanceToCamera;}

	void calculateVisibleRect(DRVector3 edgePoints[4], DRVector3 ray, DRMatrix modelview);

    void debugRender();

	DRTexturePtr getTexture() {return mTexture;}
	DRTexturePtr getMipMapTexture() {return mMipMapTextue;}
	void renderToTexture();

private:
    DRPlane     mEbene;
	DRVector3	mVisibleRect[4];
	DRVector3	mPlaneEdges[4];
    float       mDistanceToCamera;
	ShaderProgram* mShader;
	DRMatrix   mRotation;
	DRMatrix   mTextureRotation;

	DRTexturePtr mTexture;
	DRTexturePtr mMipMapTextue;

};

#endif // SPACE_CRAFT_MIP_MAP_TEXTURE_H
	