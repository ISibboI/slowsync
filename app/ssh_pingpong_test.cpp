#include "network/SSHConnection.hpp"

#include <iostream>
#include <network/PackageConnection.hpp>

int main(int argc, char** argv) {
    SSHConnection ssh("/usr/bin/ssh", "bash", "localhost", "/home/bash/git/slowsync/cmake-build-debug/server");
    PackageConnection con(&ssh);
    bool connected = con.connect();
    std::cout << "Connection " << (connected ? "established" : "FAILED") << std::endl;

    return 0;
}