#include <iostream>
#include <sstream>
#include <ftree/DirectoryScanner.hpp>
#include <fstream>

#include "network/BufferedOutputStream.hpp"
#include "ftree/FileTreeBuffer.hpp"
#include "ftree/Synchronizer.hpp"

int main(int argc, char** argv) {
    SSHConnection ssh("/usr/bin/ssh", "bash", "localhost", "/home/bash/git/slowsync/cmake-build-debug/server");
    PackageConnection con(&ssh);
    BufferedOutputStream out(&con);
    bool connected = con.connect();
    std::cout << "Connection " << (connected ? "established" : "FAILED") << std::endl;

    std::vector<char> requestFileList;
    requestFileList.assign(1, 'S');
    std::string path("/home/bash/git/slowsync");
    size_t size = path.size();
    requestFileList.insert(requestFileList.end(), (char*) &size, (char*) ((&size) + 1));
    requestFileList.insert(requestFileList.end(), path.data(), path.data() + path.size());
    con.send(requestFileList);
    FileTreeBuffer ftb(&con);
    ftb.readComplete();

    std::ofstream persistentCopy("tree.bin");
    ftb.persist(persistentCopy);

    for (const auto& file : ftb) {
        std::cout << file.m_path.string() << std::endl;
    }

    Synchronizer synchronizer("/home/bash/slowsynctest", "/home/bash/git/slowsync", &std::cout);
    const auto to_download = synchronizer.synchronize_locally(ftb);

    for (const auto& path : to_download) {
        std::cout << "To download: " << path.string() << std::endl;
    }

    return 0;
}