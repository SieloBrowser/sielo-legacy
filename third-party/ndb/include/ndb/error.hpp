#ifndef ERROR_H_NDB
#define ERROR_H_NDB

#include <exception>
#include <stdexcept>

#if defined(__GNUG__)
    #define NDB_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define NDB_FUNCTION_NAME __FUNCTION__
#endif

#define ndb_error(M) throw std::logic_error(M + std::string("\nfrom ") + std::string(NDB_FUNCTION_NAME));

#endif // ERROR_H_NDB
