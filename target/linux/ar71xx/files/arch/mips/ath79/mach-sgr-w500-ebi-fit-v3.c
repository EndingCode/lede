/*
 * GRENTECH SGR-W500-EBI(FIT) v3.0 board support
 *
 * Copyright (c) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "pci.h"
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "machtypes.h"

#define GRENTECH_GPIO_LED_STATUS		5
#define GRENTECH_GPIO_LED_WAN			4
#define GRENTECH_GPIO_LED_WLAN			3

#define GRENTECH_GPIO_BUTTON_RESET		8

#define GRENTECH_KEYS_POLL_INTERVAL		20	/* msecs */
#define GRENTECH_KEYS_DEBOUNCE_INTERVAL		(3 * GRENTECH_KEYS_POLL_INTERVAL)

static struct gpio_led grentech_leds_gpio[] __initdata = {
	{
		.name		= "grentech:green:status",
		.gpio		= GRENTECH_GPIO_LED_STATUS,
		.active_low	= 1,
	},
	{
		.name		= "grentech:green:wan",
		.gpio		= GRENTECH_GPIO_LED_WAN,
		.active_low	= 0,
	},
	{
		.name		= "grentech:green:wlan",
		.gpio		= GRENTECH_GPIO_LED_WLAN,
		.active_low	= 0,
	}
};

static struct gpio_keys_button grentech_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = GRENTECH_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= GRENTECH_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
};

static struct mdio_board_info grentech_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 1,
	},
};

static void __init grentech_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(grentech_leds_gpio),
				 grentech_leds_gpio);
	ath79_register_gpio_keys_polled(-1, GRENTECH_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(grentech_gpio_keys),
					grentech_gpio_keys);

	ath79_register_pci();

	ath79_register_mdio(0, 0);

	mdiobus_register_board_info(grentech_mdio0_info,
				    ARRAY_SIZE(grentech_mdio0_info));

	ath79_init_mac(ath79_eth0_data.mac_addr, art, 0);

	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(1);
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_pll_data.pll_1000 = 0x00110000;
	ath79_eth0_pll_data.pll_100 = 0x0001099;
	ath79_eth0_pll_data.pll_10 = 0x00991099;

	ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_SGR_W500_EBI_FIT_V3, "SGRW500EBIFITV3", "GRENTECH SGR-W500-EBI(FIT) v3.0",
	     grentech_setup);
