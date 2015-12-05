#pragma once

#ifndef __CHUNK_COMPOSER__
#define __CHUNK_COMPOSER__

#include "common/core.h"

class ChunkComposer 
{

public:
    
    ChunkComposer(const std::string input_filename);
    
    void ComposeChunks();

private:

    int res_width = -1;
    int res_height = -1;
    int total_chunks = -1;
    std::string output_filename;
    std::vector<std::string> input_filenames;

};

#endif
