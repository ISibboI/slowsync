#include "util.hpp"

void serializeString(std::ostringstream *out, const std::string &str) {
    serializeSize(out, str.size());
    out->write(str.c_str(), str.size() * sizeof(char));
}

std::string deserializeString(std::istringstream *in) {
    size_t size = deserializeSize(in);
    std::string result;
    result.resize(size);
    in->read(&result.front(), size * sizeof(char));
    return result;
}

void serializeSize(std::ostringstream* out, size_t size) {
    out->write((const char*) &size, sizeof(size_t));
}

size_t deserializeSize(std::istringstream* in) {
    size_t size;
    in->read((char*) &size, sizeof(size_t));
    return size;
}