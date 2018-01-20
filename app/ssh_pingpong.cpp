#include "network/SSHConnection.hpp"

#include <iostream>

int main(int argc, char** argv) {
    SSHConnection ssh("/usr/bin/ssh", "somni", "isibboi.de", "/usr/bin/date");
    std::cout << "Connection " << (ssh.connect() ? "works" : "FAILED") << std::endl;

    char buf[1024];
    ssize_t count = ssh.receive(buf, 1023);

    if (count == -1) {
        std::cout << "Broken pipe" << std::endl;
        count = 0;
    }

    buf[count] = 0;

    std::cout << "Received: '" << buf << "'" << std::endl;

    return 0;
}