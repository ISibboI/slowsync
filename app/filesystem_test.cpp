#include <ftree/File.hpp>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    Directory local("..");
    Directory absolute = local.getAbsoluteDirectory();
    File f("CMakeLists.txt", &absolute);
    absolute.scan();

    std::cout << f.str() << std::endl;
    std::cout << f.getSize() << std::endl;
    std::cout << absolute.str() << std::endl;

    std::stringstream buffer;


    return 0;
}