#include "util.h"

// https://www.rfc-editor.org/rfc/rfc1952#section-8

/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;

/* Make the table for a fast CRC. */
void make_crc_table(void) {
    unsigned long c;
    int n, k;
    for (n = 0; n < 256; n++) {
        c = (unsigned long) n;
        for (k = 0; k < 8; k++) {
            if (c & 1) {
                c = 0xedb88320L ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/*
 Update a running crc with the bytes buf[0..len-1] and return
the updated crc. The crc should be initialized to zero. Pre- and
post-conditioning (one's complement) is performed within this
function so it shouldn't be done by the caller. Usage example:

 unsigned long crc = 0L;

 while (read_buffer(buffer, length) != EOF) {
   crc = update_crc(crc, buffer, length);
 }
 if (crc != original_crc) error();
*/
unsigned long update_crc(unsigned long crc, unsigned char *buf, int len) {
    unsigned long c = crc ^ 0xffffffffL;
    int n;

    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c ^ 0xffffffffL;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc(unsigned char *buf, int len) {
    return update_crc(0L, buf, len);
}

bool check_crcs(struct bootheader_t *bh) {
    bool result = true;
    uint32_t actual_crc = 0;
    
    actual_crc = crc((unsigned char *) &bh->flash_cfg.cfg, sizeof(struct spi_flash_cfg_t));
    if (actual_crc != bh->flash_cfg.crc32) {
        printf("Flash CRC mismatch; reported=%x, actual = %x\n", bh->flash_cfg.crc32, actual_crc);
        result = false;
    }
    
    actual_crc = crc((unsigned char *) &bh->clk_cfg.cfg, sizeof(struct sys_clk_cfg_t));
    if (actual_crc != bh->clk_cfg.crc32) {
        printf("Clock CRC mismatch; reported=%x, actual = %x\n", bh->clk_cfg.crc32, actual_crc);
        result = false;
    }
    
    actual_crc = crc((unsigned char *) bh, sizeof(struct bootheader_t) - 4);
    if (actual_crc != bh->crc32) {
        printf("Bootheader CRC mismatch; reported=%x, actual = %x\n", bh->crc32, actual_crc);
        result = false;
    }
    
    if (result) printf("All CRCs match\n");
    return result;
}

void fill_crcs(struct bootheader_t *bh) {
    bh->flash_cfg.crc32 = crc((unsigned char *) &bh->flash_cfg.cfg, sizeof(struct spi_flash_cfg_t));
    bh->clk_cfg.crc32 = crc((unsigned char *) &bh->clk_cfg.cfg, sizeof(struct sys_clk_cfg_t));
    bh->crc32 = crc((unsigned char *) bh, sizeof(struct bootheader_t) - 4);
}

void print_boot_header(struct bootheader_t *bh) {
    printf("bh->magiccode = %x\n", bh->magiccode);
    printf("bh->rivision = %x\n", bh->rivison); // lmao
    
    printf("bh->flash_cfg.magiccode = %x\n", bh->flash_cfg.magiccode);
    
    printf("bh->flash_cfg.cfg.ioMode = %x\n", bh->flash_cfg.cfg.ioMode);
    printf("bh->flash_cfg.cfg.cReadSupport = %x\n", bh->flash_cfg.cfg.cReadSupport);
    printf("bh->flash_cfg.cfg.clkDelay = %x\n", bh->flash_cfg.cfg.clkDelay);
    printf("bh->flash_cfg.cfg.clkInvert = %x\n", bh->flash_cfg.cfg.clkInvert);
    printf("bh->flash_cfg.cfg.resetEnCmd = %x\n", bh->flash_cfg.cfg.resetEnCmd);
    printf("bh->flash_cfg.cfg.resetCmd = %x\n", bh->flash_cfg.cfg.resetCmd);
    printf("bh->flash_cfg.cfg.resetCreadCmd = %x\n", bh->flash_cfg.cfg.resetCreadCmd);
    printf("bh->flash_cfg.cfg.resetCreadCmdSize = %x\n", bh->flash_cfg.cfg.resetCreadCmdSize);
    printf("bh->flash_cfg.cfg.jedecIdCmd = %x\n", bh->flash_cfg.cfg.jedecIdCmd);
    printf("bh->flash_cfg.cfg.jedecIdCmdDmyClk = %x\n", bh->flash_cfg.cfg.jedecIdCmdDmyClk);
    printf("bh->flash_cfg.cfg.enter32BitsAddrCmd = %x\n", bh->flash_cfg.cfg.enter32BitsAddrCmd);
    printf("bh->flash_cfg.cfg.exit32BitsAddrCmd = %x\n", bh->flash_cfg.cfg.exit32BitsAddrCmd);
    printf("bh->flash_cfg.cfg.sectorSize = %x\n", bh->flash_cfg.cfg.sectorSize);
    printf("bh->flash_cfg.cfg.mid = %x\n", bh->flash_cfg.cfg.mid);
    printf("bh->flash_cfg.cfg.pageSize = %x\n", bh->flash_cfg.cfg.pageSize);
    printf("bh->flash_cfg.cfg.chipEraseCmd = %x\n", bh->flash_cfg.cfg.chipEraseCmd);
    printf("bh->flash_cfg.cfg.sectorEraseCmd = %x\n", bh->flash_cfg.cfg.sectorEraseCmd);
    printf("bh->flash_cfg.cfg.blk32EraseCmd = %x\n", bh->flash_cfg.cfg.blk32EraseCmd);
    printf("bh->flash_cfg.cfg.blk64EraseCmd = %x\n", bh->flash_cfg.cfg.blk64EraseCmd);
    printf("bh->flash_cfg.cfg.writeEnableCmd = %x\n", bh->flash_cfg.cfg.writeEnableCmd);
    printf("bh->flash_cfg.cfg.pageProgramCmd = %x\n", bh->flash_cfg.cfg.pageProgramCmd);
    printf("bh->flash_cfg.cfg.qpageProgramCmd = %x\n", bh->flash_cfg.cfg.qpageProgramCmd);
    printf("bh->flash_cfg.cfg.qppAddrMode = %x\n", bh->flash_cfg.cfg.qppAddrMode);
    printf("bh->flash_cfg.cfg.fastReadCmd = %x\n", bh->flash_cfg.cfg.fastReadCmd);
    printf("bh->flash_cfg.cfg.frDmyClk = %x\n", bh->flash_cfg.cfg.frDmyClk);
    printf("bh->flash_cfg.cfg.qpiFastReadCmd = %x\n", bh->flash_cfg.cfg.qpiFastReadCmd);
    printf("bh->flash_cfg.cfg.qpiFrDmyClk = %x\n", bh->flash_cfg.cfg.qpiFrDmyClk);
    printf("bh->flash_cfg.cfg.fastReadDoCmd = %x\n", bh->flash_cfg.cfg.fastReadDoCmd);
    printf("bh->flash_cfg.cfg.frDoDmyClk = %x\n", bh->flash_cfg.cfg.frDoDmyClk);
    printf("bh->flash_cfg.cfg.fastReadDioCmd = %x\n", bh->flash_cfg.cfg.fastReadDioCmd);
    printf("bh->flash_cfg.cfg.frDioDmyClk = %x\n", bh->flash_cfg.cfg.frDioDmyClk);
    printf("bh->flash_cfg.cfg.fastReadQoCmd = %x\n", bh->flash_cfg.cfg.fastReadQoCmd);
    printf("bh->flash_cfg.cfg.frQoDmyClk = %x\n", bh->flash_cfg.cfg.frQoDmyClk);
    printf("bh->flash_cfg.cfg.fastReadQioCmd = %x\n", bh->flash_cfg.cfg.fastReadQioCmd);
    printf("bh->flash_cfg.cfg.frQioDmyClk = %x\n", bh->flash_cfg.cfg.frQioDmyClk);
    printf("bh->flash_cfg.cfg.qpiFastReadQioCmd = %x\n", bh->flash_cfg.cfg.qpiFastReadQioCmd);
    printf("bh->flash_cfg.cfg.qpiFrQioDmyClk = %x\n", bh->flash_cfg.cfg.qpiFrQioDmyClk);
    printf("bh->flash_cfg.cfg.qpiPageProgramCmd = %x\n", bh->flash_cfg.cfg.qpiPageProgramCmd);
    printf("bh->flash_cfg.cfg.writeVregEnableCmd = %x\n", bh->flash_cfg.cfg.writeVregEnableCmd);
    printf("bh->flash_cfg.cfg.wrEnableIndex = %x\n", bh->flash_cfg.cfg.wrEnableIndex);
    printf("bh->flash_cfg.cfg.qeIndex = %x\n", bh->flash_cfg.cfg.qeIndex);
    printf("bh->flash_cfg.cfg.busyIndex = %x\n", bh->flash_cfg.cfg.busyIndex);
    printf("bh->flash_cfg.cfg.wrEnableBit = %x\n", bh->flash_cfg.cfg.wrEnableBit);
    printf("bh->flash_cfg.cfg.qeBit = %x\n", bh->flash_cfg.cfg.qeBit);
    printf("bh->flash_cfg.cfg.busyBit = %x\n", bh->flash_cfg.cfg.busyBit);
    printf("bh->flash_cfg.cfg.wrEnableWriteRegLen = %x\n", bh->flash_cfg.cfg.wrEnableWriteRegLen);
    printf("bh->flash_cfg.cfg.wrEnableReadRegLen = %x\n", bh->flash_cfg.cfg.wrEnableReadRegLen);
    printf("bh->flash_cfg.cfg.qeWriteRegLen = %x\n", bh->flash_cfg.cfg.qeWriteRegLen);
    printf("bh->flash_cfg.cfg.qeReadRegLen = %x\n", bh->flash_cfg.cfg.qeReadRegLen);
    printf("bh->flash_cfg.cfg.releasePowerDown = %x\n", bh->flash_cfg.cfg.releasePowerDown);
    printf("bh->flash_cfg.cfg.busyReadRegLen = %x\n", bh->flash_cfg.cfg.busyReadRegLen);
    for (int i=0; i<4; i++) printf("bh->flash_cfg.cfg.readRegCmd[%d] = %x\n", i, bh->flash_cfg.cfg.readRegCmd[i]);
    for (int i=0; i<4; i++) printf("bh->flash_cfg.cfg.writeRegCmd[%d] = %x\n", i, bh->flash_cfg.cfg.writeRegCmd[i]);
    printf("bh->flash_cfg.cfg.enterQpi = %x\n", bh->flash_cfg.cfg.enterQpi);
    printf("bh->flash_cfg.cfg.exitQpi = %x\n", bh->flash_cfg.cfg.exitQpi);
    printf("bh->flash_cfg.cfg.cReadMode = %x\n", bh->flash_cfg.cfg.cReadMode);
    printf("bh->flash_cfg.cfg.cRExit = %x\n", bh->flash_cfg.cfg.cRExit);
    printf("bh->flash_cfg.cfg.burstWrapCmd = %x\n", bh->flash_cfg.cfg.burstWrapCmd);
    printf("bh->flash_cfg.cfg.burstWrapCmdDmyClk = %x\n", bh->flash_cfg.cfg.burstWrapCmdDmyClk);
    printf("bh->flash_cfg.cfg.burstWrapDataMode = %x\n", bh->flash_cfg.cfg.burstWrapDataMode);
    printf("bh->flash_cfg.cfg.burstWrapData = %x\n", bh->flash_cfg.cfg.burstWrapData);
    printf("bh->flash_cfg.cfg.deBurstWrapCmd = %x\n", bh->flash_cfg.cfg.deBurstWrapCmd);
    printf("bh->flash_cfg.cfg.deBurstWrapCmdDmyClk = %x\n", bh->flash_cfg.cfg.deBurstWrapCmdDmyClk);
    printf("bh->flash_cfg.cfg.deBurstWrapDataMode = %x\n", bh->flash_cfg.cfg.deBurstWrapDataMode);
    printf("bh->flash_cfg.cfg.deBurstWrapData = %x\n", bh->flash_cfg.cfg.deBurstWrapData);
    printf("bh->flash_cfg.cfg.timeEsector = %x\n", bh->flash_cfg.cfg.timeEsector);
    printf("bh->flash_cfg.cfg.timeE32k = %x\n", bh->flash_cfg.cfg.timeE32k);
    printf("bh->flash_cfg.cfg.timeE64k = %x\n", bh->flash_cfg.cfg.timeE64k);
    printf("bh->flash_cfg.cfg.timePagePgm = %x\n", bh->flash_cfg.cfg.timePagePgm);
    printf("bh->flash_cfg.cfg.timeCe = %x\n", bh->flash_cfg.cfg.timeCe);
    printf("bh->flash_cfg.cfg.pdDelay = %x\n", bh->flash_cfg.cfg.pdDelay);
    printf("bh->flash_cfg.cfg.qeData = %x\n", bh->flash_cfg.cfg.qeData);
    
    printf("bh->flash_cfg.crc32 = %x\n", bh->flash_cfg.crc32);
    
    printf("bh->clk_cfg.magiccode = %x\n", bh->clk_cfg.magiccode);
    
    printf("bh->clk_cfg.cfg.xtal_type = %x\n", bh->clk_cfg.cfg.xtal_type);
    printf("bh->clk_cfg.cfg.mcu_clk = %x\n", bh->clk_cfg.cfg.mcu_clk);
    printf("bh->clk_cfg.cfg.mcu_clk_div = %x\n", bh->clk_cfg.cfg.mcu_clk_div);
    printf("bh->clk_cfg.cfg.mcu_bclk_div = %x\n", bh->clk_cfg.cfg.mcu_bclk_div);
    printf("bh->clk_cfg.cfg.mcu_pbclk_div = %x\n", bh->clk_cfg.cfg.mcu_pbclk_div);
    printf("bh->clk_cfg.cfg.lp_div = %x\n", bh->clk_cfg.cfg.lp_div);
    printf("bh->clk_cfg.cfg.dsp_clk = %x\n", bh->clk_cfg.cfg.dsp_clk);
    printf("bh->clk_cfg.cfg.dsp_clk_div = %x\n", bh->clk_cfg.cfg.dsp_clk_div);
    printf("bh->clk_cfg.cfg.dsp_bclk_div = %x\n", bh->clk_cfg.cfg.dsp_bclk_div);
    printf("bh->clk_cfg.cfg.dsp_pbclk = %x\n", bh->clk_cfg.cfg.dsp_pbclk);
    printf("bh->clk_cfg.cfg.dsp_pbclk_div = %x\n", bh->clk_cfg.cfg.dsp_pbclk_div);
    printf("bh->clk_cfg.cfg.emi_clk = %x\n", bh->clk_cfg.cfg.emi_clk);
    printf("bh->clk_cfg.cfg.emi_clk_div = %x\n", bh->clk_cfg.cfg.emi_clk_div);
    printf("bh->clk_cfg.cfg.flash_clk_type = %x\n", bh->clk_cfg.cfg.flash_clk_type);
    printf("bh->clk_cfg.cfg.flash_clk_div = %x\n", bh->clk_cfg.cfg.flash_clk_div);
    printf("bh->clk_cfg.cfg.wifipll_pu = %x\n", bh->clk_cfg.cfg.wifipll_pu);
    printf("bh->clk_cfg.cfg.aupll_pu = %x\n", bh->clk_cfg.cfg.aupll_pu);
    printf("bh->clk_cfg.cfg.cpupll_pu = %x\n", bh->clk_cfg.cfg.cpupll_pu);
    printf("bh->clk_cfg.cfg.mipipll_pu = %x\n", bh->clk_cfg.cfg.mipipll_pu);
    printf("bh->clk_cfg.cfg.uhspll_pu = %x\n", bh->clk_cfg.cfg.uhspll_pu);
    
    printf("bh->clk_cfg.crc32 = %x\n", bh->clk_cfg.crc32);
    
    printf("bh->basic_cfg.sign_type = %x\n", bh->basic_cfg.sign_type);
    printf("bh->basic_cfg.encrypt_type = %x\n", bh->basic_cfg.encrypt_type);
    printf("bh->basic_cfg.key_sel = %x\n", bh->basic_cfg.key_sel);
    printf("bh->basic_cfg.xts_mode = %x\n", bh->basic_cfg.xts_mode);
    printf("bh->basic_cfg.aes_region_lock = %x\n", bh->basic_cfg.aes_region_lock);
    printf("bh->basic_cfg.no_segment = %x\n", bh->basic_cfg.no_segment);
    printf("bh->basic_cfg.rsvd_0 = %x\n", bh->basic_cfg.rsvd_0);
    printf("bh->basic_cfg.rsvd_1 = %x\n", bh->basic_cfg.rsvd_1);
    printf("bh->basic_cfg.cpu_master_id = %x\n", bh->basic_cfg.cpu_master_id);
    printf("bh->basic_cfg.notload_in_bootrom = %x\n", bh->basic_cfg.notload_in_bootrom);
    printf("bh->basic_cfg.crc_ignore = %x\n", bh->basic_cfg.crc_ignore);
    printf("bh->basic_cfg.hash_ignore = %x\n", bh->basic_cfg.hash_ignore);
    printf("bh->basic_cfg.power_on_mm = %x\n", bh->basic_cfg.power_on_mm);
    printf("bh->basic_cfg.em_sel = %x\n", bh->basic_cfg.em_sel);
    printf("bh->basic_cfg.cmds_en = %x\n", bh->basic_cfg.cmds_en);
    printf("bh->basic_cfg.cmds_wrap_mode = %x\n", bh->basic_cfg.cmds_wrap_mode);
    printf("bh->basic_cfg.cmds_wrap_len = %x\n", bh->basic_cfg.cmds_wrap_len);
    printf("bh->basic_cfg.icache_invalid = %x\n", bh->basic_cfg.icache_invalid);
    printf("bh->basic_cfg.dcache_invalid = %x\n", bh->basic_cfg.dcache_invalid);
    printf("bh->basic_cfg.rsvd_3 = %x\n", bh->basic_cfg.rsvd_3);
    printf("bh->basic_cfg.group_image_offset = %x\n", bh->basic_cfg.group_image_offset);
    printf("bh->basic_cfg.aes_region_len = %x\n", bh->basic_cfg.aes_region_len);
    printf("bh->basic_cfg.img_len_cnt = %x\n", bh->basic_cfg.img_len_cnt);
    for (int i=0; i<8; i++) printf("bh->basic_cfg.hash[%d] = %x\n", i, bh->basic_cfg.hash[i]);
    
    for (int i=0; i<3; i++) {
        printf("bh->cpu_cfg[%d].config_enable = %x\n", i, bh->cpu_cfg[i].config_enable);
        printf("bh->cpu_cfg[%d].halt_cpu = %x\n", i, bh->cpu_cfg[i].halt_cpu);
        printf("bh->cpu_cfg[%d].cache_enable = %x\n", i, bh->cpu_cfg[i].cache_enable);
        printf("bh->cpu_cfg[%d].cache_wa = %x\n", i, bh->cpu_cfg[i].cache_wa);
        printf("bh->cpu_cfg[%d].cache_wb = %x\n", i, bh->cpu_cfg[i].cache_wb);
        printf("bh->cpu_cfg[%d].cache_wt = %x\n", i, bh->cpu_cfg[i].cache_wt);
        printf("bh->cpu_cfg[%d].cache_way_dis = %x\n", i, bh->cpu_cfg[i].cache_way_dis);
        printf("bh->cpu_cfg[%d].rsvd = %x\n", i, bh->cpu_cfg[i].rsvd);
        printf("bh->cpu_cfg[%d].cache_range_h = %x\n", i, bh->cpu_cfg[i].cache_range_h);
        printf("bh->cpu_cfg[%d].cache_range_l = %x\n", i, bh->cpu_cfg[i].cache_range_l);
        printf("bh->cpu_cfg[%d].image_address_offset = %x\n", i, bh->cpu_cfg[i].image_address_offset);
        printf("bh->cpu_cfg[%d].rsvd0 = %x\n", i, bh->cpu_cfg[i].rsvd0);
        printf("bh->cpu_cfg[%d].msp_val = %x\n", i, bh->cpu_cfg[i].msp_val);
    }
    
    printf("bh->boot2_pt_table_0_rsvd = %x\n", bh->boot2_pt_table_0_rsvd);
    printf("bh->boot2_pt_table_1_rsvd = %x\n", bh->boot2_pt_table_1_rsvd);
    printf("bh->flash_cfg_table_addr = %x\n", bh->flash_cfg_table_addr);
    printf("bh->flash_cfg_table_len = %x\n", bh->flash_cfg_table_len);
    for (int i=0; i<8; i++) printf("bh->rsvd0[%d] = %x\n", i, bh->rsvd0[i]);
    for (int i=0; i<8; i++) printf("bh->rsvd1[%d] = %x\n", i, bh->rsvd1[i]);
    for (int i=0; i<5; i++) printf("bh->rsvd3[%d] = %x\n", i, bh->rsvd3[i]);
    printf("bh->crc32 = %x\n", bh->crc32);
}
