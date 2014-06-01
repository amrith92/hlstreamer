#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <string>

namespace hlserver {

class Playlist
{
public:
    static void generate(const std::string &directory);
};

} // namespace hlserver

#endif
