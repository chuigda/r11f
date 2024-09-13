#include "error.h"

#include <locale.h>
#include <stdbool.h>
#include <string.h>

static const char* g_error_strings_zh_cn[] = {
    [R11F_success] = "操作成功完成",
    [R11F_ERR_out_of_memory] = "内存不足",
    [R11F_ERR_malformed_classfile] = ".class 文件格式错误",
    [R11F_ERR_duplicate_class] = "重复的类",
    [R11F_ERR_class_not_found] = "未找到类",
    [R11F_ERR_method_not_found] = "未找到方法",
    [R11F_ERR_cannot_invoke_abstract_method] = "不能调用抽象方法",
    [R11F_ERR_cannot_invoke_native_method] = "不能调用本地方法",
    [R11F_ERR_cannot_invoke_non_static_method] = "不能调用非静态方法"
};

static const char* g_error_strings_en_us[] = {
    [R11F_success] = "operation completed successfully",
    [R11F_ERR_out_of_memory] = "out of memory",
    [R11F_ERR_malformed_classfile] = "malformed .class file",
    [R11F_ERR_duplicate_class] = "duplicate class",
    [R11F_ERR_class_not_found] = "class not found",
    [R11F_ERR_method_not_found] = "method not found",
    [R11F_ERR_cannot_invoke_abstract_method] = "cannot invoke abstract method",
    [R11F_ERR_cannot_invoke_native_method] = "cannot invoke native method",
    [R11F_ERR_cannot_invoke_non_static_method] = "cannot invoke non-static method"
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
