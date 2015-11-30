#include "common/Application.h"
#include "common/Acceleration/AccelerationCommon.h"
#include "common/Output/ImageWriter.h"

std::string Application::GetOutputFilename() const
{
    return "output.png";
}

int Application::GetSamplesPerPixel() const
{
    return 16;
}

bool Application::DepthOfFieldEnabled() const
{
	return true;
}

int Application::GetNumRaysFromLens() const
{
	return 8;	// number of rays shot out from the lens to be averaged for the depth of field effect.
}

float Application::GetLensRadius() const
{
	return 5.0f;	// radius of the aperature. bigger lens may require more number of rays to be shot out.
}

float Application::GetFocalPlaneDistanceFromLens() const
{
	return 5.0f;	// distance of the focal plane from the aperature.
}

float Application::GetImagePlaneDistanceFromLens() const
{
	return 1.0f;	// distance of the image plane from the aperature.
}

glm::vec2 Application::GetImageOutputResolution() const
{
    return glm::vec2(1280.f, 720.f);
}

void Application::PerformImagePostprocessing(class ImageWriter&)
{
}