#include "common/Rendering/Renderer/Photon/PhotonMappingRenderer.h"
#include "common/Scene/Scene.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Scene/Lights/Light.h"
#include "common/Scene/Geometry/Primitives/Primitive.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "common/Intersection/IntersectionState.h"
#include "common/Scene/SceneObject.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "glm/gtx/component_wise.hpp"

#define VISUALIZE_PHOTON_MAPPING 1

PhotonMappingRenderer::PhotonMappingRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler):
    BackwardRenderer(scene, sampler), 
    diffusePhotonNumber(1000000),
    maxPhotonBounces(1000)
{
    srand(static_cast<unsigned int>(time(NULL)));
}

void PhotonMappingRenderer::InitializeRenderer()
{
    // Generate Photon Maps
    GenericPhotonMapGeneration(diffuseMap, diffusePhotonNumber);
    diffuseMap.optimise();
}

void PhotonMappingRenderer::GenericPhotonMapGeneration(PhotonKdtree& photonMap, int totalPhotons)
{
    float totalLightIntensity = 0.f;
    size_t totalLights = storedScene->GetTotalLights();
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }
        totalLightIntensity = glm::length(currentLight->GetLightColor());
    }

    // Shoot photons -- number of photons for light is proportional to the light's intensity relative to the total light intensity of the scene.
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }

        const float proportion = glm::length(currentLight->GetLightColor()) / totalLightIntensity;
        const int totalPhotonsForLight = static_cast<const int>(proportion * totalPhotons);
        const glm::vec3 photonIntensity = currentLight->GetLightColor() / static_cast<float>(totalPhotonsForLight);
        for (int j = 0; j < totalPhotonsForLight; ++j) {
            Ray photonRay;
            std::vector<char> path;
            path.push_back('L');
            currentLight->GenerateRandomPhotonRay(photonRay);
            TracePhoton(photonMap, &photonRay, photonIntensity, path, 1.f, maxPhotonBounces);
        }
    }
}

void PhotonMappingRenderer::TracePhoton(PhotonKdtree& photonMap, Ray* photonRay, glm::vec3 lightIntensity, 
        std::vector<char>& path, float currentIOR, int remainingBounces)
{

    // check the recursive base case
    if (remainingBounces < 0)
    {
        return;
    }

    assert(photonRay);
    IntersectionState state(0, 0);
    state.currentIOR = currentIOR;

    // Trace the photon ray and test for intersections
    bool didIntersect = storedScene->Trace(photonRay, &state);

    if (!didIntersect)
    {
        return;
    }

    // Get the normal to the intersection
    glm::vec3 normal = state.ComputeNormal();

    // Move the ray to the intersection point + a little offset
    glm::vec3 intersectionPoint = state.intersectionRay.GetRayPosition(state.intersectionT);
    photonRay->SetRayPosition(intersectionPoint + normal * SMALL_EPSILON);

    if (path.size() > 1) 
    {
        // This photon did NOT come directly from the light, so
        // create and store a new Photon in the photon map

        Ray toLightRay(glm::vec3(photonRay->GetPosition()), -photonRay->GetRayDirection(), photonRay->GetMaxT());
        
        Photon myPhoton;
        myPhoton.position = glm::vec3(photonRay->GetPosition());
        myPhoton.intensity = lightIntensity;
        myPhoton.toLightRay = toLightRay;

        photonMap.insert(myPhoton);
    }

    // --------determine whether this photon is scattered or absorbed--------

    const MeshObject* hitMeshObject = state.intersectedPrimitive->GetParentMeshObject();
    const Material* hitMaterial = hitMeshObject->GetMaterial();

    glm::vec3 diffuseResponse = hitMaterial->GetBaseDiffuseReflection();

    // the probability of reflection is the max of the RGB components of the diffuse response

    float max = diffuseResponse.x;
    if (diffuseResponse.y > max)
    {
        max = diffuseResponse.y;
    }

    if (diffuseResponse.z > max)
    {
        max = diffuseResponse.z;
    }

    // 0 to RAND_MAX
    float rand_f = static_cast<float>(rand());

    // 0 to 1
    rand_f /= RAND_MAX;

    if (rand_f > max)
    {
        // photon absorbed
        return;
    }

    // ------scatter the photon------

    // sample the hemisphere around the point in order to 
    // pick a diffuse reflection direction

    // 0 to RAND_MAX
    float u1 = static_cast<float>(rand());
    float u2 = static_cast<float>(rand());

    // 0 to 1
    u1 /= RAND_MAX;
    u2 /= RAND_MAX;

    float r = sqrt(u1);
    float theta = 2 * PI * u2;

    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1 - u1);

    glm::vec3 newDir = glm::normalize(glm::vec3(x, y, z));

    // ---transform from tangent space to world space---


    // pick one of (1, 0, 0) (0, 1, 0) (0, 0, 1) as long as
    // dot product is not close to 1 (therefore not parallel)
    // to cross the normal with

    glm::vec3 candidate(1.f, 0.f, 0.f);

    float dot = glm::dot(normal, candidate);
    if (dot > 0.9)
    {
        candidate = glm::vec3(0.f, 1.f, 0.f);
    }

    glm::vec3 tangent = glm::cross(normal, candidate);
    glm::vec3 bitangent = glm::cross(normal, tangent);

    // construct 3x3 matrix where columns of the matrix are the
    // tangent, bitangent, and normal vectors (in that order)
    glm::mat3 mat(tangent, bitangent, normal);

    // multiply 3x3 matrix by the newDir vector in the previous step
    // to get the diffuse reflection ray direction in world space
    glm::vec3 worldDir = mat * newDir;

    photonRay->SetRayDirection(worldDir);

    // append to path vector, decrement remaining bounces
    path.push_back('L');
    remainingBounces--;

    // recursive call
    TracePhoton(photonMap, photonRay, lightIntensity, path, currentIOR, remainingBounces);
}

glm::vec3 PhotonMappingRenderer::ComputeSampleColor(const struct IntersectionState& intersection, const class Ray& fromCameraRay) const
{
    glm::vec3 finalRenderColor = BackwardRenderer::ComputeSampleColor(intersection, fromCameraRay);
#if VISUALIZE_PHOTON_MAPPING
    Photon intersectionVirtualPhoton;
    intersectionVirtualPhoton.position = intersection.intersectionRay.GetRayPosition(intersection.intersectionT);

    std::vector<Photon> foundPhotons;
    diffuseMap.find_within_range(intersectionVirtualPhoton, 0.003f, std::back_inserter(foundPhotons));
    if (!foundPhotons.empty()) {
        finalRenderColor += glm::vec3(1.f, 0.f, 0.f);
    }
#endif
    return finalRenderColor;
}

void PhotonMappingRenderer::SetNumberOfDiffusePhotons(int diffuse)
{
    diffusePhotonNumber = diffuse;
}
