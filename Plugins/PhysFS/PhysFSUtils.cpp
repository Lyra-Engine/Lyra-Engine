#include "PhysFSUtils.h"

bool sort_mount_points(const PhysMountPoint& a, const PhysMountPoint& b)
{
    // sort based on priority
    if (a.priority != b.priority)
        return a.priority > b.priority;

    // longer vpath first as tiebreaker
    return a.vpath.size() > b.vpath.size();
}
