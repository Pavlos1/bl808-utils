#include "util.h"
#include <errno.h>
#include <string.h>

uint32_t _atol(char *in) {
    errno = 0;
    uint32_t res = strtol(in, NULL, 0);
    if (errno) {
        fprintf(stderr, "cannot parse number: %s, errno=%d\n", in, errno);
        exit(1);
    }
    return res;
}

int main(int argc, char **argv) {
    uint32_t offset, size = 0;
    bool second_bh = false;;
    char *outfile;
    FILE *fp;
    
    if (argc == 4) {
        offset = _atol(argv[1]);
        size = _atol(argv[2]);
        outfile = argv[3];
    } else if ((argc == 5) && (strncmp(argv[1], "-s", 2) == 0)) {
        second_bh = true;
        fw_header.magiccode = 0x50414642;
        fw_header.cpu_cfg[0].config_enable = 0;
        fw_header.cpu_cfg[1].config_enable = 1;
        
        offset = _atol(argv[2]);
        size = _atol(argv[3]);
        outfile = argv[4];
    } else {
        fprintf(stderr, "usage: %s [-s] <offset> <size> <outfile>\n", argv[0]);
        exit(1);
    }
    
    fw_header.basic_cfg.img_len_cnt = size;
    fw_header.basic_cfg.group_image_offset = offset;
    fw_header.cpu_cfg[second_bh ? 1 : 0].image_address_offset = offset;
    fill_crcs(&fw_header);

    fp = fopen(outfile, "w");
    if (!fp) {
        fprintf(stderr, "failed to open %s, errno=%d\n", outfile, errno);
        exit(1);
    }
    
    if (fwrite(&fw_header, sizeof(struct bootheader_t), 1, fp) != 1) {
        fprintf(stderr, "failed to write to file, errno=%d\n", errno);
        exit(1);
    }
    
    if (fclose(fp)) {
        fprintf(stderr, "failed to close output file\n");
        exit(1);
    }
    return 0;
}
