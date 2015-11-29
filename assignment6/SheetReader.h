#pragma once

#ifndef __SHEETREADER__
#define __SHEETREADER__

#include "common/core.h"
#include <string.h>
#include <fstream>
#include <sstream>

typedef std::vector<std::vector<std::string>> Rows;

class SheetReader
{
public:

    // Creates a sheetreader and imports the google sheet
    // either downloading it from a URL or opening it locally
    // ***only support tab-separated value (.tsv) sheets
    static Rows ImportSheet(const std::string &sheet_URL, const std::string &sheet_PATH, bool use_cached_sheet);

private:

    // Parses the tsv from the given file and returns the rows. Copied from
    // http://stackoverflow.com/questions/1075712/reading-delimited-files-in-c
    static Rows ParseTSV(const std::string &fname);

    // Downloads sheet from URL and returns the filename for the sheet
    // or nullptr if it could not download the sheet
    static void DownloadSheet(const std::string &sheet_URL, const std::string &sheet_PATH);
};

#endif
