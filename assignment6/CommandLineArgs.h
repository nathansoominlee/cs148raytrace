#pragma once

#ifndef __COMMAND_LINE_ARGS__
#define __COMMAND_LINE_ARGS__

#include "common/core.h"

class CommandLineArgs
{

public:

    static const std::string DEFAULT_OUTPUT_FILENAME_ROOT;

    CommandLineArgs(int argc, char **argv);

    // Did the user request that we post-process the chunks that have already been created?
    bool ChunkComposeRequested() const { return !this->input_filename_root.empty(); }
    
    // Did the user request only a specific chunk to be rendered
    bool ChunkingRequested() const { return this->total_chunks != -1; }

    std::string GetOutputFilenameRoot() const { return this->output_filename_root; }

    std::string GetInputFilenameRoot() const { return this->input_filename_root; }
    
    int GetResWidth() const { return this->res_width; }

    int GetResHeight() const { return this->res_height; }
    
    int GetCurrentChunk() const { return this->current_chunk; }
    
    int GetTotalChunks() const { return this->total_chunks; }

private:
    
    const int DEFAULT_RESOLUTION_WIDTH = 1280;
    const int DEFAULT_RESOLUTION_HEIGHT = 720;

    void ProcessArgs();

    void PrintUsage();

    void MissingArgAfter(std::string flag, std::string arg_after_flag);

    std::string output_filename_root = "";
    std::string input_filename_root = "";
    int res_width = -1;
    int res_height = -1;
    int current_chunk = -1;
    int total_chunks = -1;

};

#endif
