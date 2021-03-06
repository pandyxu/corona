#ifndef __corona_v8_util_h__
#define __corona_v8_util_h__

#include <string.h>
#include <stdarg.h>
#include <v8.h>
#include <v8/globals.h>
#include <v8/checks.h>
#include <v8/allocation.h>
#include <v8/utils.h>
#include <v8/list.h>
#include <v8/platform.h>

namespace v8 {
    namespace internal {
        extern v8::internal::Thread *main_thread;
        extern v8::internal::Thread *current_thread;
    }
}

// Helper macros for argument parsing

#define V8_ARG_EXISTS(args, index) \
    do { \
        if ((args).Length() < ((index) + 1)) { \
            return v8::ThrowException(v8::Exception::TypeError( \
                v8::String::New( \
                    "Missing required argument at index " #index \
                ) \
            )); \
        } \
    } while (0)

#define V8_ARG_TYPE(args, index, type) \
    do { \
        if (!(args)[(index)]->Is##type()) { \
            return v8::ThrowException(v8::Exception::TypeError( \
                v8::String::New( \
                    "Argument at index " #index " is not a " #type \
                ) \
            )); \
        } \
    } while (0)

#define V8_ARG_VALUE(lval, args, index, type) \
    do { \
        V8_ARG_EXISTS(args, index); \
        V8_ARG_TYPE(args, index, type); \
        (lval) = (args)[(index)]->type##Value(); \
    } while (0)

#define V8_ARG_VALUE_UTF8(lval, args, index) \
    V8_ARG_EXISTS(args, index); \
    V8_ARG_TYPE(args, index, String); \
    v8::String::Utf8Value lval##__((args)[(index)]->ToString()); \
    (lval) = *lval##__

#define V8_ARG_VALUE_FD(lval, args, index) \
    do { \
        V8_ARG_VALUE(lval, args, index, Int32); \
        if ((lval) < 0) { \
            return v8::ThrowException(v8::Exception::TypeError( \
                FormatString( \
                    "Argument at index %d not a valid file descriptor: %d", \
                        (index), (lval) \
                ) \
            )); \
        } \
    } while (0)

#define V8_ARG_VALUE_FUNCTION(lval, args, index) \
    do { \
        V8_ARG_EXISTS(args, index); \
        V8_ARG_TYPE(args, index, Function); \
        (lval) = v8::Local<v8::Function>::Cast((args)[(index)]); \
    } while (0)


// Misc helper functions for other V8 interactions

// Set a constant numerical property on the given target
#define SET_CONST(target, e) \
    (target)->Set( \
        v8::String::NewSymbol(#e), \
        v8::Integer::New(e), \
        (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete) \
    )

// Set a constant function property on the given target
#define SET_FUNC(target, name, func) \
    (target)->Set( \
        v8::String::NewSymbol(name), \
        v8::FunctionTemplate::New(func)->GetFunction(), \
        (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete) \
    )

// Create a namespace object of the given name inside a target
static inline v8::Handle<v8::Object>
CreateNamespace(v8::Handle<v8::Object> target, v8::Handle<v8::String> name) {
    v8::Local<v8::Object> o =
        (v8::FunctionTemplate::New())->GetFunction()->NewInstance();
    target->Set(
        name,
        o,
        (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete)
    );

    return o;
}

// Return a new V8 string, formatted as specified
static inline v8::Handle<v8::String>
FormatString(const char *fmt, ...) {
    static char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    return v8::String::New(buf);
}

#endif /* __corona_v8_util_h__ */
