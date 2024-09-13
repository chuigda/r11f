#include "classfile.h"
#include "classfile_dump.h"

#include <string.h>

int main(int argc, char *argv[]) {
    if (argc >= 3 && !strcmp(argv[1], "--dump")) {
        for (int i = 2; i < argc; i++) {
            r11f_classfile_t classfile;
            memset(&classfile, 0, sizeof(r11f_classfile_t));

            FILE *fp = fopen(argv[i], "rb");
            if (!fp) {
                fprintf(stderr, "failed to open file\n");
                continue;
            }

            if (!r11f_classfile_read(fp, &classfile)) {
                fprintf(stderr, "failed to read classfile\n");
                r11f_classfile_cleanup(&classfile);
                fclose(fp);
            }

            r11f_classfile_dump(argv[i], &classfile);
            r11f_classfile_cleanup(&classfile);
            fclose(fp);
        }
    }
}
