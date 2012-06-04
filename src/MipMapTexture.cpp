#include "main.h"

MipMapTexture::MipMapTexture(DRVector3 center, const char* texture)
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
	mTexture->setWrappingMode(GL_CLAMP_TO_BORDER);
	mMipMapTextue = DRTextureManager::Instance().getTexture(DRVector2i(512), 4);

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
	glEnable(GL_TEXTURE_2D);
	if(mTexture.getResourcePtrHolder())
		mTexture->bind();
	mShader->bind();
	mShader->setUniform3fv("edge1", mVisibleRect[0]);
	mShader->setUniform3fv("edge2", mVisibleRect[1]); 
	mShader->setUniform3fv("edge3", mVisibleRect[2]); 
	mShader->setUniform3fv("edge4", mVisibleRect[3]);
	mShader->setUniformMatrix("projection", DRMatrix::identity());
	mShader->setUniformMatrix("texturRotation", DRMatrix::identity());
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

void MipMapTexture::renderToTexture()
{
	DRMatrix projectionMatrix = DRMatrix::ortho_projection(0.0f, 1.0f, 0.0f, 1.0f, -1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);
	if(mTexture.getResourcePtrHolder())
		mTexture->bind();
	glClearColor(0.0, 0.0, 0.0, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	mShader->bind();
	mShader->setUniform3fv("edge1", mVisibleRect[0]);
	mShader->setUniform3fv("edge2", mVisibleRect[1]); 
	mShader->setUniform3fv("edge3", mVisibleRect[2]); 
	mShader->setUniform3fv("edge4", mVisibleRect[3]);
	mShader->setUniformMatrix("projection", projectionMatrix);
	mShader->setUniformMatrix("texturRotation", mTextureRotation.invert());
	DRGeometrieManager::Instance().getGrid(10, GEO_FULL, GEO_VERTEX_QUADS)->render();
	mShader->unbind();
}


void MipMapTexture::calculateVisibleRect(DRVector3 edgePoints[4], DRVector3 ray, DRMatrix modelview)
{
	if(!edgePoints) LOG_ERROR_VOID("Zero-Pointer");
	glColor3f(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	//glMultMatrixf(mRotation);
	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i < 4; i++)
	{
		edgePoints[i] = edgePoints[i].transformCoords(modelview);//.transformCoords(mRotation);
		
		DRVector3 n = DRVector3(mEbene.v);
		float deltaT = -(edgePoints[i].dot(n)+mEbene.d) / (ray.dot(n));
		mVisibleRect[i] = DRVector3(edgePoints[i]+ray*deltaT);

		DRVector3 point = mVisibleRect[i].transformCoords(modelview.invert());
		if(point.x < -1.0f) point.x = -1.0f;
		else if(point.x > 1.0f) point.x = 1.0f;
		if(point.y < -1.0) point.y = -1.0f;
		else if(point.y > 1.0f ) point.y = 1.0f;
		//glVertex3fv(mVisibleRect[i].transformCoords(modelview.invert()));
	//	glVertex3fv(point);
		
	}
	glEnd();
	glPopMatrix();

	DRVector3 rotateAxis = DRVector3(mVisibleRect[1]-mVisibleRect[0]).normalize(); 
	DRVector3 defaultAxis = DRVector3(2.0f, 0.0f, 0.0f).normalize();

	DRVector3 rotationAxis = defaultAxis.cross(rotateAxis).normalize();//startAxis.cross(centerPosition).normalize();
	float rotationAngle = defaultAxis.dot(rotateAxis);//startAxis.dot(centerPosition);

	Eigen::Affine3f affine;
	affine = Eigen::AngleAxisf(acosf(rotationAngle), Eigen::Vector3f(rotationAxis));
	mTextureRotation = DRMatrix(affine.data()).invert();
	for(int i = 0; i < 4; i++)
		mVisibleRect[i] = mVisibleRect[i].transformCoords(mTextureRotation);
	
	//mVisibleRect
}