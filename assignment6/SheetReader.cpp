#include "assignment6/SheetReader.h"

Rows SheetReader::ImportSheet(const std::string &sheet_URL, const std::string &sheet_PATH, bool use_cached_sheet)
{
    if (use_cached_sheet)
    {
        // make sure it's a .tsv file
        assert(sheet_PATH.substr(sheet.length() - 4) == ".tsv");
        return std::move(ParseTSV(sheet_PATH));
    }
    else
    {
        // make sure it's a URL
        assert(sheet_URL.substr(0, 5) == "https");
        DownloadSheet(sheet_URL, sheet_PATH);
        return std::move(ParseTSV(sheet_PATH));
    }
}

// Reads TSV line by line and extracts fields
Rows SheetReader::ParseTSV(const std::string &fname)
{
    Rows rows;

    std::ifstream file(fname);
    std::string line;

    std::string tab = "\t";

    while (std::getline(file, line))
    {
        rows.push_back(Rows::value_type());
        
        int tab_pos = -1; 
        int last_tab_pos = -1;

        do
        {
            last_tab_pos = tab_pos + 1;
            tab_pos = line.find(tab, tab_pos + 1);

            std::size_t start = last_tab_pos;
            std::size_t count = tab_pos - last_tab_pos;
            std::string field = line.substr(start, count);

            rows.back().push_back(field);
        }
        while (tab_pos != std::string::npos);

    }

    return std::move(rows);
}


void SheetReader::DownloadSheet(const std::string &sheet_URL, const std::string &sheet_PATH)
{
    // check for curl
    std::string cmd = "curl --version > /dev/null 2>&1";
    int retval = system(cmd.c_str());
    if (retval)
    {
        std::cerr << "Error: curl is missing, " + cmd + " returned non-zero exit code " 
            + std::to_string(retval)<< std::endl;
        exit(retval);
    }

    // download sheet
    cmd = "curl -sk " + sheet_URL + " > " + sheet_PATH;

    retval = system(cmd.c_str());

    if (retval)
    {
        std::cerr << "Error: " + cmd + " returned non-zero exit code " + std::to_string(retval) << std::endl;
        exit(retval);
    }
}
