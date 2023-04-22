/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "smc.h"

#include "types.h"
#include "utils.h"
#include "i2c.h"
#include "latte.h"
#include "exi.h"
#include "asic.h"
#include "gfx.h"
#include "gpio.h"

// 0x00 - odd on (raw)
// 0x01 - odd off (raw)
// 0x02 - odd eject (raw)
// 0x0D - ?    coldboot: 0x00
// 0x0E - ?    coldboot: 0x00
// 0x0F - ?    coldboot: 0x00
// 0x10 - ON LED_ON (OFF LED_OFF?)
// 0x11 - ON LED_ON?
// 0x12 - ON LED_OFF
// 0x13 - ON LED_PULSE
// 0x14 - CC LED_OFF
// 0x15 - CC LED_ON
// 0x16 - CC LED_PULSE
// 0x17 - ?
// 0x18 - ?
// 0x19 - ?
// 0x1A - ?
// 0x1B - ?
// 0x1C - ?
// 0x1D - ?
// 0x1E - OFF LED_ON
// 0x1F - OFF LED_PULSE
// 0x20 - bt_rst (raw)
// 0x21 - wifi_rst (raw)
// 0x22 - drc_wifi_rst (raw)

// 0x30 - TimerCounter
// 0x31 - ?
// 0x32 - ?
// 0x33~0x3F - ?                 coldboot: 0xFF

// 0x40 - ProgramRevision   coldboot: 0xC5
// 0x41 - SystemEventFlag   coldboot: 0x00
// 0x42 - ?                 coldboot: 0x0b
// 0x43 - usb power         coldboot: 0x03
// 0x44 - NotificationLED   coldboot: 0x20
// 0x45 - PowerFailureState coldboot: 0x8
// 0x46 - WifiRstCtrl       coldboot: 0x0
// 0x47 - ?                 coldboot: 0xFF
// 0x48 - ?                 coldboot: 0x0F

// 0x49~0x4F - ?            coldboot: 0xFF

// 0x60 - LAPSSetting       coldboot: 0x00
// 0x61 - SystemError       coldboot: 0x00
// 0x62 - ?                 coldboot: 0x00
// 0x63 - ?                 coldboot: 0x00
// 0x64 - ?                 coldboot: 0x00
// 0x65 - ?                 coldboot: 0x00
// 0x66 - ?                 coldboot: 0x00
// 0x67 - ?                 coldboot: 0x00
// 0x68 - ?                 coldboot: 0x00
// 0x69 - ?                 coldboot: 0x00
// 0x6A - ?                 coldboot: 0x00
// 0x6B - ?                 coldboot: 0x00
// 0x6C - ?                 coldboot: 0x00
// 0x6D - ?                 coldboot: 0x00
// 0x6E - ?                 coldboot: 0x00
// 0x6F - ?                 coldboot: 0x00
// 0x70 - ?                 coldboot: 0x00
// 0x71 - blinks drive LED? coldboot: 0xFF
// 0x72~0x7F - ?            coldboot: 0xFF

// I think it wraps around here
// 0x80~0x8C - ?            coldboot: 0xFF
// 0x8D~0x8F - ?            coldboot: 0x00


int smc_read_register(u8 offset, u8* data)
{
    int res = 0;

    // Clock is 10000 in C2W, but 5000 in IOS...
    i2c_init(5000, 1);

    res = i2c_write(I2C_SLAVE_SMC, &offset, 1);
    if(res) return res;

    res = i2c_read(I2C_SLAVE_SMC, data, 1);
    return res;
}

int smc_write_register(u8 offset, u8 data)
{
    // Clock is 10000 in C2W, but 5000 in IOS...
    i2c_init(5000, 1);

    u8 cmd[2] = {offset, data};
    return i2c_write(I2C_SLAVE_SMC, cmd, 2);
}

int smc_mask_register(u8 offset, u8 mask, u8 val)
{
    int ret = 0;
    u8 existing = 0;
    if (ret = smc_read_register(offset, &existing)) {
        return ret;
    }

    existing &= ~mask;
    existing |= val;

    ret = smc_write_register(offset, &existing);
    return ret;
}

int smc_write_raw(u8 data)
{
    // Clock is 10000 in C2W, but 5000 in IOS...
    i2c_init(5000, 1);

    return i2c_write(I2C_SLAVE_SMC, &data, 1);
}

int smc_set_odd_power(bool enable)
{
    u8 cmd = 0x01;
    if(enable) cmd = 0x00;

    return smc_write_raw(cmd);
}

int smc_eject_request()
{
    return smc_write_raw(0x02);
}

int smc_set_cc_indicator(int state)
{
    u8 cmd = 0x14;

    switch(state) {
        case LED_ON:
            cmd = 0x15;
            break;

        case LED_PULSE:
            cmd = 0x16;
            break;

        default: break;
    }

    return smc_write_raw(cmd);
}

int smc_set_on_indicator(int state)
{
    u8 cmd = 0x12;

    switch(state) {
        case LED_ON:
            cmd = 0x10;
            break;

        case LED_PULSE:
            cmd = 0x13;
            break;

        default: break;
    }

    return smc_write_raw(cmd);
}

int smc_set_off_indicator(int state)
{
    u8 cmd = 0x10;

    switch(state) {
        case LED_ON:
            cmd = 0x1E;
            break;

        case LED_PULSE:
            cmd = 0x1F;
            break;

        default: break;
    }

    return smc_write_raw(cmd);
}

int smc_bt_rst()
{
    return smc_write_raw(0x20);
}

int smc_wifi_rst()
{
    return smc_write_raw(0x21);
}

int smc_drc_wifi_rst()
{
    return smc_write_raw(0x22);
}

int smc_set_rearusb_power(int state)
{
    smc_mask_register(0x43, 2, state ? 2 : 0);
}

int smc_set_frontusb_power(int state)
{
    smc_mask_register(0x43, 1, state ? 1 : 0);
}

int smc_set_wifi_idk(int state)
{
    smc_mask_register(0x46, 1, state ? 1 : 0);
}

u8 smc_get_events(void)
{
    u8 data = 0;
    smc_read_register(0x41, &data);
    return data;
}

u8 smc_wait_events(u8 mask)
{
    smc_get_events();

    while(true) {
        u8 data = smc_get_events();
        if(data & mask) return data & mask;
    }
}

void smc_set_ctrl1(u32 val)
{
    exi0_write32(0x21000D00, val);
}

u32 smc_get_ctrl1()
{
    return exi0_read32(0x21000D00);
}

void smc_set_ctrl0(u32 val)
{
    exi0_write32(0x21000C00, val);
}

u32 smc_get_ctrl0()
{
    return exi0_read32(0x21000C00);
}

void smc_get_panic_reason(char* buffer)
{
    u32* buf32 = (u32*)buffer;

    write32(EXI0_CSR, 0x108);
    write32(EXI0_DATA, 0x20000100);
    write32(EXI0_CR, 0x35);
    while(!(read32(EXI0_CSR) & 8));

    for(int i = 0; i < 64 / sizeof(u32); i++)
    {
        write32(EXI0_CSR, 0x108);
        write32(EXI0_CR, 0x31);
        while(!(read32(EXI0_CSR) & 8));

        buf32[i] = read32(EXI0_DATA);
    }

    write32(EXI0_CSR, 0);
}

void smc_set_panic_reason(const char* buffer)
{
    const u32* buf32 = (const u32*)buffer;

    write32(EXI0_CSR, 0x108);
    write32(EXI0_DATA, 0xA0000100);
    write32(EXI0_CR, 0x35);
    while(!(read32(EXI0_CSR) & 8));

    for(int i = 0; i < 64 / sizeof(u32); i++)
    {
        write32(EXI0_CSR, 0x108);
        write32(EXI0_DATA, buf32[i]);
        write32(EXI0_CR, 0x35);
        while(!(read32(EXI0_CSR) & 8));
    }

    write32(EXI0_CSR, 0);
}

void SRAM_TEXT smc_shutdown(bool reset)
{
    if (reset) {
        printf("Resetting...\n");
    }
    else {
        printf("Powering down...\n");
    }
    // Request a reset
    serial_send_u32(0x55AA55AA);
    serial_send_u32(0x55AA55AA);
    serial_send_u32(0x55AA55AA);
    serial_send_u32(0x55AA55AA);
    serial_send_u32(0x4E525354);

    write16(MEM_FLUSH_MASK, 0b1111);
    while(read16(MEM_FLUSH_MASK) & 0b1111);

    if(read32(LT_RESETS) & 4) {
        // Our stack is in DRAM :)
        //abif_gpu_write32(0x8020, 0xFFFFFFFF);
        //abif_gpu_write32(0x0E60, 0xFFFFFFDB);

        write32(LT_ABIF_OFFSET, 0xC0008020);
        write32(LT_ABIF_DATA, 0xFFFFFFFF);
        write32(LT_ABIF_OFFSET, 0xC0000E60);
        write32(LT_ABIF_DATA, 0xFFFFFFDB);
    }

    write32(LT_RESETS_AHB, 0xFFFFCE71);
    write32(LT_RESETS_AHMN, 0xFFFFCD70);
    write32(LT_RESETS_COMPAT, 0xFF8FCDEF);

    write16(MEM_REFRESH_FLAG, 0);

    write16(MEM_SEQ_REG_ADDR, 0x18);
    write16(MEM_SEQ_REG_VAL, 1);
    write16(MEM_SEQ_REG_ADDR, 0x19);
    write16(MEM_SEQ_REG_VAL, 0);
    write16(MEM_SEQ_REG_ADDR, 0x1A);
    write16(MEM_SEQ_REG_VAL, 1);

    write16(MEM_SEQ0_REG_ADDR, 0x18);
    write16(MEM_SEQ0_REG_VAL, 1);
    write16(MEM_SEQ0_REG_ADDR, 0x19);
    write16(MEM_SEQ0_REG_VAL, 0);
    write16(MEM_SEQ0_REG_ADDR, 0x1A);
    write16(MEM_SEQ0_REG_VAL, 1);

    if(reset) {
        {
            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0xA1000D00);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0x501);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0);
        }

        {
            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0xA1000100);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0);
        }

        clear32(LT_RESETS, 1);
    } else {
        {
            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0xA1000100);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0);
        }

        {
            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0xA1000D00);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0x101);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0);
        }

        {
            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0xA1000D00);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0x108);
            write32(EXI0_DATA, 0x10101);
            write32(EXI0_CR, 0x35);
            while(!(read32(EXI0_CSR) & 8));

            write32(EXI0_CSR, 0);
        }
    }

    while(true);
}

void smc_power_off(void)
{
    smc_shutdown(false);
}

void smc_reset(void)
{
    smc_shutdown(true);
}
