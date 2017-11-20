#ifndef CLERR_HPP_
#define CLERR_HPP_

#include <CL/cl.h>

//#define IGNORE_CL_ERRORS

#ifndef IGNORE_CL_ERRORS
#define STR(x) STR2(x)
#define STR2(x) #x
#define HANDLE_CLERR(foo) exit_on_error("File " __FILE__ ", Line " STR(__LINE__) ": " #foo " ->", foo)

[[noreturn]]
void exit_clerr(const char *err_msg, cl_int err);

inline void exit_on_error(const char *err_msg, cl_int err)
{
    if (err != CL_SUCCESS) {
        exit_clerr(err_msg, err);
    }
}

#else
#define HANDLE_CLERR(foo) (void)foo
#endif // HANDLE_CL_ERRORS



#endif // CLERR_HPP_
