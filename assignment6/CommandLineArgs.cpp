#include "assignment6/CommandLineArgs.h"

// static initializer, cannot do inline in .h because "must be initialzed out of line" error
// http://stackoverflow.com/questions/1563897/c-static-constant-string-class-member
const std::string CommandLineArgs::DEFAULT_OUTPUT_FILENAME_ROOT = "output";

CommandLineArgs::CommandLineArgs(int argc, char **argv)
{

    // Parse command line args
    int i = 1;
    while (i < argc)
    {
        if (strncmp(argv[i], "-o", 3) == 0)
        {
            if (i + 1 == argc)
            {
                MissingArgAfter("-o", "output file name");
            }
            this->output_filename_root = argv[++i];
        } // -o ooo
        else if (strncmp(argv[i], "-r", 3) == 0)
        {
            if (i != 1)
            {
                std::cerr << "Error: -r rrr must be the first and only arguments" << std::endl;
                PrintUsage();
                exit(1);
            }
            else
            {
                // -r rrr are the first and only arguments
                if (i + 1 == argc)
                {
                    MissingArgAfter("-r", "chunk name format");
                }
                this->input_filename_root = argv[++i];
            }
        } // -r rrr
        else if (strncmp(argv[i], "-t", 3) == 0)
        {
            if (i + 1 == argc)
            {
                MissingArgAfter("-t", "total number of chunks");
            }
            this->total_chunks = std::stoi(argv[++i]);
        }
        else if (strncmp(argv[i], "-c", 3) == 0)
        {
            if (i + 1 == argc)
            {
                MissingArgAfter("-c", "current chunk");
            }
            this->current_chunk = std::stoi(argv[++i]);
        }
        else if (strncmp(argv[i], "-h", 3) == 0)
        {
            if (i + 1 == argc)
            {
                MissingArgAfter("-h", "resolution height");
            }
            this->res_height = std::stoi(argv[++i]);
        }
        else if (strncmp(argv[i], "-w", 3) == 0)
        {
            if (i + 1 == argc)
            {
                MissingArgAfter("-w", "resolution width");
            }
            this->res_width = std::stoi(argv[++i]);
        }
        else
        {
            std::cerr << "Error: Unidentified argument " << argv[i] << std::endl;
            PrintUsage();
            exit(1);
        }

        i++;
    }

    ProcessArgs();

}

void CommandLineArgs::ProcessArgs()
{

    // If we are processing in chunks, make sure that if the user set the total chunk or the current chunk
    // that they also set the other one
    if ( (this->total_chunks == -1 && this->current_chunk != -1) ||
         (this->total_chunks != -1 && this->current_chunk == -1) )
    {
        std::cerr << "Error: setting one of -t total_chunks or -c current_chunk" 
            "requires the other to be set too." << std::endl; 
        PrintUsage();
        exit(1);
    }

    // If the user did not set an output file name, then set it to the default
    if (this->output_filename_root.empty())
    {
        this->output_filename_root = DEFAULT_OUTPUT_FILENAME_ROOT;
    }

    // If the resolution width or height was not set on the command line
    // then set it to the defaults

    if (this->res_width == -1)
    {
        this->res_width = DEFAULT_RESOLUTION_WIDTH;
    }
    
    if (this->res_height == -1)
    {
        this->res_height = DEFAULT_RESOLUTION_HEIGHT;
    }

    // If the user requested chunking, make sure that the resolution width and height are divisible
    // by the total number of chunks
    
    if (this->total_chunks != -1)
    {
        if ( (this->res_width  % this->total_chunks != 0) ||
             (this->res_height % this->total_chunks != 0) )
        {
            std::cerr << "Error: width " << this->res_width << " and height " << this->res_height <<
                "must be divisible by the total number of chunks " << this->total_chunks << std::endl;
            PrintUsage();
            exit(1);
        }
    }

}

void CommandLineArgs::PrintUsage()
{
    std::cerr << "Usages:\n" 
                 "\tcs148raytracer               Render the full scene\n" 
                 "\tcs148raytracer -t ttt -c ccc Render the ccc/ttt chunk of the scene\n"
                 "\tcs148raytracer -h hhh        Set the resolution height\n" 
                 "\tcs148raytracer -w www        Set the resolution width\n" 
                 "\tcs148raytracer -r rrr        Compose chunks named like rrr\n"
                 "\tcs148raytracer -o ooo        Name for output files, if used with -t ttt -c ccc\n"
                 "\t                             the filename will be ooo_ttt_ccc.png\n"; 
}

void CommandLineArgs::MissingArgAfter(std::string flag, std::string arg_after_flag)
{
    std::cerr << "Error: " << arg_after_flag << " must come after " << flag << std::endl;
    PrintUsage();
    exit(1);
}
