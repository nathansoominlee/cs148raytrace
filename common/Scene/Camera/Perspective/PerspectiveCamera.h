#pragma once

#include "common/Scene/Camera/Camera.h"

class PerspectiveCamera : public Camera
{
public:
    // inputFov is in degrees. 
    PerspectiveCamera(float aspectRatio, float inputFov);
    virtual std::shared_ptr<class Ray> GenerateRayForNormalizedCoordinates(glm::vec2 coordinate) const override;

    void SetZNear(float input);
    void SetZFar(float input);

private:
    float aspectRatio;
    float fov; // fov is stored as radians

    float zNear;
    float zFar;

	// depth of field variables. 
	// modify the constant values to change the rendering.
	const bool depthOfFieldEnabled = true;
	const float zFocalPlane = 5.0f;
	const float zImagePlane = 1.0f;
	const float lensRadius = 0.5f;
	const int nRaysFromLens = 16; // not used here. should be used in RayTracer.cpp.

};