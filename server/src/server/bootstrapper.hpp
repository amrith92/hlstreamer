#ifndef BOOTSTRAPPER_HPP
#define BOOTSTRAPPER_HPP

#include <string>
#include "exceptions.hpp"

using namespace ::hlserver::exceptions;

namespace hlserver {

class Bootstrapper
{
public:
    explicit Bootstrapper(const std::string &media_path);
    
    Bootstrapper(const Bootstrapper&) = delete;
    Bootstrapper &operator = (const Bootstrapper&) = delete;

    /// Add throw() decls
    void setup() throw (DirectoryInsufficientPrivilegesError, DirectoryNotFoundError, DirectoryNotWritable, DirectoryNotReadable, DirectoryNotReadableByGroup);

protected:
    const std::string &media_path_;
};

} // namespace hlserver

#endif /* BOOTSTRAPPER_HPP */
