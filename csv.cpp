#include "csv.h"
#include <string>
#include <iostream>
#include <fstream>

namespace bht {

CSVReader::CSVReader(std::string path) {
  // Try to open the input file
  this->path = path;
  ifs.open(path, std::ifstream::in);

  // Fetch headers
  if (ifs.good()) {
    headers.clear();
    std::vector<std::string> items = parseLine();
    for (size_t index = 0; index < items.size(); index++) {
      headers[items[index]] = index;
    }
    
    // Fetch the first line of the file
    next();
  }
}

CSVReader::~CSVReader() {
  ifs.close();
  headers.clear();
  fields.clear();
}

void CSVReader::reset() {
  ifs.seekg(0);

  // Skip the first line because it contains the header
  std::string line;
  std::getline(ifs, line);

  // Seek to next line
  next();
}

bool CSVReader::next() {
  // Clear input if no more lines are available
  if (hasNext() == false) {
    fields.clear();
    return false;
  }

  fields = parseLine();
  return true;
}

bool CSVReader::hasNext() {
  return ifs.good();
}

std::string CSVReader::getField(std::string name) {
  return getField(name, "");
}

std::string CSVReader::getField(std::string name, std::string defaultValue) {
  if (headers.count(name) > 0) {
    size_t index = headers.at(name);
    return index >= fields.size() || fields[index].empty() ? defaultValue : fields[index];
  }

  return defaultValue;
}

std::vector<std::string> CSVReader::parseLine() {
  // Fetch next line
  std::string line;
  std::getline(ifs, line);

  // Create result container
  std::vector<std::string> result;
  std::string chunk;
  char quoteCharacter = '"';
  char delimiter = ',';
  bool isQuoted = false;
  size_t begin = 0; 
  size_t n = 0;
  
  for (size_t index = 0; index < line.length(); index++) {
    char c = line.at(index);
    if (c == delimiter && isQuoted == false) {
      // New field begins
      result.push_back(line.substr(begin, n));
      begin = index + 1;
      n = 0;
    }
    else if (c == quoteCharacter) {
      isQuoted = !isQuoted;
      if (isQuoted) {
        begin++;
      }
    }
    else if (c != '\n' && c != '\r') {
      n++;
    }
  }

  // Consume any remaining characters
  if (n > 0) {
    result.push_back(line.substr(begin, n));
  }

  return result;
}

}