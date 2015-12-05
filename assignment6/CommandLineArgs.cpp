#include "assignment6/CommandLineArgs.h"
#include "assignment6/Utility.h"
#include <regex>

// static initializers, cannot do inline in .h because "must be initialzed out of line" error
// http://stackoverflow.com/questions/1563897/c-static-constant-string-class-member
const std::string CommandLineArgs::DEFAULT_OUTPUT_FILENAME_ROOT = "output";
bool CommandLineArgs::VERBOSE = false;
bool CommandLineArgs::DIAGNOSTICS = false;

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
        else if (strncmp(argv[i], "-v", 3) == 0)
        {
            CommandLineArgs::VERBOSE = true;
        }
        else if (strncmp(argv[i], "-d", 3) == 0)
        {
            CommandLineArgs::DIAGNOSTICS = true;
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

    if (!this->input_filename_root.empty())
    {
        // The user requested that we compose chunks, make sure that all the info we need
        // is present in the filename itself (this restricts chunk filenames to a specific fmt)

        // The following is an acceptable matching: output1600x900_100-16.png  
        static const std::regex input_filename_fmt("[a-zA-Z]+[0-9]+x[0-9]+_[0-9]+-[0-9]+.[a-zA-Z]+");
        //                                          output   1600  x900   _100   -16    .png
        if ( ! std::regex_match(this->input_filename_root, input_filename_fmt))
        {
            std::cerr << "Error: the chunk file name " << this->input_filename_root << " has been changed since writing"
                         " or was written incorrectly." << std::endl;
            PrintUsage();
            exit(1);
        }

    }

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

    if (this->output_filename_root.empty())
    {
        // The user did not set an output file name, set it to the default
        this->output_filename_root = DEFAULT_OUTPUT_FILENAME_ROOT;
    }
    else
    {
        // The user did set an output file name, enforce single word constraint
        std::regex alpha_word_regex("[a-zA-Z]+");
        if ( ! std::regex_match(this->output_filename_root, alpha_word_regex) )
        {
            std::cerr << "Error: the argument " << this->output_filename_root << " after -o must be "
                         "a single alpha word ([a-zA-Z]+)" << std::endl;
            PrintUsage();
            exit(1);
        }
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

    
    // If the user requested chunking
    if (this->ChunkingRequested())
    {
        // Check that total is a perfect square
        if ( ! Utility::IsPerfectSquare(this->total_chunks) )
        {
            std::cerr << "Error: total_chunks " << this->total_chunks << " is not a perfect square." << std::endl;
        }

        // check that res width and res height are divisible by square root of total num chunks
        int num = (int) sqrt(this->total_chunks);
        if ( (this->res_width  % num != 0) ||
             (this->res_height % num != 0) )
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
                 "\tcs148raytracer -v            Print verbose output\n" 
                 "\tcs148raytracer -d            Print diagnostics (timing + intersection counts)\n" 
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
