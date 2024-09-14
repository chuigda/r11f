#ifndef R11F_BYTECODE_H
#define R11F_BYTECODE_H

#include <stddef.h>
#include <stdint.h>

#include "defs.h"
#include "forward.h"
#ifdef __cplusplus
extern "C" {
#endif

enum {
#define BYTECODE(CODE, VALUE) R11F_##CODE = VALUE,
#include "bcodeinc.h"
};

R11F_EXPORT char const* r11f_explain_bytecode(uint8_t bytecode);
R11F_EXPORT void r11f_disassemble(r11f_class_t *clazz,
                                  r11f_method_info_t *method_info,
                                  size_t indent);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* R11F_BYTECODE_H */
