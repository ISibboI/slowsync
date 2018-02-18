#include "DirectoryScanner.hpp"

DirectoryScanner::DirectoryScanner(std::ostream *out, std::ostream* log) : m_out(out), m_log(log) {}

void DirectoryScanner::scanDirectory(const boost::filesystem::path &base_path) {
    scanDirectoryRecursive(base_path);
    endDirectory();
    m_out->flush();
}

void DirectoryScanner::scanDirectoryRecursive(const boost::filesystem::path &base_path) {
    const boost::filesystem::path canonical_path = boost::filesystem::canonical(base_path);
    boost::filesystem::directory_iterator limit;
    boost::filesystem::directory_iterator entry(canonical_path);
    for (; entry != limit; ++entry) {
        if ((entry->status().permissions() & 0004) != 0004) {
            // We can't read the file, so let's just skip it
            *m_log << "Skipping file without others read permissions: " << entry->path().string() << std::endl;
            continue;
        }
        if (boost::filesystem::is_regular_file(entry->status())) {
            emitFile(entry->path());
        } else if (boost::filesystem::is_directory(entry->status())) {
            startDirectory(entry->path());
            scanDirectoryRecursive(entry->path());
            endDirectory();
        }
    }
}

void DirectoryScanner::emitFile(const boost::filesystem::path &path) {
    *m_log << "Found file " << path << std::endl;
    const std::string leafName = path.leaf().string();
    const size_t size = leafName.size();
    m_buffer.assign(1, 'F');
    m_buffer.insert(m_buffer.end(), (const char*) &size, ((const char*) &size) + sizeof(size_t));
    m_buffer.insert(m_buffer.end(), leafName.data(),  leafName.data() + leafName.size());
    const size_t fileSize = boost::filesystem::file_size(path);
    m_buffer.insert(m_buffer.end(), (const char*) &fileSize, ((const char*) &fileSize) + sizeof(size_t));
    const time_t lastModified = boost::filesystem::last_write_time(path);
    m_buffer.insert(m_buffer.end(), (const char*) &lastModified, ((const char*) &lastModified) + sizeof(time_t));
    m_out->write(m_buffer.data(), m_buffer.size());
    m_buffer.clear();
}

void DirectoryScanner::startDirectory(const boost::filesystem::path &path) {
    *m_log << "Start directory " << path << std::endl;
    const std::string leafName = path.leaf().string();
    const size_t size = leafName.size();
    m_buffer.assign(1, 'D');
    m_buffer.insert(m_buffer.end(), (const char*) &size, ((const char*) &size) + sizeof(size_t));
    m_buffer.insert(m_buffer.end(), leafName.data(),  leafName.data() + leafName.size());
    m_out->write(m_buffer.data(), m_buffer.size());
    m_buffer.clear();
}

void DirectoryScanner::endDirectory() {
    *m_log << "End directory" << std::endl;
    m_buffer.assign(1, 'E');
    m_out->write(m_buffer.data(), m_buffer.size());
    m_buffer.clear();
}