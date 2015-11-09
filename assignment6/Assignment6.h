#pragma once

#include "common/Application.h"

class Assignment6 : public Application
{
public:
    virtual std::shared_ptr<class Camera> CreateCamera() const override;
    virtual std::shared_ptr<class Scene> CreateScene() const override;
    virtual std::shared_ptr<class ColorSampler> CreateSampler() const override;
    virtual std::shared_ptr<class Renderer> CreateRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler) const override;
    virtual int GetSamplesPerPixel() const override;
    virtual bool NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex) override;
    virtual int GetMaxReflectionBounces() const override;
    virtual int GetMaxRefractionBounces() const override;
    virtual glm::vec2 GetImageOutputResolution() const override;

    std::shared_ptr<class Scene> CreateSceneFromSheet() const;

private:
//https://docs.google.com/spreadsheets/d/1U00wnt888PKLGEc6vnDlIguzLnzOWJGJXCjst0bIE3s/pub?output=tsv
    const std::string baseURL = 
        "https://docs.google.com/spreadsheets/d/";
    const std::string key = 
        "1U00wnt888PKLGEc6vnDlIguzLnzOWJGJXCjst0bIE3s";
    const std::string fmt = 
        "/pub?output=tsv"; // tab-separated values


    const std::string sheetURL = baseURL + key + fmt;
    const std::string sheet_path = "final_scene_sheet.tsv";
	const std::string sheet_path_local = "C:/Users/natha/Downloads/scene_scanline.tsv";

};
