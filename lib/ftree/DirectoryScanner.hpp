#ifndef SLOWSYNC_DIRECTORYSCANNER_HPP
#define SLOWSYNC_DIRECTORYSCANNER_HPP

#include <boost/filesystem.hpp>
#include <vector>
#include <iostream>

class DirectoryScanner {
public:
    DirectoryScanner(std::ostream* out, std::ostream* log);

    void scanDirectory(const boost::filesystem::path& base_path);

private:
    std::ostream* m_out;
    std::ostream* m_log;
    std::vector<char> m_buffer;

    void scanDirectoryRecursive(const boost::filesystem::path& base_path);
    void emitFile(const boost::filesystem::path& path);
    void startDirectory(const boost::filesystem::path& path);
    void endDirectory();
};


#endif //SLOWSYNC_DIRECTORYSCANNER_HPP
