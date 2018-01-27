#ifndef SLOWSYNC_UTIL_HPP
#define SLOWSYNC_UTIL_HPP

#include <iostream>
#include <string>

void serializeString(std::ostringstream *out, const std::string &str);
std::string deserializeString(std::istringstream *in);

void serializeSize(std::ostringstream* out, size_t size);
size_t deserializeSize(std::istringstream* in);


#endif //SLOWSYNC_UTIL_HPP
