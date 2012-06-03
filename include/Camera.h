/*
 * File:   Camera.h
 * Author: dario
 *
 * Created on 20. August 2011, 21:17
 */

#ifndef CAMERA_H
#define	CAMERA_H


class Camera : public DRObjekt
{
public:
    Camera();
    Camera(const DRVector3& position);
    virtual ~Camera();

    //void rotateAbs(const DRVector3& rotation);
    //void rotateRel(const DRVector3& rotation);

     //! diese Funktion sollte vorm rendern aller anderen Objekte aufgerufen werden
    //! \brief setzt die Objekt Matrix als Kameramatrix
    void setKameraMatrix();

    //! diese Funktion sollte vorm rendern aller anderen Objekte aufgerufen werden
    //! \brief setzt die Objekt-Rotation als Kameramatrix
    void setKameraMatrixRotation();

    __inline__ Vector3Unit getSektorPosition() const {return mSektorPosition;}
    //! \brief calculate camera sektor position relative to targetSektor

    __inline__ void setSektorPosition(Vector3Unit absPosition) {mSektorPosition = absPosition;}

    //! in Verh&auml;ltniss zum Objekteigenem Koordinatensystem
    //! \brief bewegt die Kamera relativ
    //! \param translate die Bewegung des Objektes auf allen drei Achsen
    void translateRel_SektorPosition(const DRVector3& translate, const UnitTypes& type);

    void lookAt_SektorPosition(Vector3Unit targetPosition, Vector3Unit upVector = Vector3Unit(0.0f, 1.0f, 0.0f));

    void translateRel(const DRVector3& translate);

    //! Projection Matrix
    const DRMatrix& getProjectionMatrix() {return mPerspectiveProjectionMatrix;}
    void setProjectionMatrix(const float angle_of_view, const float aspect_ratio,
                             const float z_near, const float z_far);

    __inline__ float getFOV() const {return mFOV;}
    __inline__ float getAspectRatio() const {return mAspectRatio;}

protected:
    virtual void update();

private:
    //! absolute position of camera in the current sector in sector coordinates (example: AE)
    //! the position of camera is the distance from the abs position, abs position is local zero-point
    Vector3Unit mSektorPosition;

    float       mFOV;
    float       mAspectRatio;
    DRMatrix    mPerspectiveProjectionMatrix;
};

#endif	/* CAMERA_H */

