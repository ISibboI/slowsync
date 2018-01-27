#ifndef SLOWSYNC_FILE_HPP
#define SLOWSYNC_FILE_HPP

#include <boost/filesystem.hpp>
#include "Directory.hpp"

class Directory;

class File {
public:
    File(const boost::filesystem::path& name, Directory* parent);
    File(const boost::filesystem::path& name, size_t size, Directory* parent);
    ~File() = default;

    boost::filesystem::path resolve() const;
    size_t getSize() const;

    std::string str() const;

    void serialize(std::ostringstream* out) const;
    static File& deserialize(std::istringstream* in, Directory* parent);

private:
    const boost::filesystem::path m_name;
    const size_t m_size;
    const Directory* m_parent;
};


#endif //SLOWSYNC_FILE_HPP
