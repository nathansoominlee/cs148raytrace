#pragma once

#ifndef __FINALSCENELIGHT__
#define __FINALSCENELIGHT__

#include "common/core.h"
#include "assignment6/SheetReader.h"

class FinalSceneLight
{

public:

    // Factory function to create a collection from the google sheet
    static std::vector<FinalSceneLight> MakeContainer(const Rows &rows); 

    // Print function for the created container
    static void PrintContainer(const std::vector<FinalSceneLight>& final_scene_lights);

    // Utility function to add a container of objects to the scene
    static void AddContainer(const std::vector<FinalSceneLight>& final_scene_lights, std::shared_ptr<Scene> scene);

private:

// The column data looks like this
// Parse? object description texture material:p1,p2,... reflectivity transparency IOR scale transformations:tx,ty,tz rotations:rx,ry,rz

    enum Column {TryToParse      = 0,
                 Name            = 1,
                 Description     = 2,
                 Type            = 3,
                 Color           = 4,
                 Transformations = 5
                };

    enum class LightType { None, Point };

    FinalSceneLight(std::string name, 
                    std::string description, 
                    LightType light_type,
                    float red,
                    float green,
                    float blue,
                    float tx,
                    float ty,
                    float tz) :
        name(name),
        description(description),
        light_type(light_type),
        red(red),
        green(green),
        blue(blue),
        tx(tx),
        ty(ty),
        tz(tz)
    {}

    // Non-static private member functions
    void AddLight(std::shared_ptr<Scene> scene);

    // Static private member functions
    static FinalSceneLight ParseFSL(std::vector<std::string> row);

    static void PrintRow(std::vector<std::string> row);

    static FinalSceneLight ParseLight(std::vector<std::string> row);

    static void ParseColor(const std::string& c_params, float& red, float& green, float& blue);

    static void ParseTransformations(const std::string& t_params,
                                    float* tx, float* ty, float* tz);

    static std::size_t ReadFloat(const std::string& in_float, float* out_float);

    //  operator overload
    friend std::ostream& operator<< (std::ostream& os, const FinalSceneLight& fso);

    std::string name;
    std::string description;
    LightType light_type;
    float red;
    float green;
    float blue;
    float tx;
    float ty;
    float tz;
};

#endif
