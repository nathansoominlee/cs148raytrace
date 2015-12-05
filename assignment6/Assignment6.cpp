#include "assignment6/Assignment6.h"
#include "assignment6/Utility.h"
#include "common/core.h"
#include "assignment6/SheetReader.h"
#include "assignment6/FinalSceneObject.h"
#include "assignment6/FinalSceneLight.h"
#include "assignment6/Utility.h"

#include <fstream>

Assignment6::Assignment6(const CommandLineArgs &args)
{
    this->res_width = args.GetResWidth();
    this->res_height = args.GetResHeight();

    if (args.ChunkingRequested())
    {
        this->total_chunks = args.GetTotalChunks();
        this->current_chunk = args.GetCurrentChunk();
        this->chunking_requested = true;
    }
    else
    {
        this->total_chunks = -1;
        this->current_chunk = -1;
        this->chunking_requested = false;
    }
    this->output_filename = BuildOutputFilename(args);
}

std::string Assignment6::BuildOutputFilename(const CommandLineArgs &args)
{
    std::string filename;
    std::string file_fmt(".png");

    if (args.GetTotalChunks() == -1)
    {
        // The user did not request chunking
        filename = args.GetOutputFilenameRoot(); 
    }
    else
    {
        // The user requested chunking, so we have to append the resolution, total chunks and
        // current chunk to the output so that we know how to compose the file
        filename = args.GetOutputFilenameRoot() + 
                               std::to_string(args.GetResWidth()) + "x" + std::to_string(args.GetResHeight()) +
                               "_" + std::to_string(args.GetTotalChunks()) + 
                               "-" + std::to_string(args.GetCurrentChunk());
    }

    // Do not clobber any output files that already exist
    int i = 0;
    std::ifstream file(filename + file_fmt);
    while (file.good()) // checks that the file exists
    {
        file = std::ifstream(filename + "_" + std::to_string(++i) + file_fmt);
    }

    std::string retvalue;
    if (i > 0)
    {
        retvalue = filename + "_" + std::to_string(i) + file_fmt;
    }
    else
    {
        retvalue = filename + file_fmt;
    }

    return std::move(retvalue);
}

std::shared_ptr<Camera> Assignment6::CreateCamera() const
{

    // Create the camera
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(1280.f / 720.f, 30.f);

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

    // Import the object sheet
    bool use_cached_sheet = false;
    Rows rows = SheetReader::ImportSheet(this->objects_URL, this->objects_PATH, use_cached_sheet);

    // Parse the objects
    std::cout << "Parsing objects..." << std::endl;
    std::vector<FinalSceneObject> objects = FinalSceneObject::MakeContainer(rows);

    std::cout << "Adding object container to scene..." << std::endl;
    FinalSceneObject::AddContainer(objects, scene);

    // bounding volume hierarchy
    std::cout << "Generating acceleration data..." << std::endl;
    scene->GenerateAccelerationData(AccelerationTypes::BVH); 

    // Import the light sheet
    use_cached_sheet = false;
    rows = SheetReader::ImportSheet(this->lights_URL, this->lights_PATH, use_cached_sheet);

    // Parse the lights
    std::cout << "Parsing lights..." << std::endl;
    std::vector<FinalSceneLight> lights = FinalSceneLight::MakeContainer(rows);

    std::cout << "Adding light container to scene..." << std::endl;
    FinalSceneLight::AddContainer(lights, scene);

    std::cout << "Returning new scene..." << std::endl;
    return scene;
}

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
    return 4;
}

int Assignment6::GetMaxRefractionBounces() const
{
    return 4;
}

glm::vec2 Assignment6::GetImageOutputResolution() const
{
    return glm::vec2(static_cast<float>(this->res_width), 
                     static_cast<float>(this->res_height));
}
