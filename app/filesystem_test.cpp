#include <iostream>
#include <sstream>
#include <ftree/DirectoryScanner.hpp>
#include <fstream>

#include "network/BufferedOutputStream.hpp"
#include "ftree/FileTreeBuffer.hpp"

int main(int argc, char** argv) {
    SSHConnection ssh("/usr/bin/ssh", "sibbo", "localhost", "/home/sibbo/git/slowsync/cmake-build-debug/server");
    PackageConnection con(&ssh);
    BufferedOutputStream out(&con);
    bool connected = con.connect();
    std::cout << "Connection " << (connected ? "established" : "FAILED") << std::endl;

    std::vector<char> requestFileList;
    requestFileList.assign(1, 'S');
    std::string path("/home/sibbo/git/slowsync");
    size_t size = path.size();
    requestFileList.insert(requestFileList.end(), (char*) &size, (char*) ((&size) + 1));
    requestFileList.insert(requestFileList.end(), path.data(), path.data() + path.size());
    con.send(requestFileList);
    FileTreeBuffer ftb(&con);
    ftb.readComplete();

    std::ofstream persistentCopy("tree.bin");
    ftb.persist(persistentCopy);

    return 0;
}