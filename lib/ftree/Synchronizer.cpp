#include "Synchronizer.hpp"

Synchronizer::Synchronizer(const boost::filesystem::path &new_increment, const boost::filesystem::path &last_increment,
                           std::ostream *log) :
        m_new_increment(new_increment),
        m_last_increment(last_increment),
        m_log(log) {}

std::vector<boost::filesystem::path> Synchronizer::synchronize_locally(FileTreeBuffer &file_tree) {
    *m_log << "Synchronizing locally..." << std::endl;
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
        const auto size = boost::filesystem::file_size(path);
        if (size != file.m_size) {
            to_download.push_back(file.m_path);
            continue;
        }
        boost::filesystem::path new_path = m_new_increment;
        new_path /= file.m_path;
        if (boost::filesystem::is_regular_file(new_path)) {
            if (boost::filesystem::file_size(new_path) == file.m_size
                && boost::filesystem::last_write_time(new_path) == file.m_last_modified) {
                continue;
            } else {
                // TODO This needs to change for download continuation
                boost::filesystem::remove(new_path);
            }
        }


        // Create hardlink
        boost::filesystem::create_directories(new_path.parent_path());
        boost::filesystem::create_hard_link(path, new_path);
    }

    *m_log << "Synchronized locally" << std::endl;
    return to_download;
}