#include "error.h"

#include <locale.h>
#include <stdbool.h>
#include <string.h>

static const char* g_error_strings_zh_cn[] = {
    [R11F_ERR_none] = "操作成功完成",
    [R11F_ERR_out_of_memory] = "内存不足",
    [R11F_ERR_malformed_classfile] = ".class 文件格式错误"
};

static const char* g_error_strings_en_us[] = {
    [R11F_ERR_none] = "operation completed successfully",
    [R11F_ERR_out_of_memory] = "out of memory",
    [R11F_ERR_malformed_classfile] = "malformed .class file"
};

char const* r11f_explain_error(r11f_error_t error) {
    char const* const* strings = g_error_strings_en_us;
    char const* ret = "unknown error";

    char const* lang = setlocale(LC_MESSAGES, NULL);
    if (lang) {
        if (!strcmp(lang, "zh_CN")) {
            strings = g_error_strings_zh_cn;
            ret = "未知错误";
        }
    }

    if (error < sizeof(g_error_strings_en_us) / sizeof(g_error_strings_en_us[0])
        && strings[error]) {
        ret = strings[error];
    }

    return ret;
}
