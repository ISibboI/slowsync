#include <iostream>
#include <string>
#include <ftree/DirectoryScanner.hpp>

class Command {
public:
    constexpr static char Scan = 'S';
    constexpr static char Exit = 'E';
};

int main(int argc, char** argv) {
    bool running = true;
    constexpr size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    std::ofstream log("server.log");

    while (running) {
        int packageLength;
        std::cin.read((char*) &packageLength, sizeof(int));
        if (packageLength < 0) {
            log << "Package length negative" << std::endl;
            return 1;
        }

        if (packageLength == 0) {
            log << "Received ping" << std::endl;
            std::cout.write((char*) &packageLength, sizeof(int));
            log << "Sent pong" << std::endl;
            continue;
        } else if (packageLength > BUF_SIZE) {
            log << "Package too large" << std::endl;
            return 1;
        }

        log << "Received package of length " << packageLength << std::endl;
        std::cin.read(buffer, packageLength);
        switch (buffer[0]) {
            case Command::Exit:
                return 0;
            case Command::Scan: {
                log << "Received scan command" << std::endl;
                size_t size = *((size_t*)(buffer + 1));
                if (size > BUF_SIZE - 1 - sizeof(size_t)) {
                    log << "Path too long" << std::endl;
                    return 1;
                }
                std::string path(buffer + 1 + sizeof(size_t), size);
                log << "Path is " << path << std::endl;
                DirectoryScanner scanner(&std::cout, &log);
                scanner.scanDirectory(path);
            }
            default:
                log << "Unknown command" << std::endl;
                return 1;
        }
    }
}