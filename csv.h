#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

namespace bht {

/**
 * Class for processing CSV formatted files
 */
class CSVReader {
  private:
    /// @brief Input file path
    std::string path;

    /// @brief Extracted headers from the CSV file
    std::unordered_map<std::string, size_t> headers;

    /// @brief Current line fetched from the input file
    std::vector<std::string> fields;

    /// @brief Input file stream to fetch data from
    std::ifstream ifs;

    /**
     * Read the next line from the input file
     */
    std::vector<std::string> parseLine();

  public:
    /**
     * Create a new reader for the given file
     */
    CSVReader(std::string path);
    ~CSVReader();

    /**
     * reset the position of the reader back to the first line 
     * of content from the input file
     */
    void reset();

    /**
     * Move the read pointer to the next line
     */
    bool next();

    /**
     * Check if there is more data in the file to process
     */
    bool hasNext();

    /**
     * Return the data for the given field in the current line
     * @param name Name of the field to get the data for
     * @return Parsed content for the field or an empty string if the field is not found
     */
    std::string getField(std::string name);

    /**
     * Return the data for the given field in the current line
     * @param name Name of the field to get the data for
     * @param defaultValue Default value to return if this field is not found or empty
     * @return Parsed content for the field or defaultValue if the field is not found or empty
     */
    std::string getField(std::string name, std::string defaultValue);
};

}