
#pragma once

#include <thread>

#ifdef __GLIBCXX__

namespace {

// https://bloglitb.blogspot.de/2011/12/access-to-private-members-safer.html

template <typename Tag, typename Tag::type M>
struct Rob
{
    friend typename Tag::type get(Tag) { return M; }
};

struct std_thread_id_M_thread_f
{
    typedef std::thread::native_handle_type std::thread::id::*type;
    friend type get(std_thread_id_M_thread_f);
};

template struct Rob<std_thread_id_M_thread_f, &std::thread::id::_M_thread>;

}  // namespace

namespace Generic::Extension::this_thread {

inline std::thread::native_handle_type native_handle()
{
    return std::this_thread::get_id().*get(std_thread_id_M_thread_f());
}

}  // namespace Generic::Extension::this_thread

#endif