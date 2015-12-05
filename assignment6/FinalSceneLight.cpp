#include "assignment6/FinalSceneLight.h"
#include "assignment6/Utility.h"

// The format for the .TSV file looks like this
//
//Parse?    name    description type    color (r, g, b) transformations (tx,ty,tz)
//y light1  red Point   0.1, 0.1, 0.8   -20, 2, 9.5
//y light2  blue    Point   0.1, 0.1, 0.8   -20, 2, 9.5
//y light3  green   Point   0.1, 0.4, 0.1   -7.4, 8, 1.8
//y light4  red Point   0.8, 0.1, 0.1   10, 2, -2.6
//y light5  blue    Point   0.1, 0.1, 0.6   -19.5, 2, 1.4
//y light6  green   Point   0.1, 0.7, 0.1   30, 2, 4

std::vector<FinalSceneLight> FinalSceneLight::MakeContainer(const Rows &rows)
{
    std::vector<FinalSceneLight> final_scene_lights;

    // r = 1 skips the first row of headers
    for (int r = 1; r < (int) rows.size(); r++)
    {
        auto row = rows[r];

        if (row.size() > 0 && row[0] == "y")
        {
            //std::cout << "Told to parse: " << std::endl;
            //PrintRow(row);

            FinalSceneLight fsl = ParseFSL(row);
            final_scene_lights.push_back(fsl);
        }
        else if (row.size() > 0 && row[0] == "n")
        {
            // We are told to skip this light
        }
        else
        {
            std::cerr << "Error: Unsure whether to parse row based on indicator: " << std::endl;
            PrintRow(row);
            exit(1);
        }

    } // for j

    return std::move(final_scene_lights);

}

void FinalSceneLight::PrintRow(std::vector<std::string> row)
{
    for (auto field : row)
    {
        std::cout << field << " ";
    }

    std::cout << std::endl;
}

FinalSceneLight FinalSceneLight::ParseFSL(std::vector<std::string> row)
{
    std::string name;
    std::string description;
    LightType light_type = FinalSceneLight::LightType::None;
    float red;
    float green;
    float blue;
    float tx;
    float ty;
    float tz;

    std::string field;
    for (int i = 0; i < (int) row.size(); i++)
    {
        field = row[i];

        switch(i)
        {
            case(Column::TryToParse):

                // already handled this column
                break;

            case(Column::Name):

                name = field;
                Utility::Print("Reading in ", name, "\n");
                break;

            case(Column::Description):

                description = field;
                break;

            case(Column::Type):

                if (field == "PointAttenuated")
                {
                    light_type = FinalSceneLight::LightType::PointAttenuated;
                }
                else if (field == "PointUnattenuated")
                {
                    light_type = FinalSceneLight::LightType::PointUnattenuated;
                }
                else
                {
                    light_type = FinalSceneLight::LightType::None;
                    std::cout << "Setting light type to be None." << std::endl;
                }
                break;

            case(Column::Color):
                FinalSceneLight::ParseColor(field, red, green, blue);
                break;

            case(Column::Transformations):
                FinalSceneLight::ParseTransformations(field, &tx, &ty, &tz);
                break;
        } // switch

    } // for i

    // Build and return the light
    FinalSceneLight fsl(name, description, light_type, red, green, blue, tx, ty, tz);

    return fsl;

}

void FinalSceneLight::AddContainer(const std::vector<FinalSceneLight>& final_scene_lights, std::shared_ptr<Scene> scene)
{
    for (auto fsl : final_scene_lights)
    {
        Utility::Print("Adding ", fsl.name, " to scene...", "\n");

        switch (fsl.light_type)
        {
            case FinalSceneLight::LightType::PointAttenuated:
            case FinalSceneLight::LightType::PointUnattenuated:
            {
                bool isAttenuated = fsl.light_type == FinalSceneLight::LightType::PointAttenuated ? true : false;
                glm::vec3 color(fsl.red, fsl.green, fsl.blue);
                glm::vec3 position(fsl.tx, fsl.ty, fsl.tz);
                Utility::AddPLight(color, position, isAttenuated, scene);
                break;
            }
            case FinalSceneLight::LightType::None:
            default:
                std::cout << "Skipping none or unidentified Light Type" << std::endl;
                break;
        }

    }
}

std::size_t FinalSceneLight::ReadFloat(const std::string& in_float, float* out_float)
{
    std::size_t sz;
    (*out_float) = std::stof(in_float, &sz);

    return sz;
}

void FinalSceneLight::ParseColor(const std::string& c_params, float& red, float& green, float& blue)
{
    std::string::size_type sub_pos = 0;
    for (int i = 0; i < 3; i++)
    {
        float param = -1;
        sub_pos += ReadFloat(c_params.substr(sub_pos), &param) + 1;

        if (i == 0)
        {
            red = param;
        }
        else if (i == 1)
        {
            green = param;
        }
        else //if (i == 1)
        {
            blue = param;
        }
    }
}

void FinalSceneLight::ParseTransformations(const std::string& t_params, float* tx, float* ty, float* tz)
{
    std::string::size_type sub_pos = 0;
    for (int i = 0; i < 3; i++)
    {
        float param = -1;
        sub_pos += ReadFloat(t_params.substr(sub_pos), &param) + 1;

        if (i == 0)
        {
            (*tx) = param;
        }
        else if (i == 1)
        {
            (*ty) = param;
        }
        else //if (i == 1)
        {
            (*tz) = param;
        }
    }
}

void FinalSceneLight::PrintContainer(const std::vector<FinalSceneLight>& final_scene_objects)
{
    std::cout << "container.size(): " << final_scene_objects.size() << std::endl;
    for (int i = 0; i < (int) final_scene_objects.size(); i++)
    {
        std::cout << "i: " << i << " fso: " << final_scene_objects[i] << std::endl;
    }
}

std::ostream& operator<< (std::ostream& os, const FinalSceneLight& fsl)
{
    std::string light = "None";
    switch(fsl.light_type)
    {
        case (FinalSceneLight::LightType::PointAttenuated):

            light = "PointAttenuated";
            break;
        
        case (FinalSceneLight::LightType::PointUnattenuated):

            light = "PointUnattenuated";
            break;

        case (FinalSceneLight::LightType::None):

            light = "None";
            break;
    }

    os << "name: "            << fsl.name
       << " description: "    << fsl.description
       << " light_type: "     << light
       << " red: "            << fsl.red
       << " green: "          << fsl.green
       << " blue: "           << fsl.blue
       << " tx: "             << fsl.tx
       << " ty: "             << fsl.ty
       << " tz: "             << fsl.tz;

    return os;
}
