#include "util.h"
#include <errno.h>

int main(int argc, char **argv) {
    struct bootheader_t bh = {0};

    if (argc < 2) {
        fprintf(stderr, "usage: %s <binary>\n", argv[0]);
        exit(1);
    }
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "cannot open file %s, errno=%d\n", argv[1], errno);
        exit(1);
    }
    
    if (fread(&bh, sizeof(struct bootheader_t), 1, fp) != 1) {
        fprintf(stderr, "failed to read file, errno=%d\n", errno);
        fclose(fp);
        exit(1);
    }
    
    fclose(fp);
    print_boot_header(&bh);
    (void) check_crcs(&bh);
    
    return 0;
}
