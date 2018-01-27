#include <stdexcept>
#include "FileTreeBuffer.hpp"

FileTreeBuffer::FileTreeBuffer(PackageConnection *con) : m_con(con) {
    m_tmp = std::tmpfile();
}

FileTreeBuffer::~FileTreeBuffer() {
    std::fclose(m_tmp);
}

void FileTreeBuffer::readComplete() {
    int level = 0;
    constexpr size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    do {
        m_con->receiveBuffer(buffer, 1);
        if (std::fwrite(buffer, sizeof(char), 1, m_tmp) != sizeof(char)) {
            throw std::runtime_error("Could not write to tmp file");
        }
        switch (buffer[0]) {
            case 'F': {
                constexpr size_t FILE_STATIC_SIZE = 2 * sizeof(size_t) + sizeof(time_t);
                m_con->receiveBuffer(buffer, sizeof(size_t));
                size_t size = *((size_t *) buffer);
                if (size + FILE_STATIC_SIZE > BUF_SIZE) {
                    throw std::runtime_error("Filename too long");
                }
                m_con->receiveBuffer(buffer + sizeof(size_t), size + sizeof(size_t) + sizeof(time_t));
                if (std::fwrite(buffer, sizeof(char), FILE_STATIC_SIZE + size, m_tmp) != FILE_STATIC_SIZE + size) {
                    throw std::runtime_error("Could not write to tmp file");
                }
                break;
            }
            case 'D': {
                m_con->receiveBuffer(buffer, sizeof(size_t));
                size_t size = *((size_t *) buffer);
                if (size > BUF_SIZE) {
                    throw std::runtime_error("Filename too long");
                }
                m_con->receiveBuffer(buffer + sizeof(size_t), size);
                if (std::fwrite(buffer, sizeof(char), size + sizeof(size_t), m_tmp) != size + sizeof(size_t)) {
                    throw std::runtime_error("Could not write to tmp file");
                }
                level++;
                break;
            }
            case 'E':
                level--;
                break;
            default:
                throw std::runtime_error("Illegal file type");
        }
    } while (level > -1);
}

void FileTreeBuffer::persist(std::ofstream &out) {
    std::rewind(m_tmp);
    constexpr size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    size_t bytes_read;
    do {
        bytes_read = std::fread(buffer, sizeof(char), BUF_SIZE, m_tmp);
        out.write(buffer, bytes_read);
    } while (bytes_read == BUF_SIZE);
}