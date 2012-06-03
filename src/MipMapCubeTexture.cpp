#include "main.h"

int MipMapCubeTexture::mCubeEdgeIndices[] = {
    0, 1, 2, 3, // front
    1, 4, 3, 6, // right
    4, 5, 6, 7, // back
    5, 0, 7, 2, // left
    5, 4, 0, 1, // top
    2, 3, 7, 6}; // bottom

MipMapCubeTexture::MipMapCubeTexture()
{
    DRVector3 edges[4];
	DRVector3 mCubeEdges[8];
	mCubeEdges[0] = DRVector3(-1.0f, 1.0f, -1.0f);
	mCubeEdges[1] = DRVector3(1.0f, 1.0f, -1.0f);
	mCubeEdges[2] = DRVector3(-1.0f, -1.0f, -1.0f);
	mCubeEdges[3] = DRVector3(1.0f, -1.0f, -1.0f); // front
	mCubeEdges[4] = DRVector3(1.0f, 1.0f, 1.0f); 
	mCubeEdges[5] = DRVector3(-1.0f, 1.0f, 1.0f);
	mCubeEdges[6] = DRVector3(1.0f, -1.0f, 1.0f);
	mCubeEdges[7] = DRVector3(-1.0f, -1.0f, 1.0f); // back

	DRVector3 center[] = {DRVector3(0.0f,0.0f,-1.0f),DRVector3(1.0f,0.0f,0.0f),DRVector3(0.0f,0.0f, 1.0f),// front, right, back
						  DRVector3(-1.0f,0.0f,0.0f),DRVector3(0.0f,1.0f,0.0f),DRVector3(0.0f,-1.0f,0.0f)};// left, top, bottom

	const char* names[] = {"data/planet/front.png",
						   "data/planet/back.png",
						   "data/planet/left.png",
						   "data/planet/right.png",
						   "data/planet/top.png",
						   "data/planet/bottom.png"};

    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 4; j++)
            edges[j] = mCubeEdges[mCubeEdgeIndices[i*4+j]];
        mCubeSides.push_back(new MipMapTexture(edges, center[i], names[i]));
    }
}

MipMapCubeTexture::~MipMapCubeTexture()
{
    for(uint i = 0; i < mCubeSides.size(); i++)
    {
       DR_SAVE_DELETE(mCubeSides[i]);
    }
    mCubeSides.clear();
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


DRTexturePtr MipMapCubeTexture::updateTexture(DRVector3 cameraPosition, Camera* camera, DRMatrix modelview)
{
    if(!camera) LOG_ERROR("Zero Pointer", DRTexturePtr());

    // calculate field of view, horizontal and vertical
    DRVector2 fov(camera->getFOV()*GRADTORAD, 0.0f);
    fov.y = atanf(tanf(fov.x)/camera->getAspectRatio());

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
	for(uint i = 0; i < 3; i++)
		mCubeSides[i]->debugRender();

	DRVector3 viewFrustum[8];
	float epsilon = 0.1f;

	DRVector2 screenCoords[] = {DRVector2(epsilon, 1.0f-epsilon),
								DRVector2(epsilon, epsilon),
								DRVector2(1.0f-epsilon, epsilon),
								DRVector2(1.0f-epsilon, 1.0f-epsilon)};
	//Transformation matrices
	//Calculation for inverting a matrix, compute projection x modelview
	//and store in A[16]
	Eigen::Matrix4f calculate = Eigen::Matrix4f(DRMatrix(modelview * camera->getProjectionMatrix()));
	DRMatrix inverseMatrix = calculate.inverse().eval().data();
	for(int i = 0; i < 4; i++)
	{
		unproject(screenCoords[i], inverseMatrix, viewFrustum[i*2], viewFrustum[i*2+1]);
	}

	for(uint i = 0; i < 3; i++)
		mCubeSides[i]->calculateVisibleRect(viewFrustum);

    return DRTexturePtr();
}
