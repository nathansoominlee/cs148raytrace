#include "assignment6/FinalSceneObject.h"

#include "assignment6/Utility.h"

// The format for the .TSV file looks like this
//
// Try to parse? object description texture shader:p1,p2{,p3} transformations:tx,ty,tz 
// y blind_window blinds and the window. pane not added. None Epic:0.5,0,0.5 23,30,21.4 
// y pearl_necklace Hangs off dummy's right arm None Epic:0.5,0,0.5 -6.107,26.359,-2.537 
// y banister The ground plane for our scene None Epic:0.5,0,0.6 0,0,0 
// y wall  The background of the scene None Epic:0.5,0,0.7 0,0,-23 
// y dummy_body Full body of the dummy dummy/Texture/dummy_wood.jpg Epic:0.5,0,0.8 0,2.8,0 
// y dummy_stand The pole extending from the base to body dummy/Texture/dummy_wood.jpg Epic:0.5,0,0.9 0,2.8,0 
// n light_christmas    0,2.7,0 
// n necklace_pearl    -5.113,27.9,-1.648 
// n camera  FOV: 35.9  zNear: 0.1  zFar: 1000  -0.607,40.868,115.363 
// n persp (camera)  FOV: 39  zNear: 0.1  zFar: 1000  0,31.5,0 
//
// Which we have have in rows and need to turn into a container of FinalSceneObjects

// Driver function for ParseSFO
int FinalSceneObject::MakeContainer(const Rows &rows,                      // input parameter
                      std::vector<FinalSceneObject> &final_scene_objects)  // output parameter
{
    // r = 1 skips the first row of headers
    for (int r = 1; r < (int) rows.size(); r++)
    {
        auto row = rows[r];

        if (row.size() > 0 && row[0] == "y")
        {
            //std::cout << "Told to parse: " << std::endl;
            //PrintRow(row);

            FinalSceneObject fso = ParseFSO(row);
            final_scene_objects.push_back(fso);
        }
        else if (row.size() > 0 && row[0] == "n")
        {
            // We are told to skip this object
        }
        else
        {
            std::cerr << "Error: Unsure whether to parse row based on indicator: " << std::endl;
            PrintRow(row);
            exit(1);
        }

    } // for j

    return 0;

} // FinalSceneObject::MakeContainer()

void FinalSceneObject::PrintRow(std::vector<std::string> row)
{
    for (auto field : row)
    {
        std::cout << field << " ";
    }

    std::cout << std::endl;
}

FinalSceneObject FinalSceneObject::ParseFSO(std::vector<std::string> row)
{
    std::string name = "";
    std::string description = "";
    std::string texture = "";
    ShaderType shader_type = ShaderType::None;
    float epic_metallic = -1.f;
    float epic_roughness = -1.f;
    float epic_specular = -1.f;
    glm::vec4 bp_diffuse = glm::vec4(-1.f, -1.f, -1.f, -1.f);
    glm::vec4 bp_specular = glm::vec4(-1.f, -1.f, -1.f, -1.f);
    float scale = -1.f;
    float tx = -1.f;
    float ty = -1.f;
    float tz = -1.f;
    float rx = -1.f;
    float ry = -1.f;
    float rz = -1.f;


    std::string field;
    for (int i = 0; i < (int) row.size(); i++)
    {
        field = row[i];

        switch(i)
        {

            case(Column::TryToParse):

                // already handled this column
                break;

            case(Column::Object):

                name = field;
                std::cout << "Reading in " << name << std::endl;
                break;

            case(Column::Description):

                description = field;
                break;

            case(Column::Texture):

                if (!(field == "None"))
                {
                    texture = field;
                }
                break;

            case(Column::Shader):

                FinalSceneObject::ParseShader(field,
                                              shader_type,
                                              &epic_metallic,
                                              &epic_roughness,
                                              &epic_specular,
                                              bp_diffuse,
                                              bp_specular);
                break;

            case(Column::Scale):
                FinalSceneObject::ParseScale(field, &scale);
                break;
            case(Column::Transformations):
                FinalSceneObject::ParseTransformations(field, &tx, &ty, &tz);
                break;
            case(Column::Rotations):
                FinalSceneObject::ParseRotations(field, &rx, &ry, &rz);
                break;
        } // switch

    } // for i

    // Build and return the object
    FinalSceneObject fso(name, description, texture, shader_type, \
                    epic_metallic, epic_roughness, epic_specular, bp_diffuse, bp_specular, \
                    scale, tx, ty, tz, rx, ry, rz);

    return fso;

} //FinalSceneObject::ParseFSO()

void FinalSceneObject::AddContainer(const std::vector<FinalSceneObject>& final_scene_objects, std::shared_ptr<Scene> scene)
{
    for (auto fso : final_scene_objects)
    {
        // Get the shader
        std::shared_ptr<class ShaderProgram> shader = fso.MakeShader();

        // Load the object into the scene
        std::shared_ptr<class SceneObject> so = fso.LoadObj(shader, scene);

        // Position the object
        so->SetPosition(glm::vec3(fso.tx, fso.ty, fso.tz));

        // Rotate the object
        so->Rotate(glm::vec3(1.f, 0.f, 0.f), fso.rx * PI / 180.f);
        so->Rotate(glm::vec3(0.f, 1.f, 0.f), fso.ry * PI / 180.f);
        so->Rotate(glm::vec3(0.f, 0.f, 1.f), fso.rz * PI / 180.f);

        // Scale the object
        so->MultScale(fso.scale);
    }
}

std::shared_ptr<class SceneObject> FinalSceneObject::LoadObj(std::shared_ptr<class ShaderProgram> shader, std::shared_ptr<Scene> scene)
{
    std::string object_path = "objects/" + this->name + ".obj";
    return Utility::LoadObj(shader, scene, object_path);
}

std::shared_ptr<class ShaderProgram> FinalSceneObject::MakeShader()
{

    std::string texture_path = "";
    if (!this->texture.empty())
    {
        texture_path = "textures/" + this->texture;
    }

    switch (this->shader_type)
    {
        case (ShaderType::Epic):
            return Utility::MakeEpicShader(this->epic_metallic, this->epic_roughness, this->epic_specular, texture_path);

        case (ShaderType::BP):
            return Utility::MakeBlinnPhongShader(this->bp_diffuse, this->bp_specular, texture_path);

        case (ShaderType::None):
            std::cerr << "MakeShader: Don't know how to make None shader type." << std::endl;
    }

    return nullptr;
}

// returns 0 on success
int FinalSceneObject::ParseShader(const std::string& field,          // input parameter
                                 ShaderType& shader_type,  // output parameter
                                 float* epic_metallic,              // output parameter
                                 float* epic_roughness,             // output parameter
                                 float* epic_specular,              // output parameter
                                 glm::vec4& bp_diffuse,                 // output parameter
                                 glm::vec4& bp_specular)                // output parameter
{
    // Should look like either 
    // Epic:0.5,0,0.6 or BP:0.5,0.5,0.5,1.0,0.5,0.5,0.5,1.0

    std::string colon = ":";
    std::size_t colon_pos = field.find(colon);

    if (colon_pos == std::string::npos)
    {
        std::cerr << "Error: Malformatted shader_type specification " << field << std::endl;
        exit(1);
    }

    // Check which shader this is
    std::string shader_spec = field.substr(0, colon_pos);

    int retval = -1;
    if (shader_spec == "Epic")
    {
        // Set what we know
        shader_type = ShaderType::Epic;
        bp_diffuse = glm::vec4(-1.f, -1.f, -1.f, -1.f);
        bp_specular = glm::vec4(-1.f, -1.f, -1.f, -1.f);

        // Process Epic parameters
        std::string epic_params(field.substr(colon_pos + 1));

        retval = FinalSceneObject::ParseEpicParams(epic_params,
                                                   epic_metallic,
                                                   epic_roughness,
                                                   epic_specular);
    }
    else if (shader_spec == "BP")
    {
        // Set what we know
        shader_type = ShaderType::BP;
        (*epic_metallic) = -1;
        (*epic_roughness) = -1;
        (*epic_specular) = -1;

        std::string bp_params(field.substr(colon_pos + 1));

        retval = FinalSceneObject::ParseBPParams(bp_params,
                                                 bp_diffuse,
                                                 bp_specular);
    }
    else
    {
        std::cerr << "Error: Unrecognized shader specification " << shader_spec << std::endl;
        exit(1);
    }

    return retval;
}

// returns 0 on success
int FinalSceneObject::ParseEpicParams(const std::string& epic_params, // input parameter
                                      float* epic_metallic,           // output parameter
                                      float* epic_roughness,          // output parameter
                                      float* epic_specular)           // output parameter
{
    // Collect the 3 parameters

    std::string::size_type sub_pos = 0;
    for (int i = 0; i < 3; i++)
    {

        float param = -1;
        sub_pos += ReadFloat(epic_params.substr(sub_pos), &param) + 1;

        if (i == 0) 
        {
            (*epic_metallic) = param;
        }
        else if (i == 1)
        {
            (*epic_roughness) = param;
        }
        else //if (i == 2)
        {
            (*epic_specular) = param;
        }

    } 

    return 0;
}

// returns 0 on success
int FinalSceneObject::ParseBPParams(const std::string& bp_params, // input parameter
                                    glm::vec4 &bp_diffuse,        // output parameter
                                    glm::vec4 &bp_specular)    // output parameter
{
    // Collect the 4 parameters for each vec4, 4 * 2 = 8 
    int n = 8;
    
    std::string::size_type sub_pos = 0;
    float v1 = -1;
    float v2 = -1;
    float v3 = -1;
    float v4 = -1;
    for (int i = 0; i < n; i++)
    {
        float param = -1;
        sub_pos += ReadFloat(bp_params.substr(sub_pos), &param) + 1;

        if ( i == 0 || (i == n - 4) )
        {
            v1 = param;
        }
        else if ( i == 1 || (i == n - 3) )
        {
            v2 = param;
        }
        else if ( i == 2 || (i == n - 2) )
        {
            v3 = param;
        }
        else if ( i == 3 || (i == n - 1) )
        {
            v4 = param;
            if (i == 3)
            {
                bp_diffuse = glm::vec4(v1, v2, v3, v4);
                v1 = -1;
                v2 = -1;
                v3 = -1;
                v4 = -1;
            }
            else if (i == n - 1)
            {
                bp_specular = glm::vec4(v1, v2, v3, v4);
            }
        }
    }

    return 0;
}

std::size_t FinalSceneObject::ReadFloat(const std::string& in_float, float* out_float)
{
    std::size_t sz;
    (*out_float) = std::stof(in_float, &sz);

    return sz;
}

// Returns 0 on success
int FinalSceneObject::ParseTransformations(const std::string& t_params,
                                           float* tx, float* ty, float* tz)
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

    return 0;
}

// Returns 0 on success
int FinalSceneObject::ParseRotations(const std::string& r_params,
                                     float* rx, float* ry, float* rz)
{
    std::string::size_type sub_pos = 0;
    for (int i = 0; i < 3; i++)
    {
        float param = -1;
        sub_pos += ReadFloat(r_params.substr(sub_pos), &param) + 1;

        if (i == 0)
        {
            (*rx) = param;
        }
        else if (i == 1)
        {
            (*ry) = param;
        }
        else //if (i == 1)
        {
            (*rz) = param;
        }
    }
    return 0;
}

// Returns 0 on success
int FinalSceneObject::ParseScale(const std::string& scale_param,
                                 float* scale)
{
    float param = std::stof(scale_param);
    (*scale) = param;
    return 0;
}

void FinalSceneObject::PrintContainer(const std::vector<FinalSceneObject>& final_scene_objects)
{
    std::cout << "container.size(): " << final_scene_objects.size() << std::endl;
    for (int i = 0; i < (int) final_scene_objects.size(); i++)
    {
        std::cout << "i: " << i << " fso: " << final_scene_objects[i] << std::endl;
    }
}

std::ostream& operator<< (std::ostream& os, const FinalSceneObject& fso)
{
    std::string shader = "None";
    switch(fso.shader_type)
    {
        case (FinalSceneObject::ShaderType::Epic):

            shader = "Epic";
            break;

        case (FinalSceneObject::ShaderType::BP):

            shader = "BP";
            break;

        case (FinalSceneObject::ShaderType::None):

            shader = "None";
            break;
    }

    os << "name: "            << fso.name
       << " description: "    << fso.description
       << " texture: "        << fso.texture
       << " shader: "         << shader
       << " epic_metallic: "  << fso.epic_metallic
       << " epic_roughness: " << fso.epic_roughness
       << " epic_specular: "  << fso.epic_specular
       << " bp_diffuse: "     << fso.bp_diffuse[0] << " " << fso.bp_diffuse[1] << " " << fso.bp_diffuse[2] << " " << fso.bp_diffuse[3]
       << " bp_specular: "    << fso.bp_specular[0] << " " << fso.bp_specular[1] << " " << fso.bp_specular[2] << " " << fso.bp_specular[3]
       << " scale: "          << fso.scale
       << " tx: "             << fso.tx
       << " ty: "             << fso.ty
       << " tz: "             << fso.tz
       << " rx: "             << fso.rx
       << " ry: "             << fso.ry
       << " rz: "             << fso.rz;

    return os;
}
