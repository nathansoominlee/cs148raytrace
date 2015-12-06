#pragma once

#ifndef __COMMAND_LINE_ARGS__
#define __COMMAND_LINE_ARGS__

#include "common/core.h"

class CommandLineArgs
{

public:

    static const std::string DEFAULT_OUTPUT_FILENAME_ROOT;
    static bool VERBOSE;
    static bool DIAGNOSTICS;

    CommandLineArgs(int argc, char **argv);

    // Did the user request that we post-process the chunks that have already been created?
    bool ChunkComposeRequested() const { return !this->input_filename_root.empty(); }
    
    // Did the user request only a specific chunk to be rendered
    bool ChunkingRequested() const { return this->total_chunks != -1; }

    std::string GetOutputFilenameRoot() const { return this->output_filename_root; }

    std::string GetInputFilenameRoot() const { return this->input_filename_root; }
    
    int GetSamplesPerPixel() const { return this->samples_per_pixel; }

    int GetResWidth() const { return this->res_width; }

    int GetResHeight() const { return this->res_height; }
    
    int GetCurrentChunk() const { return this->current_chunk; }
    
    int GetTotalChunks() const { return this->total_chunks; }

private:
    
    const int DEFAULT_RESOLUTION_WIDTH = 1280;
    const int DEFAULT_RESOLUTION_HEIGHT = 720;
    const int DEFAULT_SAMPLES_PER_PIXEL = 1;

    void ProcessArgs();

    void PrintUsage();

    void MissingArgAfter(std::string flag, std::string arg_after_flag);

    std::string output_filename_root = "";
    std::string input_filename_root = "";
    int samples_per_pixel = DEFAULT_SAMPLES_PER_PIXEL;
    int res_width = DEFAULT_RESOLUTION_WIDTH;
    int res_height = DEFAULT_RESOLUTION_HEIGHT;
    int current_chunk = -1;
    int total_chunks = -1;

};

#endif
