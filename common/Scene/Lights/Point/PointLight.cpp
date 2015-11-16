#include "common/Scene/Lights/Point/PointLight.h"


void PointLight::ComputeSampleRays(std::vector<Ray>& output, glm::vec3 origin, glm::vec3 normal) const
{
    origin += normal * LARGE_EPSILON;
    const glm::vec3 lightPosition = glm::vec3(GetPosition());
    const glm::vec3 rayDirection = glm::normalize(lightPosition - origin);
    const float distanceToOrigin = glm::distance(origin, lightPosition);
    output.emplace_back(origin, rayDirection, distanceToOrigin);
}

float PointLight::ComputeLightAttenuation(glm::vec3 origin) const
{
    return 1.f;
}

// Called for every photon we send out from the light,
// this function takes a reference to a 'Ray' object and
// sets the position and direction of the ray
void PointLight::GenerateRandomPhotonRay(Ray& ray) const
{
    // set ray position to the point light position
    ray.SetRayPosition(glm::vec3(this->GetPosition()));

    // pick random number between -1 and 1 for x, y, and z
    // by scaling rand() function, then normalize and use
    // to set the ray direction
        
    // 0 to RAND_MAX
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    do {

        // 0 to RAND_MAX
        x = static_cast<float>(rand());
        y = static_cast<float>(rand());
        z = static_cast<float>(rand());

        // 0 to 1
        x /= RAND_MAX;
        y /= RAND_MAX;
        z /= RAND_MAX;

        // 0 to 2
        x *= 2;
        y *= 2;
        z *= 2;

        // -1 to 1
        x -= 1;
        y -= 1;
        z -= 1;

    } while ( (powf(x, 2) + powf(y, 2) + powf(z, 2)) > 1 );

    glm::vec3 dir(x, y, z);

    glm::vec3 ndir = glm::normalize(dir);

    ray.SetRayDirection(ndir);
}
