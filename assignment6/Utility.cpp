#include "assignment6/Utility.h"

#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include "common/Utility/Texture/TextureLoader.h"

void Utility::AddPLight(glm::vec4 color, glm::vec3 position, std::shared_ptr<Scene> scene)
{
    // turn on point light.
    std::shared_ptr<Light> pointLight = std::make_shared<PointLight>();

    pointLight->SetLightColor(glm::vec3(color));
    pointLight->SetPosition(position);

    scene->AddLight(pointLight);
}

/* Returns an epic shader with the given parameters
// and texture. Any objects with different parameter/textures
// need different shaders.
std::shared_ptr<EpicShader> Utility::MakeEpicShader(float metallic, float roughness, float specular, std::string texture)
{
    std::unordered_map<GLenum, std::string> shaderSpec = { 
        { GL_VERTEX_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.vert" },
        { GL_FRAGMENT_SHADER, "brdf/epicshader/frag/noSubroutine/epicshader.frag"}
    };  

    std::shared_ptr<EpicShader> shader = std::make_shared<EpicShader>(shaderSpec, GL_FRAGMENT_SHADER);
    shader->SetMetallic(metallic);
    shader->SetRoughness(roughness);
    shader->SetSpecular(specular);

    if (!texture.empty())
    {   
        shader->SetTexture(EpicShader::TextureSlots::DIFFUSE, TextureLoader::LoadTexture(texture));
    }   

    return shader;
}
*/

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
