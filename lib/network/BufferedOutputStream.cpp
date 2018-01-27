#include "BufferedOutputStream.hpp"

BufferedOutputStream::BufferedOutputStream(PackageConnection *connection) : m_con(connection) {}

void BufferedOutputStream::write(const char *buf, size_t size) {
    const char* limit = buf + size;

    while (buf < limit) {
        size_t writeSize = std::min((size_t) (limit - buf), BUFFER_SIZE - m_buffer.size());
        m_buffer.insert(m_buffer.end(), buf, buf + writeSize);
        buf += writeSize;
        if (m_buffer.size() == BUFFER_SIZE) {
            flush();
        }
    }
}

void BufferedOutputStream::flush() {
    m_con->sendBuffer(m_buffer.data(), m_buffer.size());
    m_buffer.clear();
}