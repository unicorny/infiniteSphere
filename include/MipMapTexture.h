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
    MipMapTexture(DRVector3 edges[4], DRVector3 center, const char* texture);
    __inline__ void calculateDistanceToCamera(DRVector3 cameraPosition)
		{mDistanceToCamera = (mEbene.pointPlaneDistance(cameraPosition));}
    __inline__ float getDistanceToCamera() const {return mDistanceToCamera;}

	void calculateVisibleRect(DRVector3 frustum[8]);

    void debugRender();

private:
    DRPlane     mEbene;
    DRVector3   mEdges[4];
	DRVector3	mVisibleRect[4];
    float       mDistanceToCamera;
	ShaderProgram* mShader;
	DRMatrix   mRotation;

	DRTexturePtr mTexture;

};

#endif // SPACE_CRAFT_MIP_MAP_TEXTURE_H
