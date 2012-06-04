/*
 * File:   MipMapCubeTexture.h
 * Author: Dario
 *
 * Created on 29. Mai 2012, 17:53
 */

#ifndef SPACE_CRAFT_MIP_MAP_CUBE_TEXTURE_H
#define	SPACE_CRAFT_MIP_MAP_CUBE_TEXTURE_H

class Camera;

class MipMapCubeTexture
{
public:
    MipMapCubeTexture();
    ~MipMapCubeTexture();

    DRTexturePtr updateTexture(DRVector3 cameraPosition, Camera* camera, DRMatrix modelview, float theta);


private:
	void unproject(DRVector2 screenCoords, DRMatrix inverseMatrix, DRVector3& nearPos, DRVector3& farPos);
	DRReturn setupFrameBuffer(DRTexturePtr texture);


    std::vector<MipMapTexture*> mCubeSides;
	GLuint			mFrameBufferID;
	ShaderProgram*  mShader;
	DRTexturePtr	mSumTexture;

};

#endif // SPACE_CRAFT_MIP_MAP_CUBE_TEXTURE_H
