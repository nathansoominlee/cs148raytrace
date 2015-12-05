#include "assignment6/ChunkComposer.h"
#include "assignment6/Utility.h"
#include "common/Output/ImageWriter.h"
#include "FreeImage.h"
#include <regex>
#include <fstream>

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

    // Create the composition filename based on the chunk names
    // output1600x900_100-16.png -> output_Composition1600x900.png 
    this->output_filename = basename + "_Composition" + std::to_string(res_width) + "x" + std::to_string(res_height) 
                            + file_ext;

    // Build the list of filenames
    for (int chunk = 0; chunk < this->total_chunks; chunk++)
    {
        std::string fname = basename + std::to_string(res_width) + "x" + std::to_string(res_height) 
            + "_" + std::to_string(total_chunks) + "-" + std::to_string(chunk) + file_ext;

        // check that the file exists
        std::ifstream infile(fname);
        if (infile.good())
        {
            this->input_filenames.push_back(fname);
        }
        else
        {
            std::cout << "Error: could not find chunk " << fname << std::endl;
            exit(1);
        }
    }
}

// Take all the chunk files and copy their pixels to a new file and save it
void ChunkComposer::ComposeChunks()
{
    // Initialize Free Image and allocate buffer
    FreeImage_Initialise();

    // Hard-code bits per pixel to 24, 3 bytes to store RGB
    FIBITMAP* out_bitmap = FreeImage_Allocate(this->res_width, this->res_height, 24);

    // Assume the chunks and composition are PNGs
    FREE_IMAGE_FORMAT fm = FIF_PNG;

    for (int i = 0; i < (int) this->input_filenames.size(); i++)
    {
        const char *fname = this->input_filenames[i].c_str();
        std::cout << "Processing " << fname << std::endl;

        // Copy the chunk pixels into the output bitmap, assume PNG
        FIBITMAP* in_bitmap = FreeImage_Load(fm, fname, PNG_DEFAULT);

        // Determine this chunk's position in the final image
        int start_c = -1;
        int start_r = -1;
        int end_c = -1;
        int end_r = -1;
        Utility::CalculateChunk(this->res_height, this->res_width, i, this->total_chunks,
                start_c, start_r, end_c, end_r);

        // Copy the pixels like reading a book, top-bottom left-right
        for (int r = start_r; r < end_r; r++) 
        {
            for (int c = start_c; c < end_c; c++) 
            {
                RGBQUAD color;

                // Get the pixel color in the chunk
                // Make row-argument correction: while our convention has top-left as (0, 0), 
                // FreeImage has bottom-left as the origin (0, 0)
                if (!FreeImage_GetPixelColor(in_bitmap, c, this->res_height - r - 1, &color))
                {
                    std::cout << "Error: FreeImage_GetPixelColor(" << c << ", " << r << " ) failed" << std::endl;
                    exit(1);
                }

                //std::cout << "Setting composition pixel: (" << this->res_height - r - 1 << ", " << c << ")" << std::endl;

                // Set the pixel color in the composition
                if (!FreeImage_SetPixelColor(out_bitmap, c, this->res_height - r - 1, &color))
                {
                    std::cout << "Error: FreeImage_SetPixelColor(" << c << ", " << r << " ) failed" << std::endl;
                    exit(1);
                }

            } // col
        } // row
    }

    std::cout << "Saving " << this->output_filename << std::endl;
    if (!FreeImage_Save(fm, out_bitmap, this->output_filename.c_str(), 0))
    {
                        std::cout << "Error: FreeImage_Save(" << this->output_filename << ") failed" << std::endl;
                        exit(1);
    }

}
