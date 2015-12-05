#include "assignment6/ChunkComposer.h"
#include <regex>

ChunkComposer::ChunkComposer(const std::string input_filename)
{
    // The argument is a file like 
    //          
    //          output1600x900_100-16.png
    //
    // resolution:        1600x900 
    // total chunks:      100
    // this file's chunk: 16
    //
    // From this we can build the rest of the chunks.

    //                                                   match[0] match[1] match[2]
    static const std::regex input_filename_regex("([a-zA-Z]+)([0-9]+)x([0-9]+)_([0-9]+)-[0-9]+(.[a-zA-Z]+)");
    //                                            output     1600    x900     _100     -16    .png
    // This is an acceptable matching: output1600x900_100-16.png  
    std::smatch match;

    std::string basename;
    std::string file_ext;

    if (std::regex_search(input_filename.begin(), input_filename.end(), match, input_filename_regex))
    {
        basename = match[1];
        std::cout << "captured basename: " << basename << std::endl;

        this->res_width = std::stoi(match[2]);
        std::cout << "captured res_width: " << this->res_width << std::endl;

        this->res_height = std::stoi(match[3]);
        std::cout << "captured res_height: " << this->res_height << std::endl;

        this->total_chunks = std::stoi(match[4]);
        std::cout << "captured total_chunks: " << this->total_chunks << std::endl;

        file_ext = match[5];
        std::cout << "captured file_ext: " << file_ext << std::endl;
    }
    else
    {
        std::cout << "Error: Failed to extract necessary fields from chunk file named " << input_filename << std::endl;
        exit(1);
    }

    for (int chunk = 1; chunk <= this->total_chunks; chunk++)
    {
        std::string fname = basename + std::to_string(res_width) + "x" + std::to_string(res_height) 
            + "_" + std::to_string(total_chunks) + "-" + std::to_string(chunk) + file_ext;

        this->input_filenames.push_back(fname);
    }
}
