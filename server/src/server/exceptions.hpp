#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace hlserver { namespace exceptions {

class DirectoryError : public std::runtime_error
{
public:
    DirectoryError(const std::string& path)
        : std::runtime_error(""), path_(path) {}

    virtual const char* what() const noexcept { return message_.c_str(); }

    std::string& getPath() { return path_; }
protected:
    std::string path_;
    std::string message_;
};

class DirectoryInsufficientPrivilegesError : public DirectoryError
{
public:
    DirectoryInsufficientPrivilegesError(const std::string& path)
        : DirectoryError(path)
    {
        message_ = "Insufficient privileges to access directory: " + path;
    }
};

class DirectoryNotFoundError : public DirectoryError
{
public:
    DirectoryNotFoundError(const std::string& path)
        : DirectoryError(path)
    {
        message_ = "Unable to find directory at this path: " + path;
    }
};

class DirectoryNotWritable : public DirectoryError
{
public:
    DirectoryNotWritable(const std::string& path)
        : DirectoryError(path)
    {
        message_ = "Directory not writable by me: " + path;
    }
};

class DirectoryNotWritableByGroup : public DirectoryNotWritable
{
public:
    DirectoryNotWritableByGroup(const std::string& path)
        : DirectoryNotWritable(path)
    {
        message_ = "Directory not writable by group: " + path +
            "\nTo fix it, apply WRITE permission for group to directory";
    }
};

class DirectoryNotWritableByOthers : public DirectoryNotWritable
{
public:
    DirectoryNotWritableByOthers(const std::string& path)
        : DirectoryNotWritable(path)
    {
        message_ = "Directory not writable by others: " + path +
            "\nTo fix it, apply WRITE permission for others to directory";
    }
};

class DirectoryNotReadable : public DirectoryError
{
public:
    DirectoryNotReadable(const std::string& path)
        : DirectoryError(path)
    {
        message_ = "Directory not readable by me: " + path;
    }
};

class DirectoryNotReadableByGroup : public DirectoryNotReadable
{
public:
    DirectoryNotReadableByGroup(const std::string& path)
        : DirectoryNotReadable(path)
    {
        message_ = "Directory not readable by group: " + path +
            "\nTo fix it, apply READ permission for group to directory";
    }
};

class DirectoryNotReadableByOthers : public DirectoryNotReadable
{
public:
    DirectoryNotReadableByOthers(const std::string& path)
        : DirectoryNotReadable(path)
    {
        message_ = "Directory not readable by others: " + path +
            "\nTo fix it, apply READ permission for others to directory";
    }
};

} /* namespace exceptions */ } // namespace hlserver

#endif /* EXCEPTIONS_HPP */
