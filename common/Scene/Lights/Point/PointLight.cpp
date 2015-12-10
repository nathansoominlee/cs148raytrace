#include "common/Scene/Lights/Point/PointLight.h"


void PointLight::ComputeSampleRays(std::vector<Ray>& output, glm::vec3 origin, glm::vec3 normal) const
{
    origin += normal * LARGE_EPSILON;
    const glm::vec3 lightPosition = glm::vec3(GetPosition());

	// volume light
	float lightRadius = 0.1f; //increase this value for smoother shadows.
	float randXOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
	float randYOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
	float randZOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
	const glm::vec3 newLightPosition = lightPosition + glm::vec3(GetRightDirection()) * randXOffset + glm::vec3(GetUpDirection()) * randYOffset + glm::vec3(GetForwardDirection()) * randZOffset;
	
	//const glm::vec3 rayDirection = glm::normalize(lightPosition - origin);
    //const float distanceToOrigin = glm::distance(origin, lightPosition);

	const glm::vec3 rayDirection = glm::normalize(newLightPosition - origin);
	const float distanceToOrigin = glm::distance(origin, newLightPosition);

    output.emplace_back(origin, rayDirection, distanceToOrigin);
}

float PointLight::ComputeLightAttenuation(glm::vec3 origin) const
{
    
    if (this->isAttenuated)
	{

		const glm::vec3 lightPosition = glm::vec3(GetPosition());

		// volume light
		float lightRadius = 0.1f;	//increase this value for smoother shadows.
		float randXOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
		float randYOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
		float randZOffset = -lightRadius + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (lightRadius + lightRadius)));
		const glm::vec3 newLightPosition = lightPosition + glm::vec3(GetRightDirection()) * randXOffset + glm::vec3(GetUpDirection()) * randYOffset + glm::vec3(GetForwardDirection()) * randZOffset;

        // Return a value betwen 0 and 1 for attenuation factor,
        // intensity for a point light falls off as 1 / d ** 2

        //const float dist = glm::distance(lightPosition, origin);
		const float dist = glm::distance(newLightPosition, origin);
        const float exponent = 1.1;
        return 1.f / pow(1.f + dist, exponent);
    }
    else
    {
        return 1.f;
    }
}

void PointLight::GenerateRandomPhotonRay(Ray& ray) const
{
    // Assignment 7 TODO: Fill in the random point light samples here.
}
