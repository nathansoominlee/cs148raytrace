#pragma once

#ifndef __FINALSCENEOBJECT__
#define __FINALSCENEOBJECT__

#include "common/core.h"
#include "assignment6/SheetReader.h"

class FinalSceneObject : public SceneObject
{

public:

    // Factory function to create a collection from the google sheet
    static std::vector<FinalSceneObject> MakeContainer(const Rows &rows); 

    // Print function for the created container
    static void PrintContainer(const std::vector<FinalSceneObject>& final_scene_objects);

    // Utility function to add a container of objects to the scene
    static void AddContainer(const std::vector<FinalSceneObject>& final_scene_objects, std::shared_ptr<Scene> scene);

private:

// The column data looks like this
// Parse? object description texture material:p1,p2,... reflectivity opacity IOR scale transformations:tx,ty,tz rotations:rx,ry,rz

    enum Column {TryToParse      = 0,
                 Object          = 1,
                 Description     = 2,
                 Texture         = 3,
                 Material        = 4,
                 Reflectivity    = 5,
                 Opacity         = 6,
                 IoR             = 7,
                 Scale           = 8,
                 Transformations = 9,
                 Rotations       = 10
                };

    enum class MaterialType { None, Epic, BP };

    FinalSceneObject(std::string name, 
                     std::string description, 
                     std::string texture,
                     MaterialType material_type,
                     float epic_metallic,
                     float epic_roughness,
                     float epic_specular,
                     glm::vec3 bp_diffuse,
                     glm::vec3 bp_specular,
                     float bp_shininess,
                     float reflectivity,
                     float opacity,
                     float IOR,
                     float scale,
                     float tx,
                     float ty,
                     float tz,
                     float rx,
                     float ry,
                     float rz) :
        name(name),
        description(description),
        texture(texture),
        material_type(material_type),
        epic_metallic(epic_metallic),
        epic_roughness(epic_roughness),
        epic_specular(epic_specular),
        bp_diffuse(bp_diffuse),
        bp_specular(bp_specular),
        bp_shininess(bp_shininess),
        reflectivity(reflectivity),
        opacity(opacity),
        IOR(IOR),
        scale(scale),
        tx(tx),
        ty(ty),
        tz(tz),
        rx(rx),
        ry(ry),
        rz(rz)
    {}

    // Non-static private member functions
    std::shared_ptr<class Material> MakeMaterial();
    std::shared_ptr<class SceneObject> LoadObj(std::shared_ptr<class Material> material, std::shared_ptr<Scene> scene);

    // Static private member functions
    static void PrintRow(std::vector<std::string> row);

    static FinalSceneObject ParseFSO(std::vector<std::string> row);

    static int ParseMaterial(const std::string& field,          // input parameter
                           MaterialType& material_type,  // output parameter
                           float* epic_metallic,              // output parameter
                           float* epic_roughness,             // output parameter
                           float* epic_specular,              // output parameter
                           glm::vec3& bp_diffuse,                 // output parameter
                           glm::vec3& bp_specular,
                           float* bp_shininess);               // output parameter

    static int ParseEpicParams(const std::string& epic_params, // input parameter
                               float* epic_metallic,           // output parameter
                               float* epic_roughness,          // output parameter
                               float* epic_specular);          // output parameter

    static int ParseBPParams(const std::string& bp_params, // input parameter
                             glm::vec3 &bp_diffuse,        // output parameter
                             glm::vec3 &bp_specular,
                             float* bp_shininess);   // output parameter

    static int ParseTransformations(const std::string& t_params,
                                    float* tx, float* ty, float* tz);

    static int ParseRotations(const std::string& r_params,
                              float* rx, float* ry, float* rz);

    static std::size_t ReadFloat(const std::string& in_float, float* out_float);

    //  operator overload
    friend std::ostream& operator<< (std::ostream& os, const FinalSceneObject& fso);

    std::string name;
    std::string description;
    std::string texture;
    MaterialType material_type;
    float epic_metallic;
    float epic_roughness;
    float epic_specular;
    glm::vec3 bp_diffuse;
    glm::vec3 bp_specular;
    float bp_shininess;
    float reflectivity;
    float opacity;
    float IOR;
    float scale;
    float tx;
    float ty;
    float tz;
    float rx;
    float ry;
    float rz;

};

#endif
