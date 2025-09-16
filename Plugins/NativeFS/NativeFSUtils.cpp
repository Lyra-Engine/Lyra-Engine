#include <algorithm>
#include <Lyra/Common/Container.h>
#include "NativeFSUtils.h"

// strip a prefix from path if present. Returns true if stripped.
bool strip_prefix(String& path, const String& prefix)
{
    // path and prefix are normalized with '/'
    if (path.size() < prefix.size())
        return false;

    if (path.compare(0, prefix.size(), prefix) != 0)
        return false;

    // either exact match or next char is '/'
    if (path.size() > prefix.size() && path[prefix.size()] != '/')
        return false;

    // strip prefix and optional '/'
    if (path.size() == prefix.size()) {
        path = ""; // requesting the mount root
    } else {
        path.erase(0, prefix.size());
        if (!path.empty() && path.front() == '/')
            path.erase(0, 1);
    }
    return true;
}

// normalize virtual path: ensure leading '/', strip trailing '/'
String normalize_vpath(FSPath vpath)
{
    if (!vpath) return String("/");

    String s(vpath);
    if (s.empty()) return String("/");

    // replace backslashes with forward slashes to be consistent
    std::replace(s.begin(), s.end(), '\\', '/');
    if (s.front() != '/')
        s.insert(s.begin(), '/');

    // remove trailing '/'
    while (s.size() > 1 && s.back() == '/')
        s.pop_back();

    return s;
}

bool sort_mount_points(const NativeMount& a, const NativeMount& b)
{
    // sort based on priority
    if (a.priority != b.priority)
        return a.priority > b.priority;

    // longer vpath first as tiebreaker
    return a.vpath.size() > b.vpath.size();
}
