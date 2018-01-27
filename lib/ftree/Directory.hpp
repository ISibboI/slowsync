#ifndef SLOWSYNC_DIRECTORY_HPP
#define SLOWSYNC_DIRECTORY_HPP

#include <vector>
#include "File.hpp"

class File;

class Directory {
public:
    explicit Directory(const boost::filesystem::path& path);
    Directory(const boost::filesystem::path& path, Directory* parent);
    ~Directory() = default;

    boost::filesystem::path resolve(const boost::filesystem::path& path) const;
    boost::filesystem::path resolve() const;
    Directory getAbsoluteDirectory() const;
    boost::filesystem::path getAbsolutePath() const;

    std::string str() const;

    void scan();
    Directory& addDirectory(const boost::filesystem::path& path);
    File& addFile(const boost::filesystem::path& name, size_t size);

    void serialize(std::stringstream* out) const;
    static Directory& deserialize(std::stringstream* in, Directory* parent);

private:
    const boost::filesystem::path m_path;
    const Directory* m_parent;
    std::vector<Directory> m_directories;
    std::vector<File> m_files;
};


#endif //SLOWSYNC_DIRECTORY_HPP
