#include "assignment6/Utility.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Utility/Texture/TextureLoader.h"

// need class def'n of Texture2D for material->SetTexture() calls
// to compile otherwise get "no matching function call" due to
// Texture2D for 2nd argument when Texture type required
// Read more here
// http://stackoverflow.com/questions/15247318/unable-to-pass-a-subclass-instance-as-argument-instead-of-superclass
#include "common/Rendering/Textures/Texture2D.h" 

void Utility::AddPLight(glm::vec3 color, glm::vec3 position, bool isAttenuated, std::shared_ptr<Scene> scene)
{
    // turn on point light.
    std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();

    pointLight->SetLightColor(glm::vec3(color));
    pointLight->SetPosition(position);
    pointLight->SetAttenuation(isAttenuated);

    scene->AddLight(pointLight);
}

// Make the BP material using args as parameters
std::shared_ptr<BlinnPhongMaterial> Utility::MakeBlinnPhongMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess, 
                        float reflectivity, float opacity, float IOR, std::string texture)
{
    
    std::shared_ptr<BlinnPhongMaterial> material = std::make_shared<BlinnPhongMaterial>();

    // BlinnPhongMaterial properties
    material->SetDiffuse(diffuse);
    material->SetSpecular(specular, shininess);

    // Material general properties
    material->SetReflectivity(reflectivity);
    material->SetTransmittance(1 - opacity);
    material->SetIOR(IOR);

    if (!texture.empty())
    {
        // material texture
        material->SetTexture("diffuseTexture", TextureLoader::LoadTexture(texture));
        material->SetTexture("specularTexture", TextureLoader::LoadTexture(texture));
    }

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
