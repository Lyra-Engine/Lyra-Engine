#ifndef LYRA_LIBRARY_COMMON_MSGBOX_H
#define LYRA_LIBRARY_COMMON_MSGBOX_H

#include <boxer/boxer.h>

#include <Common/String.h>

namespace lyra
{
    inline void show_error(const CString& title, const CString& message)
    {
        boxer::show(message.c_str(), title.c_str(), boxer::Style::Error);
    }

    inline void show_warning(const CString& title, const CString& message)
    {
        boxer::show(message.c_str(), title.c_str(), boxer::Style::Warning);
    }

    inline void show_message(const CString& title, const CString& message)
    {
        boxer::show(message.c_str(), title.c_str(), boxer::Style::Info);
    }

    inline boxer::Selection show_confirm(const CString& title, const CString& message)
    {
        return boxer::show(title.c_str(), message.c_str(), boxer::Style::Question, boxer::Buttons::YesNo);
    }

} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_MSGBOX_H
