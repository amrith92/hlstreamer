#include "bootstrapper.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace hlserver {

Bootstrapper::Bootstrapper(const std::string &media_path)
    : media_path_(media_path)
{
}

void Bootstrapper::setup() throw (DirectoryInsufficientPrivilegesError, DirectoryNotFoundError, DirectoryNotWritable, DirectoryNotReadable, DirectoryNotReadableByGroup)
{
    path p(media_path_);

    try
    {
        if (!exists(p)) {
            if (false == create_directory(media_path_)) {
                throw DirectoryNotWritable(media_path_);
            }
        }

        file_status s = status(p);
        if (!(s.permissions() & perms::owner_read)) {
            throw DirectoryNotReadable(media_path_);
        }

        if (!(s.permissions() & perms::owner_write)) {
            throw DirectoryNotWritable(media_path_);
        }


        if (!(s.permissions() & perms::group_write)) {
            throw DirectoryNotReadableByGroup(media_path_);
        }
    }
    catch (const filesystem_error &ex)
    {
        // Log this
        throw DirectoryInsufficientPrivilegesError(media_path_);
    }
}

} // namespace hlserver
