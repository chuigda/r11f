#ifndef R11F_INTERNAL_H
#define R11F_INTERNAL_H

#ifndef WIN32
#   define R11F_EXPORT __attribute__((visibility("default")))
#   define R11F_INTERNAL __attribute__((visibility("hidden")))
#else
#   define R11F_EXPORT __declspec(dllexport)
#   define R11F_INTERNAL
#endif /* WIN32 */

#endif /* R11F_INTERNAL_H */
