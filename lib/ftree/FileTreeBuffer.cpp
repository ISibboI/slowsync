#include <stdexcept>
#include <boost/filesystem.hpp>
#include <iostream>
#include "FileTreeBuffer.hpp"

File::File() :
        m_path("/placeholder"),
        m_size(0),
        m_last_modified(0) {}

FileTreeBuffer::FileTreeBuffer(PackageConnection *con) : m_con(con) {
    m_tmp = std::tmpfile();
}

FileTreeBuffer::~FileTreeBuffer() {
    std::fclose(m_tmp);
}

void FileTreeBuffer::readComplete() {
    int level = 0;
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
    char buffer[BUF_SIZE];
    size_t bytes_read;
    do {
        bytes_read = std::fread(buffer, sizeof(char), BUF_SIZE, m_tmp);
        out.write(buffer, bytes_read);
    } while (bytes_read == BUF_SIZE);
}

FileTreeBufferIt FileTreeBuffer::begin() {
    return {m_tmp, false};
}

FileTreeBufferIt FileTreeBuffer::end() {
    return {m_tmp, true};
}

FileTreeBufferIt::FileTreeBufferIt(std::FILE *tmp, bool is_end) :
        m_tmp(tmp),
        m_is_end(is_end) {
    std::rewind(m_tmp);
    if (!m_is_end) {
        operator++();
    }
}

FileTreeBufferIt& FileTreeBufferIt::operator++() {
    if (m_is_end) {
        throw std::runtime_error("Incrementing end iterator");
    }

    char buffer[FileTreeBuffer::BUF_SIZE];
    if (!std::fread(buffer, sizeof(char), 1, m_tmp)) {
        throw std::runtime_error("Could not read from tmp file");
    }
    switch (buffer[0]) {
        case 'F': {
            size_t name_length;
            std::fread((void*) &name_length, sizeof(size_t), 1, m_tmp);
            std::fread(buffer, sizeof(char), name_length, m_tmp);
            std::string name(buffer, name_length);
            m_file.m_path = m_file.m_path.parent_path();
            m_file.m_path /= name;
            std::fread((void*) &m_file.m_size, sizeof(size_t), 1, m_tmp);
            std::fread((void*) &m_file.m_last_modified, sizeof(time_t), 1, m_tmp);
            break;
        }
        case 'D': {
            size_t name_length;
            std::fread((void*) &name_length, sizeof(size_t), 1, m_tmp);
            std::fread(buffer, sizeof(char), name_length, m_tmp);
            std::string name(buffer, name_length);
            m_file.m_path = m_file.m_path.parent_path();
            m_file.m_path /= name;
            m_file.m_path /= "placeholder";
            return operator++();
        }
        case 'E': {
            m_file.m_path = m_file.m_path.parent_path();
            if (m_file.m_path == "/") {
                m_is_end = true;
                break;
            } else {
                return operator++();
            }
        }
        default:
            throw std::runtime_error("Illegal file type");
    }

    return *this;
}

const File& FileTreeBufferIt::operator*() {
    return m_file;
}

bool FileTreeBufferIt::operator!=(const FileTreeBufferIt &other) const {
    return m_is_end != other.m_is_end;
}