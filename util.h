#ifndef __UTIL_H_
#define __UTIL_H_

#include "fw_header.h"
#include <stdlib.h>
#include <stdbool.h>

unsigned long crc(unsigned char *buf, int len);
void print_boot_header(struct bootheader_t *bh);
bool check_crcs(struct bootheader_t *bh);
void fill_crcs(struct bootheader_t *bh);

#endif
