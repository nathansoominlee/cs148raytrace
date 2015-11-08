#include "assignment6/Assignment6.h"
#include "common/core.h"
#include "assignment6/SheetReader.h"
#include "assignment6/FinalSceneObject.h"
#include "assignment6/Utility.h"


std::shared_ptr<Camera> Assignment6::CreateCamera() const
{
    /*
    const glm::vec2 resolution = GetImageOutputResolution();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 26.6f);
    camera->SetPosition(glm::vec3(0.f, -4.1469f, 0.73693f));
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
    return camera;
    */

    // Create the camera
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(1280.f / 720.f, 60.f);

    // Set camera position and rotation
    camera->SetPosition(glm::vec3(15.f, 40.f, 80.f));
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), -20.f * PI / 180.f);
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), 0.1f * 1);
    camera->Rotate(glm::vec3(0.f, 1.f, 0.f), -29.2f* PI / 180.f);
    camera->Rotate(glm::vec3(0.f, 1.f, 0.f), 0.1f * 6.5);
    camera->Rotate(glm::vec3(0.f, 0.f, 1.f), 0.f * PI / 180.f);

    return camera;
}

std::shared_ptr<Scene> Assignment6::CreateScene() const
{
    return CreateSceneFromSheet();
}

std::shared_ptr<Scene> Assignment6::CreateSceneFromSheet() const
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    // Import the sheet
    Rows rows;
    SheetReader::ImportSheet(this->sheetURL, rows);

    // Parse the objects
    std::vector<FinalSceneObject> objects;
    FinalSceneObject::MakeContainer(rows, objects);

    // Add to scene
    FinalSceneObject::AddContainer(objects, scene);

    // Turn on the lights
    //AddPLight();
    //AddHLight();
    //AddDLight();
    

    // Hard-coded point lights 
    glm::vec4 color;
    glm::vec3 position;

    float factor = 5;

    color = glm::vec4(0.8f, 0.1f, 0.2f, 1.5f);	//red
    position = glm::vec3(-33.046f, 2.f, 4.295f);
    Utility::AddPLight(color * factor, position, scene);

    color = glm::vec4(0.1f, 0.1f, 0.8f, 0.9f);	//blue
    position = glm::vec3(-19.947f, 2.f, 9.482f);
    Utility::AddPLight(color * factor, position, scene);

    color = glm::vec4(0.1f, 0.4f, 0.1f, 0.9f);	//green
    position = glm::vec3(-7.37f, 8.f, 1.813f);
    Utility::AddPLight(color * factor, position, scene);

    color = glm::vec4(0.8f, 0.1f, 0.1f, 0.6f);	//red
    position = glm::vec3(10.0f, 2.f, -2.623f);
    Utility::AddPLight(color * factor, position, scene);

    color = glm::vec4(0.1f, 0.1f, 0.6f, 0.4f);	//blue
    position = glm::vec3(-19.455f, 2.f, 1.371f);
    Utility::AddPLight(color * factor, position, scene);

    color = glm::vec4(0.1f, 0.7f, 0.1f, 1.2f);	//green
    position = glm::vec3(30.177f, 2.f, 3.993f);
    Utility::AddPLight(color * factor, position, scene);

    return scene;
}


/*
    std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

    // Material
    std::shared_ptr<BlinnPhongMaterial> cubeMaterial = std::make_shared<BlinnPhongMaterial>();
    cubeMaterial->SetDiffuse(glm::vec3(1.f, 1.f, 1.f));
    cubeMaterial->SetSpecular(glm::vec3(0.6f, 0.6f, 0.6f), 40.f);
    //cubeMaterial->SetReflectivity(0.3f);

    // Objects
    std::vector<std::shared_ptr<aiMaterial>> loadedMaterials;
    std::vector<std::shared_ptr<MeshObject>> cubeObjects = MeshLoader::LoadMesh("CornellBox/CornellBox-Assignment6-Alt.obj", &loadedMaterials);
    for (size_t i = 0; i < cubeObjects.size(); ++i) {
        std::shared_ptr<Material> materialCopy = cubeMaterial->Clone();
        materialCopy->LoadMaterialFromAssimp(loadedMaterials[i]);
        cubeObjects[i]->SetMaterial(materialCopy);

        std::shared_ptr<SceneObject> cubeSceneObject = std::make_shared<SceneObject>();
        cubeSceneObject->AddMeshObject(cubeObjects[i]);
        cubeSceneObject->Rotate(glm::vec3(1.f, 0.f, 0.f), PI / 2.f);
        cubeSceneObject->CreateAccelerationData(AccelerationTypes::BVH);

        cubeSceneObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });

        cubeSceneObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });


        newScene->AddSceneObject(cubeSceneObject);
    }

    // Lights
    std::shared_ptr<Light> pointLight = std::make_shared<PointLight>();
    pointLight->SetPosition(glm::vec3(0.01909f, 0.0101f, 1.97028f));
    pointLight->SetLightColor(glm::vec3(1.f, 1.f, 1.f));
				
// Assignment 6 Part 1 TODO: Change the '1' here.
// ACCELERATION_TYPE 0 will use native acceleration
// ACCELERATION_TYPE 1 will use a bounding volume hierarchy
// ACCELERATION_TYPE 2 will use a uniform grid
// Record the time it takes for each configuration

#define ACCELERATION_TYPE 2
#if ACCELERATION_TYPE == 0
    newScene->GenerateAccelerationData(AccelerationTypes::NONE);
#elif ACCELERATION_TYPE == 1
    newScene->GenerateAccelerationData(AccelerationTypes::BVH);
#else
    UniformGridAcceleration* accelerator = dynamic_cast<UniformGridAcceleration*>(newScene->GenerateAccelerationData(AccelerationTypes::UNIFORM_GRID));
    assert(accelerator);
    // Assignment 6 Part 2 TODO: Change the glm::ivec3(10, 10, 10) here.
    accelerator->SetSuggestedGridSize(glm::ivec3(10, 10, 10));
#endif    
    newScene->AddLight(pointLight);

    return newScene;

}
*/

std::shared_ptr<ColorSampler> Assignment6::CreateSampler() const
{
    std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
    jitter->SetGridSize(glm::ivec3(1, 1, 1));
    return jitter;
}

std::shared_ptr<class Renderer> Assignment6::CreateRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) const
{
    return std::make_shared<BackwardRenderer>(scene, sampler);
}

int Assignment6::GetSamplesPerPixel() const
{
    return 1;
}

bool Assignment6::NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex)
{
    return true;
}

int Assignment6::GetMaxReflectionBounces() const
{
    return 0;
}

int Assignment6::GetMaxRefractionBounces() const
{
    return 4;
}

glm::vec2 Assignment6::GetImageOutputResolution() const
{
    return glm::vec2(640.f, 480.f);
}
