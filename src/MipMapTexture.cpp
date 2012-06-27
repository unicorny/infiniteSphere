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
	mShader->setUniform3fv("edge", 4, mVisibleRect);
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
	mShader->setUniform3fv("edge", 4, mVisibleRect);
	mShader->setUniformMatrix("projection", projectionMatrix);
	mShader->setUniformMatrix("texturRotation", mTextureRotation.invert());
	DRGeometrieManager::Instance().getGrid(10, GEO_FULL, GEO_VERTEX_QUADS)->render();
	mShader->unbind();
}

DRVector3 convertCartesianToSpherical(DRVector3 cartesian)
{
	DRReal r = sqrtf(cartesian.x* cartesian.x + cartesian.y* cartesian.y+ cartesian.z* cartesian.z); 
	DRReal lat = (asinf(cartesian.z/r))*RADTOGRAD;
	DRReal lon = (atan2f(cartesian.y, cartesian.x))*RADTOGRAD;
	return DRVector3(r, lat,lon);
}

DRVector3 convertSphericalToCartesian(DRVector3 latlong)
{
	DRReal lat = (latlong.y)*GRADTORAD;
	DRReal lon = (latlong.z)*GRADTORAD;
	DRVector3 cartesian;
	cartesian.x = cosf(lat)*cosf(lon);
	cartesian.y = cosf(lat)*sinf(lon);
	cartesian.z = sinf(lat);
	return cartesian;
}

void MipMapTexture::calculateVisibleRect(DRVector3 edgePoints[4], DRVector3 ray, DRMatrix modelview)
{
	if(!edgePoints) LOG_ERROR_VOID("Zero-Pointer");

	// look at
	DRVector3 zAxis = ray;
	DRVector3 xAxis = (-DRVector3(mRotation.m[1])).cross(zAxis).normalize();
	DRVector3 yAxis = zAxis.cross(xAxis).normalize();
	DRMatrix rotation = DRMatrix::axis(xAxis, yAxis, zAxis); 

	DRVector3 localEdgePoints[4];
	memcpy(localEdgePoints, edgePoints, sizeof(DRVector3)*4);
	DRVector3 printEdge = edgePoints[0].transformCoords(rotation.invert()).normalize();
	DRVector3 spherical = convertCartesianToSpherical(printEdge);
	//printf("\rlocalEdge: %f %f %f", spherical.x, spherical.y, spherical.z);

	DRColor colors[] = {DRColor(1.0f, 0.0f, 0.0f),
						DRColor(0.0f, 1.0f, 0.0f),
						DRColor(0.0f, 0.0f, 1.0f),
						DRColor(0.0f, 1.0f, 1.0f)};

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; i++)
	{
		glColor3fv(colors[i]);
		localEdgePoints[i] = edgePoints[i].transformCoords(rotation.invert()).normalize();//.transformCoords(mRotation);
		glVertex3fv(DRVector3(0.0f));
		glVertex3fv(edgePoints[i]*4.0f);
	}
	glEnd();
	
	for(int i = 0; i < 4; i++)
	{
		mVisibleRect[i] = mPlaneEdges[i];
		DRVector3 n = DRVector3(mEbene.v);
		// deltaT is distance from spherical surface
		float deltaT = -((mEbene.d)) / (localEdgePoints[i].dot(n));
		//if(deltaT > 0.0f)
			mVisibleRect[i] = DRVector3(localEdgePoints[i]*deltaT);
	
		//clamp 
		for(int j = 0; j < 3; j++)
		{
			if(fabs(mVisibleRect[i].c[j]) > 1.0f) mVisibleRect[i].c[j] = mPlaneEdges[i].c[j];

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
///*
	glBegin(GL_LINES);
	for(int i = 1; i < 4; i++)
	{
		glColor3fv(colors[i-1]);
		glVertex3fv(mVisibleRect[i-1]);
		glColor3fv(colors[i]);
		glVertex3fv(mVisibleRect[i]);
	}
	glColor3fv(colors[0]);
	glVertex3fv(mVisibleRect[0]);
	glColor3fv(colors[3]);
	glVertex3fv(mVisibleRect[3]);
	glEnd();
	//*/
	glPopMatrix();

	for(int i = 0; i < 4; i++)
		mVisibleRect[i] = mVisibleRect[i].transformCoords(mRotation.invert());
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