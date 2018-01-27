#ifndef SLOWSYNC_FILETREEBUFFER_HPP
#define SLOWSYNC_FILETREEBUFFER_HPP

#include "network/PackageConnection.hpp"
#include <fstream>

class FileTreeBuffer {
public:
    explicit FileTreeBuffer(PackageConnection *con);
    ~FileTreeBuffer();

    void readComplete();
    void persist(std::ofstream &out);

private:
    std::FILE* m_tmp;
    PackageConnection* m_con;
};


#endif //SLOWSYNC_FILETREEBUFFER_HPP
