#pragma once

#ifndef __UTILITY__
#define __UTILITY__

#include "common/core.h"

class Utility
{

public:

    static void AddPLight(glm::vec3 color, glm::vec3 position, std::shared_ptr<Scene> scene);

    //static std::shared_ptr<EpicShader> MakeEpicShader(float metallic, float roughness, float specular, std::string texture);

    static std::shared_ptr<BlinnPhongMaterial> MakeBlinnPhongMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess, 
                        float reflectivity, float transparency, float IOR, std::string texture);

    static std::shared_ptr<class SceneObject> LoadObj(std::shared_ptr<class Material> material, std::shared_ptr<Scene> scene, std::string object_path);

};

#endif
