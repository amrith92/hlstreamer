#include "bootstrapper.hpp"
#include <boost/filesystem.hpp>

using boost::filesystem;

namespace hlserver {

Bootstrapper::Bootstrapper(const std::string &media_path)
    : media_path_(media_path)
{
}

void Bootstrapper::setup()
{
    path p(media_path_);

    try
    {
        if (!exists(p)) {
            if (false == create_directory(media_path_)) {
                
            }
        }
    }
    catch (const filesystem_error &ex)
    {
        // Log this
    }
}

} // namespace hlserver
