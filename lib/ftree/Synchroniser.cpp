#include "Synchroniser.hpp"

Synchroniser::Synchroniser(const boost::filesystem::path &new_increment, const boost::filesystem::path &last_increment,
                           std::ostream *log) :
        m_new_increment(new_increment),
        m_last_increment(last_increment),
        m_log(log) {}

std::vector<boost::filesystem::path> Synchroniser::synchronize_locally(FileTreeBuffer &file_tree) {
    std::vector<boost::filesystem::path> to_download;

    for (const auto& file : file_tree) {
        boost::filesystem::path path = m_last_increment;
        path /= file.m_path;
        if (!boost::filesystem::is_regular_file(path)) {
            to_download.push_back(file.m_path);
            continue;
        }
        const auto last_modified = boost::filesystem::last_write_time(path);
        if (last_modified != file.m_last_modified) {
            to_download.push_back(file.m_path);
            continue;
        }

        // Create hardlink
        boost::filesystem::path new_path = m_new_increment;
        new_path /= file.m_path;
        boost::filesystem::create_hard_link(new_path, path);
    }

    return to_download;
}