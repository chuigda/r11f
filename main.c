#include "classfile.h"

#include <string.h>

int main() {
    r11f_classfile_t classfile;
    memset(&classfile, 0, sizeof(r11f_classfile_t));

    FILE *fp = fopen("Main.class", "rb");
    if (!fp) {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    if (!r11f_classfile_read(fp, &classfile)) {
        fprintf(stderr, "failed to read classfile\n");
        return 1;
    }

    r11f_classfile_dump("Main.class", &classfile);
    r11f_classfile_cleanup(&classfile);
}
