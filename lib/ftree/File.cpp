#include "File.hpp"
#include "util.hpp"

File::File(const boost::filesystem::path& name, Directory* parent) :
        File(name, boost::filesystem::file_size(resolve()), parent) {}

File::File(const boost::filesystem::path& name, size_t size, Directory* parent) :
        m_name(name),
        m_parent(parent),
        m_size(size) {}

boost::filesystem::path File::resolve() const {
    if (m_parent == nullptr) {
        return m_name;
    } else {
        return m_parent->resolve(m_name);
    }
}

size_t File::getSize() const {
    return boost::filesystem::file_size(m_parent->resolve(m_name));
}

std::string File::str() const {
    return resolve().string();
}

void File::serialize(std::stringstream* out) const {
    serializeString(out, m_name.string());
    serializeSize(out, m_size);
}

File& File::deserialize(std::stringstream *in, Directory *parent) {
    std::string name = deserializeString(in);
    size_t size = deserializeSize(in);
    return parent->addFile(name, size);
}