#include <iostream>

int main(int argc, char** argv) {
    bool running = true;
    while (running) {
        int packageLength;
        std::cin.read((char*) &packageLength, sizeof(int));
        if (packageLength < 0) {
            std::cout << "Package length negative" << std::endl;
        }
        std::cin.ignore(packageLength);

        if (packageLength == 0) {
            std::cout.write((char*) &packageLength, sizeof(int));
        }
    }
}