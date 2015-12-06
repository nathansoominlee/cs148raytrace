#pragma once

#include "common/Application.h"
#include "assignment6/CommandLineArgs.h"

class Assignment6 : public Application
{
public:

    Assignment6(const CommandLineArgs &args);

    virtual std::shared_ptr<class Camera> CreateCamera() const override;
    virtual std::shared_ptr<class Scene> CreateScene() const override;
    virtual std::shared_ptr<class ColorSampler> CreateSampler() const override;
    virtual std::shared_ptr<class Renderer> CreateRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler) const override;
    virtual int GetSamplesPerPixel() const override { return this->samples_per_pixel; }
    virtual bool NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex) override;
    virtual int GetMaxReflectionBounces() const override;
    virtual int GetMaxRefractionBounces() const override;
    virtual glm::vec2 GetImageOutputResolution() const override;

    virtual std::string GetOutputFilename() const override { return this->output_filename; }

    std::shared_ptr<class Scene> CreateSceneFromSheet() const;

    bool ChunkingRequested() const { return this->chunking_requested; }

    int GetTotalChunks() const { return this->total_chunks; }

    int GetCurrentChunk() const { return this->current_chunk; }


private:

    std::string BuildOutputFilename(const CommandLineArgs &args);

    //https://docs.google.com/spreadsheets/d/1U00wnt888PKLGEc6vnDlIguzLnzOWJGJXCjst0bIE3s/pub?output=tsv
    //https://docs.google.com/spreadsheets/d/1U00wnt888PKLGEc6vnDlIguzLnzOWJGJXCjst0bIE3s/pub?output=tsv&gid=1923272220
    const std::string baseURL = 
        "https://docs.google.com/spreadsheets/d/";
    const std::string key = 
        "1U00wnt888PKLGEc6vnDlIguzLnzOWJGJXCjst0bIE3s";
    const std::string fmt = 
        "/pub?output=tsv"; // tab-separated values
    const std::string gid_objects =
        "&gid=0";
    const std::string gid_lights =
        "&gid=1923272220";


    const std::string objects_URL = "\"" + baseURL + key + fmt + gid_objects + "\"";
    const std::string lights_URL =  "\"" + baseURL + key + fmt + gid_lights + "\"";
    const std::string objects_PATH = "objects.tsv";
    const std::string lights_PATH = "lights.tsv";

    const std::string sheet_path_local = "C:/Users/natha/Downloads/scene_scanline.tsv";

    int samples_per_pixel;
    int res_width;
    int res_height;
    int total_chunks;
    int current_chunk;
    bool chunking_requested;
    std::string output_filename;

};
