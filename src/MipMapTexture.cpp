#include "main.h"

MipMapTexture::MipMapTexture(DRVector3 edges[4], DRVector3 center, const char* texture)
: mEbene(center, -1.0f)
{
	memset(mVisibleRect, 0, sizeof(DRVector3)*4);
    //mEbene = DRPlane::fromPoints(edges[0], edges[1], edges[2]);
 /*   printf("[MipMapTexture::MipMapTexture]:\n%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n",
           planeEdges[0].x, planeEdges[0].y, planeEdges[0].z,
           planeEdges[1].x, planeEdges[1].y, planeEdges[1].z,
           planeEdges[2].x, planeEdges[2].y, planeEdges[2].z,
           planeEdges[3].x, planeEdges[3].y, planeEdges[3].z);
		   */
	mTexture = DRTextureManager::Instance().getTexture(texture);
	mTexture->setWrappingMode(TEXTURE_WRAPPING_CLAMP_TO_EDGE);

	DRVector3 centerPosition = center; 
    DRVector3 startAxis(0.00001f, 0.00001f, 1.0f);

    DRVector3 rotationAxis = startAxis.cross(centerPosition).normalize();//startAxis.cross(centerPosition).normalize();
    float rotationAngle = startAxis.dot(centerPosition);//startAxis.dot(centerPosition);

    Eigen::Affine3f affine;
    affine = Eigen::AngleAxisf(acosf(rotationAngle), Eigen::Vector3f(rotationAxis));
    mRotation = DRMatrix(affine.data());

	mShader = ShaderManager::Instance().getShader("data/shader/MipMap.vert", "data/shader/texture.frag");
}

void MipMapTexture::debugRender()
{
	DRVector2 textureCoords[] = {DRVector2( 1.0f,  1.0f),
								 DRVector2(-1.0f,  1.0f),
								 DRVector2( 1.0f, -1.0f),
								 DRVector2(-1.0f, -1.0f)};
	glEnable(GL_TEXTURE_2D);
	if(mTexture.getResourcePtrHolder())
		mTexture->bind();
	mShader->bind();
	mShader->setUniform3fv("edge1", mVisibleRect[0]);
	mShader->setUniform3fv("edge2", mVisibleRect[1]); 
	mShader->setUniform3fv("edge3", mVisibleRect[2]); 
	mShader->setUniform3fv("edge4", mVisibleRect[3]);
    /*glBegin(GL_TRIANGLE_STRIP);	
    for(int i = 0; i < 4; i++)
	{
		glTexCoord2fv(textureCoords[i]);
        glVertex3fv(mEdges[i]);
	}
    glEnd();*/
	glPushMatrix();
	glMultMatrixf(mRotation);
	DRGeometrieManager::Instance().getGrid(10, GEO_FULL, GEO_VERTEX_QUADS)->render();
	glPopMatrix();
	mShader->unbind();
}

void MipMapTexture::calculateVisibleRect(DRVector3 frustum[8])
{
	if(!frustum) LOG_ERROR_VOID("Zero-Pointer");
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glMultMatrixf(mRotation);
	//glBegin(GL_QUADS);
	for(int i = 0; i < 4; i++)
	{
		DRVector3 ray = frustum[i*2+1];//-frustum[i*2];
		DRVector3 n = DRVector3(mEbene.v);
		float deltaT = -(frustum[i*2].dot(n)+mEbene.d) / (ray.dot(n));
		mVisibleRect[i] = DRVector3(frustum[i*2]+ray*deltaT).transformCoords(mRotation.invert());
		
		//glVertex3fv(mVisibleRect[i]);
	}
	//glEnd();
	glPopMatrix();

	DRVector3 rotateAxis = DRVector3(mVisibleRect[1]-mVisibleRect[0]).normalize(); 
	DRVector3 defaultAxis = DRVector3(2.0f, 0.0f, 0.0f).normalize();

	DRVector3 rotationAxis = defaultAxis.cross(rotateAxis).normalize();//startAxis.cross(centerPosition).normalize();
	float rotationAngle = defaultAxis.dot(rotateAxis);//startAxis.dot(centerPosition);

	Eigen::Affine3f affine;
	affine = Eigen::AngleAxisf(acosf(rotationAngle), Eigen::Vector3f(rotationAxis));
	DRMatrix rotation = DRMatrix(affine.data()).invert();
	for(int i = 0; i < 4; i++)
		mVisibleRect[i] = mVisibleRect[i].transformCoords(rotation);
	
	//mVisibleRect
}