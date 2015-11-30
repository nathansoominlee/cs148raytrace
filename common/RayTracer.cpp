#include "common/RayTracer.h"
#include "common/Application.h"
#include "common/Scene/Scene.h"
#include "common/Scene/Camera/Camera.h"
#include "common/Scene/Geometry/Ray/Ray.h"
#include "common/Intersection/IntersectionState.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Output/ImageWriter.h"
#include "common/Rendering/Renderer.h"

#include "common/Scene/Geometry/Primitives/Triangle/Triangle.h"
RayTracer::RayTracer(std::unique_ptr<class Application> app):
    storedApplication(std::move(app))
{
}

void RayTracer::Run()
{
    // Scene Setup -- Generate the camera and scene.
    std::shared_ptr<Camera> currentCamera = storedApplication->CreateCamera();
    std::shared_ptr<Scene> currentScene = storedApplication->CreateScene();
    std::shared_ptr<ColorSampler> currentSampler = storedApplication->CreateSampler();
    std::shared_ptr<Renderer> currentRenderer = storedApplication->CreateRenderer(currentScene, currentSampler);
    assert(currentScene && currentCamera && currentSampler && currentRenderer);

    currentSampler->InitializeSampler(storedApplication.get(), currentScene.get());

    // Scene preprocessing -- generate acceleration structures, etc.
    // After this call, we are guaranteed that the "acceleration" member of the scene and all scene objects within the scene will be non-NULL.
    currentScene->GenerateDefaultAccelerationData();
    currentScene->Finalize();

    currentRenderer->InitializeRenderer();

    // Prepare for Output
    const glm::vec2 currentResolution = storedApplication->GetImageOutputResolution();
    ImageWriter imageWriter(storedApplication->GetOutputFilename(), static_cast<int>(currentResolution.x), static_cast<int>(currentResolution.y));

    // Perform forward ray tracing
    const int maxSamplesPerPixel = storedApplication->GetSamplesPerPixel();
    assert(maxSamplesPerPixel >= 1);

    for (int r = 0; r < static_cast<int>(currentResolution.y); ++r) {
        for (int c = 0; c < static_cast<int>(currentResolution.x); ++c) {

			if (!storedApplication->EnableDOF()) {

				imageWriter.SetPixelColor(currentSampler->ComputeSamplesAndColor(maxSamplesPerPixel, 2, [&](glm::vec3 inputSample) {
					const glm::vec3 minRange(-0.5f, -0.5f, 0.f);
					const glm::vec3 maxRange(0.5f, 0.5f, 0.f);
					const glm::vec3 sampleOffset = (maxSamplesPerPixel == 1) ? glm::vec3(0.f, 0.f, 0.f) : minRange + (maxRange - minRange) * inputSample;

					glm::vec2 normalizedCoordinates(static_cast<float>(c) + sampleOffset.x, static_cast<float>(r) + sampleOffset.y);
					normalizedCoordinates /= currentResolution;


					// Construct ray, send it out into the scene and see what we hit.
					std::shared_ptr<Ray> cameraRay = currentCamera->GenerateRayForNormalizedCoordinates(normalizedCoordinates, false);
					assert(cameraRay);

					IntersectionState rayIntersection(storedApplication->GetMaxReflectionBounces(), storedApplication->GetMaxRefractionBounces());
					bool didHitScene = currentScene->Trace(cameraRay.get(), &rayIntersection);

					// Use the intersection data to compute the BRDF response.
					glm::vec3 sampleColor;
					if (didHitScene) {
						sampleColor = currentRenderer->ComputeSampleColor(rayIntersection, *cameraRay.get());
					}
					return sampleColor;
				}), c, r);
			}

			else {

				imageWriter.SetPixelColor(currentSampler->ComputeSamplesAndColor(maxSamplesPerPixel, 2, [&](glm::vec3 inputSample) {
					const glm::vec3 minRange(-0.5f, -0.5f, 0.f);
					const glm::vec3 maxRange(0.5f, 0.5f, 0.f);
					const glm::vec3 sampleOffset = (maxSamplesPerPixel == 1) ? glm::vec3(0.f, 0.f, 0.f) : minRange + (maxRange - minRange) * inputSample;

					glm::vec2 normalizedCoordinates(static_cast<float>(c) + sampleOffset.x, static_cast<float>(r) + sampleOffset.y);
					normalizedCoordinates /= currentResolution;

					const int nSamples = storedApplication->GetNDepthOfFieldRays();	// number of rays shooting out from the aperature per image pixel.

					// array of added rgb values, later used to average
					glm::vec3 averagedColor;

					for (int i = 0; i < nSamples; i++) {
						// Construct a number of rays, send it out into the scene and see what we hit.
						std::shared_ptr<Ray> cameraRay = currentCamera->GenerateRayForNormalizedCoordinates(normalizedCoordinates, true);
						assert(cameraRay);

						IntersectionState rayIntersection(storedApplication->GetMaxReflectionBounces(), storedApplication->GetMaxRefractionBounces());
						bool didHitScene = currentScene->Trace(cameraRay.get(), &rayIntersection);

						// Use the intersection data to compute the BRDF response.
						glm::vec3 sampleColor;
						if (didHitScene) {
							sampleColor = currentRenderer->ComputeSampleColor(rayIntersection, *cameraRay.get());
						}
						averagedColor += sampleColor;
					}

					averagedColor /= nSamples;	// average.
					return averagedColor;
				}), c, r);

			}

        }
    }

    // Apply post-processing steps (i.e. tone-mapper, etc.).
    storedApplication->PerformImagePostprocessing(imageWriter);

    // Now copy whatever is in the HDR data and store it in the bitmap that we will save (aka everything will get clamped to be [0.0, 1.0]).
    imageWriter.CopyHDRToBitmap();

    // Save image.
    imageWriter.SaveImage();
}