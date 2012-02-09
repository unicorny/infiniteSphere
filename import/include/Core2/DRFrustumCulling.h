/* 
 * File:   DRFrustumCulling.h
 * Author: Dario
 *
 * Created on 20. Oktober 2011, 21:39
 */

#ifndef __DR_FRUSTUM_CULLING_H
#define	__DR_FRUSTUM_CULLING_H

enum DRFrustumPosition
{
    OUTSIDE = -1,
    INTERSECT = 0,
    INSIDE = 1,
    FR_ERROR = 2,
    BEVORE = 3,
    BEHIND = 4
};

class CORE2_API DRFrustumCulling
{
public:
    // view = Kamera Matrix, projection = DRMatrix::view_frustum
    DRFrustumCulling(DRObjekt* camera, DRReal fovAngle, DRReal aspectRatio, DRReal zNearPlane, DRReal zFarPlane);
    
    // Camera Matrix wird automatisch geupdatet, wenn camera gesetzt wurde, wenn nicht, return DR_ERROR
    DRReturn updateMatritzen(DRReal fovAngle, DRReal aspectRatio, DRReal zNearPlane, DRReal zFarPlane);
    
    DRFrustumPosition isPointInFrustum(const DRVector3& point);
    DRFrustumPosition isSphereInFrustum(const DRVector3& middlePoint, DRReal radius);
    DRFrustumPosition isBoxInFrustum(const DRVector3& boxMin, const DRVector3& boxMax, const DRMatrix& transform = DRMatrix::identity());
private:
    
    DRFrustumPosition classifyBox(const DRVector3& boxMin, const DRVector3& boxMax, const DRMatrix& invTransform, int planeIndex);
    
    DRPlane   mViewPlanes[6];
    DRObjekt* mCamera;    
    DRReal    mNearPlane;
    DRReal    mFarPlane;
    DRReal    mAspectRatio;
    DRReal    mFovAngle;
    double    mFovAngleTangent;
    double    mSphereFactorX;
    
};

#endif	/* __DR_FRUSTUM_CULLING_H */

