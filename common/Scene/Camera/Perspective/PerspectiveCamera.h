#pragma once

#include "common/Scene/Camera/Camera.h"

class PerspectiveCamera : public Camera
{
public:
    // inputFov is in degrees. 
    PerspectiveCamera(float aspectRatio, float inputFov);
    virtual std::shared_ptr<class Ray> GenerateRayForNormalizedCoordinates(glm::vec2 coordinate, bool depthOfFieldEnabled) const override;

    void SetZNear(float input);
    void SetZFar(float input);

private:
    float aspectRatio;
    float fov; // fov is stored as radians

    float zNear;
    float zFar;

	// depth of field variables. 
	// modify the constant values to change the rendering.
	const float zFocalPlane = 3.5f;
	const float lensRadius = 0.05F;
	const int nRaysFromLens = 16; // not used here. should be used in RayTracer.cpp.

};