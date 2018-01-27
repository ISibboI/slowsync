#include <boost/filesystem.hpp>
#include <sstream>
#include "Directory.hpp"
#include "util.hpp"

Directory::Directory(const boost::filesystem::path& path) : Directory(path, nullptr) {}

Directory::Directory(const boost::filesystem::path& path, Directory* parent) :
        m_path(path),
        m_parent(parent) {
}

boost::filesystem::path Directory::resolve(const boost::filesystem::path& path) const {
    boost::filesystem::path result = m_path;
    result /= path;
    if (m_parent != nullptr) {
        result = m_parent->resolve(result);
    }
    return result;
}

boost::filesystem::path Directory::resolve() const {
    if (m_parent == nullptr) {
        return m_path;
    } else {
        return m_parent->resolve(m_path);
    }
}

Directory Directory::getAbsoluteDirectory() const {
    return Directory(getAbsolutePath());
}

boost::filesystem::path Directory::getAbsolutePath() const {
    return boost::filesystem::canonical(resolve());
}

std::string Directory::str() const {
    std::stringstream ss;
    ss << resolve().string();
    for (const auto& dir : m_directories) {
        ss << "\n" << dir.str();
    }
    for (const auto& file : m_files) {
        ss << "\n" << file.str();
    }
    return ss.str();
}

void Directory::scan() {
    boost::filesystem::directory_iterator limit;
    boost::filesystem::directory_iterator entry(getAbsolutePath());
    for (; entry != limit; ++entry) {
        if (boost::filesystem::is_regular_file(entry->status())) {
            m_files.emplace_back(entry->path().leaf(), this);
        } else if (boost::filesystem::is_directory(entry->status())) {
            m_directories.emplace_back(entry->path().leaf(), this);
        }
    }
    for (auto& dir : m_directories) {
        dir.scan();
    }
}

Directory& Directory::addDirectory(const boost::filesystem::path &path) {
    m_directories.emplace_back(path, this);
    return m_directories.back();
}

File& Directory::addFile(const boost::filesystem::path &name, size_t size) {
    m_files.emplace_back(name, size, this);
    return m_files.back();
}

void Directory::serialize(std::stringstream* out) const {
    serializeString(out, m_path.string());
    serializeSize(out, m_directories.size());
    for (const auto& dir : m_directories) {
        dir.serialize(out);
    }
    serializeSize(out, m_files.size());
    for (const auto& file : m_files) {
        file.serialize(out);
    }
}

Directory& Directory::deserialize(std::stringstream *in, Directory *parent) {
    std::string path = deserializeString(in);
    auto& dir = parent->addDirectory(path);
    size_t size = deserializeSize(in);
    for (size_t i = 0; i < size; i++) {
        deserialize(in, &dir);
    }
    size = deserializeSize(in);
    for (size_t i = 0; i < size; i++) {
        File::deserialize(in, &dir);
    }
    return dir;
}