#include "main.h"

MipMapCubeTexture::MipMapCubeTexture()
{
	DRVector3 center[] = {DRVector3(0.0f,0.0f,-1.0f),DRVector3(1.0f,0.0f,0.0f),DRVector3(0.0f,0.0f, 1.0f),// front, right, back
						  DRVector3(-1.0f,0.0f,0.0f),DRVector3(0.0f,1.0f,0.0f),DRVector3(0.0f,-1.0f,0.0f)};// left, top, bottom

	const char* names[] = {"data/space/front.jpg",
						   "data/space/back.jpg",
						   "data/space/left.jpg",
						   "data/space/right.jpg",
						   "data/space/top.jpg",
						   "data/space/bottom.jpg"};

    for(int i = 0; i < 6; i++)
    {
        mCubeSides.push_back(new MipMapTexture(center[i], names[i]));
    }

	//render to texture
	glGenFramebuffersEXT(1, &mFrameBufferID);

	mShader = ShaderManager::Instance().getShader("data/shader/simple.vert", "data/shader/addTextures.frag");
	mSumTexture = DRTextureManager::Instance().getTexture(DRVector2i(512), 4);
}

MipMapCubeTexture::~MipMapCubeTexture()
{
    for(uint i = 0; i < mCubeSides.size(); i++)
    {
       DR_SAVE_DELETE(mCubeSides[i]);
    }
    mCubeSides.clear();
	if(mFrameBufferID)
		glDeleteFramebuffersEXT(1, &mFrameBufferID);
	mFrameBufferID = 0;
}

void MipMapCubeTexture::unproject(DRVector2 screenCoords, DRMatrix inverseMatrix, DRVector3& nearPos, DRVector3& farPos)
{
	//Transformation of normalized coordinates between -1 and 1
	DRVector3 normalizedCoord = DRVector3(screenCoords.x*2.0f-1.0f,
										  screenCoords.y*2.0f-1.0f,	
										  -1.0f);
	//Objects coordinates
	nearPos = normalizedCoord.transformCoords(inverseMatrix);
	normalizedCoord.z = 1.0f;
	farPos = normalizedCoord.transformCoords(inverseMatrix);
}

bool CubeSideSort(const MipMapTexture* d1, const MipMapTexture* d2)
{
  return d1->getDistanceToCamera() > d2->getDistanceToCamera();
}


DRTexturePtr MipMapCubeTexture::updateTexture(DRVector3 cameraPosition, Camera* camera, DRMatrix modelview, float theta)
{
    if(!camera) LOG_ERROR("Zero Pointer", DRTexturePtr());

    // calculate field of view, horizontal and vertical
    DRVector2 fov(camera->getFOV()*GRADTORAD, 0.0f);
    fov.y = atanf(tanf(fov.x)/camera->getAspectRatio());

	

	DRVector3 sideRays[4];
	DRVector3 edgeRays[4];
	/*DRVector3 rotateAxis[] = {DRVector3(0.5f, 0.5f, 0.0f), 
							  DRVector3(-0.5f, 0.5f, 0.0f),
							  DRVector3(-0.5f, -0.5f, 0.0f),
							  DRVector3(0.5f, -0.5f, 0.0f)};
							  //*/
	DRVector3 rotateAxis[] = {DRVector3(1.0f, 0.0f, 0.0f), 
							  DRVector3(0.0f, 1.0f, 0.0f),
							  DRVector3(-1.0f, 0.0f, 0.0f),
							  DRVector3(0.0f, -1.0f, 0.0f)};
	Eigen::Matrix4f mat = Eigen::Matrix4f(modelview);
	DRMatrix modelViewInvert = DRMatrix(mat.inverse().eval().data());
	modelViewInvert = camera->getCameraMatrixRotation();

	for(int i = 0; i < 4; i++)
	{
		rotateAxis[i] = rotateAxis[i].normalize();
		sideRays[i] = DRVector3(0.0f, 0.0, 1.0f).transformNormal(DRMatrix::rotationAxis(rotateAxis[i], theta));
		//sideRays[i] = sideRays[i].transformNormal(modelViewInvert);
	}
	DRVector3 center = (sideRays[0]-sideRays[2])/2.0f + sideRays[2];
	for(int i = 0; i < 4; i++)
	{
		int iMinusOne = i-1;
		if(iMinusOne < 0) iMinusOne = 3;
		edgeRays[i] = center + sideRays[iMinusOne] - sideRays[i];
	}
	
    //printf("\r fov: %f %f", fov.x*RADTOGRAD, fov.y*RADTOGRAD);

    // calculate distances to camera
    for(uint i = 0; i < mCubeSides.size(); i++)
        mCubeSides[i]->calculateDistanceToCamera(cameraPosition);

    // sort
    std::sort(mCubeSides.begin(), mCubeSides.end(), CubeSideSort);
    std::reverse(mCubeSides.begin(), mCubeSides.end());

     // Dump the vector to check the result
  /*  printf("sorted: \n");
    for(int i = 0; i < mCubeSides.size(); i++)
        printf("l: %f, ", mCubeSides[i]->getDistanceToCamera());
    printf("\n");
    //*/
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	//glMultMatrixf(modelview.invert());
	for(int i = 3; i >= 0; i--)
		mCubeSides[i]->debugRender();
	glPopMatrix();

	float epsilon = 0.1f;

	DRVector3 edgePoints[] = {DRVector3(1.0f, 1.0f, 1.0f),
						 	  DRVector3(-1.0f, 1.0f, 1.0f),
							  DRVector3(1.0f, -1.0f, 1.0f),
							  DRVector3(-1.0f, -1.0f, 1.0f)};
	GLfloat faModelview[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, faModelview );

	for(int i = 0; i < 4; i++)
	{
		//edgePoints[i] = edgePoints[i].transformCoords(modelview);
		edgePoints[i].x *= sqrtf(1.0f-cosf(theta))*1.25f;
		edgePoints[i].y *= sqrtf(1.0f-cosf(theta))*1.25f;
		edgePoints[i].z = 0.0f;
	}

	for(uint i = 0; i < 1; i++)
		mCubeSides[i]->calculateVisibleRect(edgeRays, cameraPosition.normalize(), modelview);

	//render to texture
	
	glViewport(0, 0, 512, 512);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); 
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	for(uint i = 0; i < 3; i++)
	{
		setupFrameBuffer(mCubeSides[i]->getMipMapTexture());
		mCubeSides[i]->renderToTexture();
	}
	
	setupFrameBuffer(mSumTexture);
	mShader->bind();
	glActiveTexture(GL_TEXTURE0);
	mCubeSides[0]->getMipMapTexture()->bind();
	mShader->setUniform1i("tex1", 0);

	glActiveTexture(GL_TEXTURE1);
	mCubeSides[1]->getMipMapTexture()->bind();
	mShader->setUniform1i("tex2", 1);

	glActiveTexture(GL_TEXTURE2);
	mCubeSides[2]->getMipMapTexture()->bind();
	mShader->setUniform1i("tex3", 2);

	DRMatrix projectionMatrix = DRMatrix::ortho_projection(0.0f, 1.0f, 0.0f, 1.0f, -1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);
	
	glClearColor(0.0, 0.0, 1.0, 0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	mShader->setUniformMatrix("projection", projectionMatrix);
	DRGeometrieManager::Instance().getGrid(10, GEO_FULL, GEO_VERTEX_QUADS)->render();
	mShader->unbind();

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_DEPTH_TEST);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, g_pSDLWindow->w, g_pSDLWindow->h);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	mSumTexture->bind();
    return mSumTexture;
}

DRReturn MipMapCubeTexture::setupFrameBuffer(DRTexturePtr texture)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBufferID);
	//create texture
	//bind to the new texture ID
	texture->bind();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, texture->getId(), 0);
	GLenum ret = GL_FRAMEBUFFER_COMPLETE_EXT;
	ret = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(ret != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		DREngineLog.writeToLog("Fehler bei Check Framebuffer Status: %s", getFrameBufferEnumName(ret));
		LOG_ERROR("Fehler bei setupFrameBuffer", DR_ERROR);
	}
	return DR_OK;
}