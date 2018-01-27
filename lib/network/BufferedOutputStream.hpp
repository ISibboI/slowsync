#ifndef SLOWSYNC_BUFFEREDOUTPUTSTREAM_HPP
#define SLOWSYNC_BUFFEREDOUTPUTSTREAM_HPP

#include <vector>
#include "PackageConnection.hpp"

class BufferedOutputStream {
public:
    explicit BufferedOutputStream(PackageConnection* connection);
    void write(const char* buf, size_t size);
    void flush();

private:
    PackageConnection* m_con;
    std::vector<char> m_buffer;
    constexpr static size_t BUFFER_SIZE = 4096;
};


#endif //SLOWSYNC_BUFFEREDOUTPUTSTREAM_HPP
