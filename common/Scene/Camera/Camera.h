#pragma once

#include "common/Scene/SceneObject.h"

class Camera : public SceneObject
{
public:
    Camera();

	virtual std::shared_ptr<class Ray> GenerateRayForNormalizedCoordinates(glm::vec2 coordinate, bool depthOfFieldEnabled) const = 0;
};