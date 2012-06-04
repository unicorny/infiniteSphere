/*
 * File:   Camera.cpp
 * Author: dario
 *
 * Created on 20. August 2011, 21:17
 */

#include "main.h"

Camera::Camera()
: mSektorPosition(Unit(0, AE))
{
}

Camera::Camera(const DRVector3& position)
: DRObjekt(position), mSektorPosition(Unit(0, NONE))
{
}

Camera::~Camera()
{

}

void Camera::setCameraMatrix()
{
    glMultMatrixf(mMatrix);
}

void Camera::setCameraMatrixRotation()
{
    glMultMatrixf(DRMatrix::axis(mXAxis, mYAxis, mZAxis));
}

DRMatrix Camera::getCameraMatrixRotation() const
{
	return DRMatrix::axis(mXAxis, mYAxis, mZAxis);
}

void Camera::translateRel_SektorPosition(const DRVector3& translate, const UnitTypes& type)
{
    DRMatrix m1 = DRMatrix::axis(mXAxis, mYAxis, mZAxis);
    DRMatrix m2 = DRMatrix::translation(mSektorPosition.getVector3());
    DRMatrix m = DRMatrix(m1) * DRMatrix(m2);
    mSektorPosition += Vector3Unit(translate.transformNormal(m), type);
    //mSektorPosition = mSektorPosition.normalize()*Unit(6478, KM);
    //mAbsPosition.print();
  /*  printf("\r x-axis: %f %f %f, y-axis: %f %f %f, z-axis: %f %f %f, abs: %s %s %s", mXAxis.x, mXAxis.y, mXAxis.z,
                                                    mYAxis.x, mYAxis.y, mYAxis.z,
                                                    mZAxis.x, mZAxis.y, mZAxis.z,
                                                    mAbsPosition.x.print().data(), mAbsPosition.y.print().data(),
                                                    mAbsPosition.z.print().data());
 // * */
}
void Camera::translateRel(const DRVector3& translate)
{
    DRObjekt::translateRel(translate);
    //printf("\r camera: %f %f %f l:%f", mPosition.x, mPosition.y, mPosition.z, mPosition.length());

    if(mPosition.lengthSq() > 1000.0f*1000.0f)
    {
        mSektorPosition += Vector3Unit(mPosition/1000.0f, KM);
        mPosition = 0.0f;
        update();
        printf("update sektor\n");
    }

}

void Camera::update()
{
	Vector3Unit pos = mSektorPosition.convertTo(KM);
    //printf("\r camera: %s %s %s: %s", pos.x.print().data(), pos.y.print().data(),
      //                                              pos.z.print().data(), pos.length().print().data());
    DRMatrix m1 = DRMatrix::axis(mXAxis, mYAxis, mZAxis);
    DRMatrix m2 = DRMatrix::translation(-mPosition);
    //mMatrix = DRMatrix(m1) * DRMatrix(m2);
    mMatrix = DRMatrix(m2) * DRMatrix(m1);
   // mMatrix.print();
}

void Camera::lookAt_SektorPosition(Vector3Unit targetPosition, Vector3Unit upVector/* = Vector3Unit(0.0f, 1.0f, 0.0f)*/)
{
    mZAxis = Vector3Unit(mSektorPosition - targetPosition).getVector3().normalizeEx();
    mXAxis = upVector.cross(mZAxis).getVector3().normalizeEx();
    mYAxis = mZAxis.cross(mXAxis).normalizeEx();
    update();
}

void Camera::setProjectionMatrix(const float angle_of_view, const float aspect_ratio,
                                 const float z_near, const float z_far)
{
    mPerspectiveProjectionMatrix = DRMatrix::perspective_projection(angle_of_view, aspect_ratio, z_near, z_far);
    mAspectRatio = aspect_ratio;
    mFOV = angle_of_view;
}
