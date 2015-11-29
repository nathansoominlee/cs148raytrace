#include "assignment6/Utility.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Utility/Texture/TextureLoader.h"

void Utility::AddPLight(glm::vec3 color, glm::vec3 position, std::shared_ptr<Scene> scene)
{
    // turn on point light.
    std::shared_ptr<Light> pointLight = std::make_shared<PointLight>();

    pointLight->SetLightColor(glm::vec3(color));
    pointLight->SetPosition(position);

    scene->AddLight(pointLight);
}

// Make the BP material using args as parameters
std::shared_ptr<BlinnPhongMaterial> Utility::MakeBlinnPhongMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess, 
                        float reflectivity, float transparency, float IOR, std::string texture)
{
    
    std::shared_ptr<BlinnPhongMaterial> material = std::make_shared<BlinnPhongMaterial>();

    // BlinnPhongMaterial properties
    material->SetDiffuse(diffuse);
    material->SetSpecular(specular, shininess);

    // Material general properties
    material->SetReflectivity(reflectivity);
    material->SetTransmittance(1 - transparency);
    material->SetIOR(IOR);

    return material;
}


std::shared_ptr<class SceneObject> Utility::LoadObj(std::shared_ptr<class Material> material, std::shared_ptr<Scene> scene, std::string object_path)
{
    // ****Does not currently support mtl file, instead we just type params into the google
    // sheet and those get read by the sheet reader.


    std::vector<std::shared_ptr<MeshObject>> meshObject = MeshLoader::LoadMesh(object_path, nullptr);
    if (meshObject.empty()) 
    {   
        std::cerr << "ERROR: Failed to load the model. Check your paths." << std::endl;
        return nullptr;
    }           

    // set the material for each object in this obj
    for (auto obj : meshObject)
    {
        obj->SetMaterial(material);
    }

    // Make scene object and add it to the scene
    std::shared_ptr<class SceneObject> sceneObject = std::make_shared<SceneObject>();
    sceneObject->AddMeshObject(meshObject);
    scene->AddSceneObject(sceneObject);

    // Return handle to scene object so caller can translate/rotate/scale it
    return sceneObject;
}
