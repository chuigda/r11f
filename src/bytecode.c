#include "bytecode.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "byteutil.h"
#include "cfdump.h"
#include "class.h"
#include "class/attrib.h"
#include "forward.h"

static char const *g_indent_str = "                                        ";

static size_t r11f_disassemble_wide(uint8_t *code, size_t idx) {
    uint8_t opcode = code[idx + 1];
    char const* bytecode_str = r11f_explain_bytecode(opcode);
    switch (opcode) {
        case R11F_iload:
        case R11F_aload:
        case R11F_fload:
        case R11F_lload:
        case R11F_dload:
        case R11F_istore:
        case R11F_astore:
        case R11F_fstore:
        case R11F_lstore:
        case R11F_dstore:
        case R11F_ret: {
            uint8_t indexbyte1 = code[idx + 2];
            uint8_t indexbyte2 = code[idx + 3];
            uint16_t index = ((uint16_t)indexbyte1 << 8) | indexbyte2;
            printf("wide %s %d\n", bytecode_str, index);
            return 4;
        }
        case R11F_iinc: {
            uint8_t indexbyte1 = code[idx + 2];
            uint8_t indexbyte2 = code[idx + 3];
            uint16_t index = ((uint16_t)indexbyte1 << 8) | indexbyte2;
            uint8_t constbyte1 = code[idx + 4];
            uint8_t constbyte2 = code[idx + 5];
            uint16_t constant = ((uint16_t)constbyte1 << 8) | constbyte2;
            printf("wide %s %d %d\n", bytecode_str, index, constant);
            return 6;
        }
        default:
            printf("(inv) wide %s\n", bytecode_str);
            return 2;
    }
}

R11F_EXPORT char const* r11f_explain_bytecode(uint8_t bytecode) {
    switch (bytecode) {
#define BYTECODE(CODE,VALUE) case R11F_##CODE: return #CODE;
#include "bcodeinc.h"

    default:
        assert(false && "unknown bytecode");
        return "unknown";
    }
}

R11F_EXPORT void r11f_disassemble(r11f_class_t *clazz,
                                  r11f_method_info_t *method_info,
                                  size_t indent) {
    if (method_info->access_flags & R11F_ACC_NATIVE) {
        printf("%.*s%s\n", (int)indent, g_indent_str, "<native>");
        return;
    }

    if (method_info->access_flags & R11F_ACC_ABSTRACT) {
        printf("%.*s%s\n", (int)indent, g_indent_str, "<abstract>");
        return;
    }

    r11f_attribute_info_t *code_info =
        r11f_method_find_attribute(clazz, method_info, "Code");
    if (!code_info) {
        printf("%.*s%s\n", (int)indent, g_indent_str, "<not found>");
        return;
    }

    uint8_t *code = code_info->info + 8;
    uint16_t code_length = read_unaligned2(code_info->info + 4);

    size_t idx = 0;
    while (idx < code_length) {
        uint8_t opcode = code[idx];
        char const* bytecode_str = r11f_explain_bytecode(opcode);

        printf("%.*s[%d]\t", (int)indent, g_indent_str, (int)idx);

        switch (opcode) {
            case R11F_wide: {
                idx += r11f_disassemble_wide(code, idx);
                break;
            }

            case R11F_aload:
            case R11F_astore:
            case R11F_bipush:
            case R11F_dload:
            case R11F_dstore:
            case R11F_fload:
            case R11F_fstore:
            case R11F_iload:
            case R11F_istore:
            case R11F_lload:
            case R11F_lstore:
            case R11F_ret: {
                uint8_t index = code[idx + 1];
                printf("%s %d\n", bytecode_str, index);
                idx += 2;
                break;
            }

            case R11F_ldc: {
                uint8_t index = code[index + 1];
                printf("%s #%d ", bytecode_str, index);
                r11f_dump_constant_pool_item(
                    stdout,
                    clazz->constant_pool[index]
                );
                putchar('\n');
                idx += 2;
                break;
            }

            case R11F_newarray: {
                uint8_t atype = code[idx + 1];
                char const* atype_str = "unknown";
                switch (atype) {
                    case 4: atype_str = "T_BOOLEAN"; break;
                    case 5: atype_str = "T_CHAR"; break;
                    case 6: atype_str = "T_FLOAT"; break;
                    case 7: atype_str = "T_DOUBLE"; break;
                    case 8: atype_str = "T_BYTE"; break;
                    case 9: atype_str = "T_SHORT"; break;
                    case 10: atype_str = "T_INT"; break;
                    case 11: atype_str = "T_LONG"; break;
                }
                printf("%s %s\n", bytecode_str, atype_str);
                idx += 2;
                break;
            }

            case R11F_iinc: {
                uint8_t index = code[idx + 1];
                uint8_t constant = code[idx + 2];
                printf("%s %d %d\n", bytecode_str, index, constant);
                idx += 3;
                break;
            }

            case R11F_sipush: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t uvalue = ((uint16_t)byte1 << 8) | byte2;
                int16_t value = *(int16_t*)&uvalue;
                printf("%s %d\n", bytecode_str, value);
                idx += 3;
                break;
            }

            case R11F_new:
            case R11F_anewarray:
            case R11F_instanceof:
            case R11F_ldc_w:
            case R11F_ldc2_w:
            case R11F_checkcast:
            case R11F_getfield:
            case R11F_getstatic:
            case R11F_putfield:
            case R11F_putstatic:
            case R11F_invokespecial:
            case R11F_invokestatic: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t index = ((uint16_t)byte1 << 8) | byte2;
                printf("%s #%d ", bytecode_str, index);
                r11f_dump_constant_pool_item(
                    stdout,
                    clazz->constant_pool[index]
                );
                putchar('\n');
                idx += 3;
                break;
            }

            case R11F_invokedynamic: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t index = ((uint16_t)byte1 << 8) | byte2;
                uint8_t byte3 = code[idx + 3];
                uint8_t byte4 = code[idx + 4];
                uint16_t zero = ((uint16_t)byte3 << 8) | byte4;
                printf("%s #%d %d\n", bytecode_str, index, zero);
                idx += 5;
                break;
            }

            case R11F_invokeinterface: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t index = ((uint16_t)byte1 << 8) | byte2;
                uint8_t count = code[idx + 3];
                uint8_t zero = code[idx + 4];
                printf("%s #%d %d %d\n", bytecode_str, index, count, zero);
                idx += 5;
                break;
            }

            case R11F_goto:
            case R11F_if_acmpeq:
            case R11F_if_acmpne:
            case R11F_if_icmpeq:
            case R11F_if_icmpne:
            case R11F_if_icmplt:
            case R11F_if_icmpge:
            case R11F_if_icmpgt:
            case R11F_if_icmple:
            case R11F_ifeq:
            case R11F_ifne:
            case R11F_iflt:
            case R11F_ifge:
            case R11F_ifgt:
            case R11F_ifle:
            case R11F_ifnonnull:
            case R11F_ifnull:
            case R11F_jsr: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t uoffset = ((uint16_t)byte1 << 8) | byte2;
                int16_t offset = *(int16_t*)&uoffset;
                printf("%s %d\n", bytecode_str, (int)(idx + offset));
                idx += 3;
                break;
            }

            case R11F_multianewarray: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint16_t index = ((uint16_t)byte1 << 8) | byte2;
                uint8_t dimensions = code[idx + 3];
                printf("%s #%d %d\n", bytecode_str, index, dimensions);
                idx += 4;
                break;
            }

            case R11F_goto_w:
            case R11F_jsr_w: {
                uint8_t byte1 = code[idx + 1];
                uint8_t byte2 = code[idx + 2];
                uint8_t byte3 = code[idx + 3];
                uint8_t byte4 = code[idx + 4];
                uint32_t uoffset =
                    ((uint32_t)byte1 << 24)
                    | ((uint32_t)byte2 << 16)
                    | ((uint32_t)byte3 << 8)
                    | byte4;
                int32_t offset = *(int32_t*)&uoffset;
                printf("%s %d\n", bytecode_str, (int)(idx + offset));
                idx += 5;
                break;
            }

            case R11F_lookupswitch: {
                size_t padding = (idx + 4) % 4;
                if (padding) {
                    padding = 4 - padding;
                }

                uint8_t defaultbyte1 = code[idx + padding];
                uint8_t defaultbyte2 = code[idx + padding + 1];
                uint8_t defaultbyte3 = code[idx + padding + 2];
                uint8_t defaultbyte4 = code[idx + padding + 3];
                uint32_t udefault =
                    ((uint32_t)defaultbyte1 << 24)
                    | ((uint32_t)defaultbyte2 << 16)
                    | ((uint32_t)defaultbyte3 << 8)
                    | defaultbyte4;
                int32_t default_offset = *(int32_t*)&udefault;

                uint8_t npairsbyte1 = code[idx + padding + 4];
                uint8_t npairsbyte2 = code[idx + padding + 5];
                uint8_t npairsbyte3 = code[idx + padding + 6];
                uint8_t npairsbyte4 = code[idx + padding + 7];
                uint32_t npairs =
                    ((uint32_t)npairsbyte1 << 24)
                    | ((uint32_t)npairsbyte2 << 16)
                    | ((uint32_t)npairsbyte3 << 8)
                    | npairsbyte4;

                printf("tableswitch\n");
                printf(
                    "%.*sdefault: %d\n",
                    (int)indent + 2,
                    g_indent_str,
                    (int)(idx + default_offset)
                );

                idx += padding + 8;
                for (uint32_t i = 0; i < npairs; i++) {
                    uint8_t byte1 = code[idx];
                    uint8_t byte2 = code[idx + 1];
                    uint8_t byte3 = code[idx + 2];
                    uint8_t byte4 = code[idx + 3];
                    uint32_t ukey =
                        ((uint32_t)byte1 << 24)
                        | ((uint32_t)byte2 << 16)
                        | ((uint32_t)byte3 << 8)
                        | byte4;
                    int32_t key = *(int32_t*)&ukey;

                    uint8_t offset1 = code[idx + 4];
                    uint8_t offset2 = code[idx + 5];
                    uint8_t offset3 = code[idx + 6];
                    uint8_t offset4 = code[idx + 7];
                    uint32_t uoffset =
                        ((uint32_t)offset1 << 24)
                        | ((uint32_t)offset2 << 16)
                        | ((uint32_t)offset3 << 8)
                        | offset4;
                    int32_t offset = *(int32_t*)&uoffset;

                    printf(
                        "%.*s%d: %d\n",
                        (int)indent + 2,
                        g_indent_str,
                        key,
                        (int)(idx + offset)
                    );

                    idx += 8;
                }

                break;
            }

            case R11F_tableswitch: {
                size_t padding = (idx + 4) % 4;
                if (padding) {
                    padding = 4 - padding;
                }

                uint8_t defaultbyte1 = code[idx + padding];
                uint8_t defaultbyte2 = code[idx + padding + 1];
                uint8_t defaultbyte3 = code[idx + padding + 2];
                uint8_t defaultbyte4 = code[idx + padding + 3];
                uint32_t udefault =
                    ((uint32_t)defaultbyte1 << 24)
                    | ((uint32_t)defaultbyte2 << 16)
                    | ((uint32_t)defaultbyte3 << 8)
                    | defaultbyte4;
                int32_t default_offset = *(int32_t*)&udefault;

                uint8_t lowbyte1 = code[idx + padding + 4];
                uint8_t lowbyte2 = code[idx + padding + 5];
                uint8_t lowbyte3 = code[idx + padding + 6];
                uint8_t lowbyte4 = code[idx + padding + 7];
                uint32_t ulow =
                    ((uint32_t)lowbyte1 << 24)
                    | ((uint32_t)lowbyte2 << 16)
                    | ((uint32_t)lowbyte3 << 8)
                    | lowbyte4;
                int32_t low = *(int32_t*)&ulow;

                uint8_t highbyte1 = code[idx + padding + 8];
                uint8_t highbyte2 = code[idx + padding + 9];
                uint8_t highbyte3 = code[idx + padding + 10];
                uint8_t highbyte4 = code[idx + padding + 11];
                uint32_t uhigh =
                    ((uint32_t)highbyte1 << 24)
                    | ((uint32_t)highbyte2 << 16)
                    | ((uint32_t)highbyte3 << 8)
                    | highbyte4;
                int32_t high = *(int32_t*)&uhigh;

                printf("tableswitch\n");
                printf(
                    "%.*sdefault: %d\n",
                    (int)indent + 2,
                    g_indent_str,
                    (int)(idx + default_offset)
                );
                printf(
                    "%.*slow: %d\n",
                    (int)indent + 2,
                    g_indent_str,
                    low
                );
                printf(
                    "%.*shigh: %d\n",
                    (int)indent + 2,
                    g_indent_str,
                    high
                );

                idx += padding + 12;
                for (int32_t i = low; i <= high; i++) {
                    uint8_t offset1 = code[idx];
                    uint8_t offset2 = code[idx + 1];
                    uint8_t offset3 = code[idx + 2];
                    uint8_t offset4 = code[idx + 3];
                    uint32_t uoffset =
                        ((uint32_t)offset1 << 24)
                        | ((uint32_t)offset2 << 16)
                        | ((uint32_t)offset3 << 8)
                        | offset4;
                    int32_t offset = *(int32_t*)&uoffset;

                    printf(
                        "%.*s%d: %d\n",
                        (int)indent + 2,
                        g_indent_str,
                        i,
                        (int)(idx + offset)
                    );

                    idx += 4;
                }

                break;
            }

            default: {
                printf("%s\n", bytecode_str);
                idx += 1;
                break;
            }
        }
    }
}
