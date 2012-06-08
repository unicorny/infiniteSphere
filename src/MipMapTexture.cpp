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
	mPlaneEdges[0] = DRVector3(1.0f, 1.0f, 1.0f);
	mPlaneEdges[1] = DRVector3(1.0f, -1.0f, 1.0f);
	mPlaneEdges[2] = DRVector3(-1.0f, -1.0f, 1.0f);
	mPlaneEdges[3] = DRVector3(-1.0f, 1.0f, 1.0f);

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
	for(int i = 0; i < 4; i++)
		mPlaneEdges[i] = mPlaneEdges[i].transformCoords(mRotation);

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

	// look at
	DRVector3 zAxis = ray;
	DRVector3 xAxis = (-DRVector3(mRotation.m[1])).cross(zAxis).normalize();
	DRVector3 yAxis = zAxis.cross(xAxis).normalize();
	DRMatrix rotation = DRMatrix::axis(xAxis, yAxis, zAxis);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; i++)
	{
		edgePoints[i] = edgePoints[i].transformCoords(rotation.invert());//.transformCoords(mRotation);
		glVertex3fv(DRVector3(0.0f));
		glVertex3fv(edgePoints[i]*2.0f);
	}
	glEnd();
	
	for(int i = 0; i < 4; i++)
	{
		
		mVisibleRect[i] = mPlaneEdges[i];
		DRVector3 n = DRVector3(mEbene.v);
		// deltaT is distance from spherical surface
		float deltaT = -(mEbene.d) / (edgePoints[i].dot(n));
		if(deltaT > 0.0f)
			mVisibleRect[i] = DRVector3(edgePoints[i]*deltaT);
	
		//clamp 
		for(int j = 0; j < 3; j++)
		{
			if(mVisibleRect[i].c[j] < -1.0f) mVisibleRect[i].c[j] = -1.0f;
			else if(mVisibleRect[i].c[j] > 1.0f) mVisibleRect[i].c[j] = 1.0f;
		}
	}
	//debug render
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);	
	glPushMatrix();
	//glMultMatrixf(mRotation);
	DRVector3 transpose(mEbene.v);
	transpose *= 0.01f;
	glTranslatef(transpose.x, transpose.y, transpose.z);
	glBegin(GL_LINES);
	for(int i = 1; i < 4; i++)
	{
		glVertex3fv(mVisibleRect[i-1]);
		glVertex3fv(mVisibleRect[i]);
	}
	glVertex3fv(mVisibleRect[0]);
	glVertex3fv(mVisibleRect[3]);
	glEnd();
	glPopMatrix();
	//*/

	DRVector3 rotateAxis = DRVector3(mVisibleRect[1]-mVisibleRect[0]).normalize(); 
	DRVector3 defaultAxis = DRVector3(2.0f, 0.0f, 0.0f).normalize();

	DRVector3 rotationAxis = defaultAxis.cross(rotateAxis).normalize();//startAxis.cross(centerPosition).normalize();
	float rotationAngle = defaultAxis.dot(rotateAxis);//startAxis.dot(centerPosition);

	Eigen::Affine3f affine;
	affine = Eigen::AngleAxisf(acosf(rotationAngle), Eigen::Vector3f(rotationAxis));
	mTextureRotation = DRMatrix(affine.data()).invert();
	//for(int i = 0; i < 4; i++)
	//	mVisibleRect[i] = mVisibleRect[i].transformCoords(mTextureRotation);
	
	//mVisibleRect
}