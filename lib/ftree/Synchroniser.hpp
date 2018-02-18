#ifndef SLOWSYNC_SYNCHRONISER_HPP
#define SLOWSYNC_SYNCHRONISER_HPP

#include <boost/filesystem.hpp>
#include "FileTreeBuffer.hpp"

class Synchroniser {
public:
    Synchroniser(const boost::filesystem::path& new_increment, const boost::filesystem::path& last_increment, std::ostream* log);

    /** Hardlinks files from the last increment to the new increment if they didn't change.
     * \returns A list of files that need to be downloaded from the backup source
     */
    std::vector<boost::filesystem::path> synchronize_locally(FileTreeBuffer& file_tree);

private:
    boost::filesystem::path m_new_increment;
    boost::filesystem::path m_last_increment;
    std::ostream* m_log;
};


#endif //SLOWSYNC_SYNCHRONISER_HPP
