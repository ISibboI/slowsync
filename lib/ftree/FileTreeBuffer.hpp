#ifndef SLOWSYNC_FILETREEBUFFER_HPP
#define SLOWSYNC_FILETREEBUFFER_HPP

#include "network/PackageConnection.hpp"
#include <fstream>

class FileTreeBufferIt;

class File {
public:
    File();

    boost::filesystem::path m_path;
    size_t m_size;
    time_t m_last_modified;
};

class FileTreeBuffer {
public:
    explicit FileTreeBuffer(PackageConnection *con);
    ~FileTreeBuffer();

    void readComplete();
    void persist(std::ofstream &out);

    FileTreeBufferIt begin();
    FileTreeBufferIt end();

private:
    friend class FileTreeBufferIt;
    constexpr static size_t BUF_SIZE = 4096;
    std::FILE* m_tmp;
    PackageConnection* m_con;
};

class FileTreeBufferIt {
public:
    FileTreeBufferIt(std::FILE* tmp, bool is_end);

    FileTreeBufferIt& operator++();
    const File& operator*();
    bool operator!=(const FileTreeBufferIt& other) const;

private:
    std::FILE* m_tmp;
    bool m_is_end;
    File m_file;
};


#endif //SLOWSYNC_FILETREEBUFFER_HPP
