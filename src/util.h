
#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <stdio.h>
#include <string>

#define UNUSED  __attribute__((__unused__))
#define NO_FLAG     0


#define JAERROR_FMT(_fmt, _arg1, _args...)  \
    fprintf(stderr, "err: " _fmt "\n", _arg1, ##_args)

#define JAERROR(_str) \
    JAERROR_FMT("%s", _str)

#define JAERROR_OBJ(_obj) \
    print_error(_obj)

#ifdef JADEBUG
#define JADBG_FMT(_fmt, _arg1, _args...) \
    printf("dbg: [%s:%d] " _fmt "\n", __FILE__, __LINE__, _arg1, ##_args)

#else // ifdef JADEBUG

#define JADBG_FMT(_fmt, _arg1, _args...)    (void)0
#endif // ifdef JADEBUG

#define JADBG(_str) \
    JADBG_FMT("%s", _str)


namespace jahn
{

enum ErrorCodes {
    ERR_NOERR = 0,
    ERR_GEN,
    ERR_NOJOB,
    ERR_NO_RESOURCE,
    ERR_NOT_EXIST,

    ERR_INVALIDARGS,
    ERR_LOWMEM,
    ERR_INSUFFICIENT_BUFFER,
    ERR_NOT_READY,
};

typedef int       err_t;

struct Error
{
    Error() : msg(), code() {}
    std::string msg;
    int code;
};

static inline void print_error(const Error& obj)
{
    JAERROR_FMT("%s with error code: %d", obj.msg.c_str(), obj.code);
}

} //namespce jahn



#endif /* SRC_UTIL_H_ */
