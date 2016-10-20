/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_string.h"
#include "lib_queue.h"
#include "lib_malloc.h"
#include "lib_printf.h"

#include "bsp_config.h"
#include "common.h"
#include "iocb.h"
#include "device.h"
#include "console.h"
#include "devfuncs.h"
#include "timer.h"
#include "ioctl.h"

#include "error.h"

#include "env_subr.h"
#include "ui_command.h"
#include "ui_init.h"
#include "bolt.h"

#include "net_ebuf.h"
#include "net_ether.h"

#include "net_api.h"
#include "net_mdio.h"

#include "fileops.h"
#include "ssbl-common.h"
#include "board.h"

#include "board_init.h"

#define ip_addriszero(a) (((a)[0]|(a)[1]|(a)[2]|(a)[3]) == 0)
#define isdigit(d) (((d) >= '0') && ((d) <= '9'))

#if CFG_NETWORK
static int ui_cmd_ifconfig(ui_cmdline_t *cmd, int argc, char *argv[]);

#if (CFG_CMD_LEVEL >= 4)
static int ui_cmd_arp(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

#if (CFG_CMD_LEVEL >= 2)
static int ui_cmd_ping(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

#if CFG_TCP
extern int ui_init_tcpcmds(void);
#endif

#if CFG_ENET
static int ui_cmd_ephy_config(ui_cmdline_t *cmd, int argc, char *argv[]);
static int ui_cmd_mii_read_write(ui_cmdline_t *cmd, int argc, char *argv[]);

#if CFG_SYSTEMPORT
extern void sf2_port_isolate_setup(const enet_params *e, unsigned int port_mask);
static int ui_cmd_switch_config(ui_cmdline_t *cmd, int argc, char *argv[]);
#endif

#endif /* CFG_ENET */
#endif /* CFG_NETWORK */

struct netparam {
	const char *str;
	int num;
};

static const struct netparam loopbacktypes[] = {
	{"off", ETHER_LOOPBACK_OFF},
	{"mac_internal", ETHER_LOOPBACK_MAC_INT},
	{"mac_external", ETHER_LOOPBACK_MAC_EXT},
	{"phy_internal", ETHER_LOOPBACK_PHY_INT},
	{"phy_external", ETHER_LOOPBACK_PHY_EXT},
	{NULL, 0}
};

static const struct netparam speedtypes[] = {
	{"auto", ETHER_SPEED_AUTO},
	{"10hdx", ETHER_SPEED_10HDX},
	{"10fdx", ETHER_SPEED_10FDX},
	{"100hdx", ETHER_SPEED_100HDX},
	{"100fdx", ETHER_SPEED_100FDX},
	{"1000hdx", ETHER_SPEED_1000HDX},
	{"1000fdx", ETHER_SPEED_1000FDX},
	{NULL, 0}
};

static const char * const env_vars[] = {
	"PHY", "MDIO_MODE", "SPEED", "PHYADDR", "PHYPROBE_ID"
};

static const char * const help_str[] = {
	"",
	ANSI_YELLOW "(defaults)" ANSI_RESET,
	ANSI_RED "(current)" ANSI_RESET
};

int ui_init_netcmds(void)
{
#if CFG_NETWORK
	cmd_addcmd("ifconfig",
		   ui_cmd_ifconfig,
		   NULL,
		   "Configure the Ethernet interface",
		   "ifconfig device [options..]\n\n"
		   "Activates and configures the specified Ethernet interface and sets its\n"
		   "IP address, netmask, and other parameters.  The -auto switch can be used\n"
		   "to set this information via DHCP.",
		   "-auto;Configure interface automatically via DHCP|"
		   "-off;Deactivate the specified interface|"
		   "-addr=*;Specifies the IP address of the interface|"
		   "-mask=*;Specifies the subnet mask for the interface|"
		   "-gw=*;Specifies the gateway address for the interface|"
		   "-dns=*;Specifies the name server address for the interface|"
		   "-domain=*;Specifies the default domain for name service queries|"
		   "-speed=*;Sets the interface speed (auto,10fdx,10hdx,\n100fdx,\n"
		   "100hdx,1000fdx,1000hdx)|"
		   "-loopback=*;Sets the loopback mode (off, mac_internal, mac_external, phy_internal,\n"
		   "phy_external).  'mac_internal' sets all data transmitted by the MAC be internally\n"
		   "received by the MAC receiver.  'mac_external' sets all data received by the MAC\n"
		   "be loopback to the MAC transmitter.  'phy_internal' sets all data transmitted by\n"
		   "the PHY be internally received by the PHY receiver.  And 'phy_external' sets all\n"
		   "data received by the PHY be loopback to PHY transmitter.|"
		   "-hwaddr=*;Sets the hardware address (overrides environment)");

#if (CFG_CMD_LEVEL >= 4)
	cmd_addcmd("arp",
		   ui_cmd_arp,
		   NULL,
		   "Display or modify the ARP Table",
		   "arp [-d] [ip-address] [dest-address]\n\n"
		   "Without any parameters, the arp command will display the contents of the\n"
		   "arp table.  With two parameters, arp can be used to add permanent arp\n"
		   "entries to the table (permanent arp entries do not time out)",
		   "-d;Delete the specified ARP entry.  If specified, ip-address\n"
		   "may be * to delete all entries.");
#endif

#if (CFG_CMD_LEVEL >= 2)
	cmd_addcmd("ping",
		   ui_cmd_ping,
		   NULL,
		   "Ping a remote IP host.",
		   "ping [-t] remote-host\n\n"
		   "This command sends an ICMP ECHO message to a remote host and waits for\n"
		   "a reply.  The network interface must be configured and operational for\n"
		   "this command to work.  If the interface is configured for loopback mode\n"
		   "the packet will be sent through the network interface, so this command\n"
		   "can be used for a simple network test.",
		   "-t;ping forever, or until the enter key is struck|"
		   "-x;exit immediately on first error (use with -f or -t)|"
		   "-f;flood ping (use carefully!) - ping as fast as possible|"
		   "-s=*;specify the number of icmp data bytes|"
		   "-c=*;specify number of packets to echo|"
		   "-A;don't abort even if key is pressed|"
		   "-E;Require all packets sent to be returned, for successful return status");
#endif

#if CFG_TCP
	ui_init_tcpcmds();
#endif

#if CFG_ENET
	cmd_addcmd("ephycfg",
		   ui_cmd_ephy_config,
		   NULL,
		   "Configure Ethernet PHY interface",
		   "This command setup following environment variable for an Ethernet\n"
		   "PHY interface.\n\n"
		   "ETH0_PHY [INT|MII|RGMII|RGMII_NO_ID]\n"
		   "ETH0_MDIO_MODE [0|1|boot]\n"
		   "ETH0_SPEED [100|1000]\n",
		   "-i=*;Sets the Ethernet interface|"
		   "-e;Expert mode, allows setting non-working configurations|"
		   "-r;Reset to board defaults.");
	cmd_addcmd("mii",
		   ui_cmd_mii_read_write,
		   NULL,
		   "Reads or writes register contents from/to specified PHY address\n"
		   "on a given MDIO bus (use show devices to discover the bus).\n",
		   "mii [read|write] [device] [phy] [reg] [value]",
		   "");
#if CFG_SYSTEMPORT
	cmd_addcmd("switchcfg",
		   ui_cmd_switch_config,
		   NULL,
		   "Configure integrated Ethernet switch settings.\n\n",
		   "Configures the integrate Ethernet switch port settings.\n",
		   "-port=*;specifies the port number|"
		   "-isolate_mask=*;specifies the bitmask to include/exclude ports from\n"
		   "use 0 to reset to include Port + CPU port.");
#endif /* CFG_SYSTEMPORT */
#endif /* CFG_ENET */

#endif /* CFG_NETWORK */

	return 0;
}

#if CFG_NETWORK
static int parsexdigit(char str)
{
	int digit;

	if ((str >= '0') && (str <= '9'))
		digit = str - '0';
	else if ((str >= 'a') && (str <= 'f'))
		digit = str - 'a' + 10;
	else if ((str >= 'A') && (str <= 'F'))
		digit = str - 'A' + 10;
	else
		return -1;

	return digit;
}

static int parsehwaddr(const char *str, uint8_t *hwaddr)
{
	int digit1, digit2;
	int idx = 6;

	while (*str && (idx > 0)) {
		digit1 = parsexdigit(*str);
		if (digit1 < 0)
			return -1;
		str++;
		if (!*str)
			return -1;

		if ((*str == ':') || (*str == '-')) {
			digit2 = digit1;
			digit1 = 0;
		} else {
			digit2 = parsexdigit(*str);
			if (digit2 < 0)
				return -1;
			str++;
		}

		*hwaddr++ = (digit1 << 4) | digit2;
		idx--;

		if (*str == '-')
			str++;
		if (*str == ':')
			str++;
	}
	return 0;
}

static int ui_ifdown(void)
{
	char *devname;

	devname = (char *)net_getparam(NET_DEVNAME);
	if (devname) {
		xprintf("Device %s has been deactivated.\n", devname);
		net_uninit();
		net_setnetvars();
	}

	return 0;
}

static void print_ip(const char *prefix, uint8_t *ipaddr)
{
	char str[20];

	sprintf_ip(str, ipaddr);
	xprintf("%s%s", prefix, str);
}

static void print_hwaddr(uint8_t *hwaddr)
{
	int i;

	for (i = 0; i < 5; i++)
		xprintf("%02X-", hwaddr[i]);
	xprintf("%02X", hwaddr[5]);
}

static void ui_showifconfig(dhcpreply_t *dhcp)
{
	char *devname;
	uint8_t *addr;
	char nul_ip[IP_ADDR_LEN] = { 0 };
	char nul_enet[ENET_ADDR_LEN] = { 0 };

	devname = (char *)net_getparam(NET_DEVNAME);
	if (devname == NULL) {
		xprintf("Network interface has not been configured\n");
		return;
	}

	xprintf("Device %s: ", devname);

	addr = net_getparam(NET_HWADDR);
	if (addr) {
		xprintf(" hwaddr ");
		print_hwaddr(addr);
	}

	addr = net_getparam(NET_IPADDR);
	if (addr) {
		if (ip_addriszero(addr))
			xprintf(", ipaddr not set");
		else
			print_ip(", ipaddr ", addr);
	}

	addr = net_getparam(NET_NETMASK);
	if (addr) {
		if (ip_addriszero(addr))
			xprintf(", mask not set");
		else
			print_ip(", mask ", addr);
	}

	xprintf("\n");
	xprintf("        ");

	addr = net_getparam(NET_GATEWAY);
	if (addr) {
		if (ip_addriszero(addr))
			xprintf("gateway not set");
		else
			print_ip("gateway ", addr);
	}

	addr = net_getparam(NET_NAMESERVER);
	if (addr) {
		if (ip_addriszero(addr))
			xprintf(", nameserver not set");
		else
			print_ip(", nameserver ", addr);
	}

	addr = net_getparam(NET_DOMAIN);
	if (addr)
		xprintf(", domain %s", addr);

	xprintf("\n");

	if (!dhcp)
		return;

	xprintf("        ");

	if (memcmp(dhcp->dr_dhcpserver, nul_ip, IP_ADDR_LEN))
		print_ip("DHCP server ", dhcp->dr_dhcpserver);

	if (memcmp(dhcp->dr_dhcpserver_hwaddr, nul_enet, ENET_ADDR_LEN)) {
		xprintf(", DHCP server MAC ");
		print_hwaddr(dhcp->dr_dhcpserver_hwaddr);
	}

	xprintf("\n");
}

int do_dhcp_request(char *devname)
{
	dhcpreply_t *reply = NULL;

	if (dhcp_bootrequest(&reply) < 0) {
		xprintf("DHCP registration failed on device %s\n", devname);
		net_uninit();
		return BOLT_ERR_NETDOWN;
	}

	net_setparam(NET_IPADDR, reply->dr_ipaddr);
	net_setparam(NET_NETMASK, reply->dr_netmask);
	net_setparam(NET_GATEWAY, reply->dr_gateway);
	net_setparam(NET_NAMESERVER, reply->dr_nameserver);
	net_setparam(NET_DOMAIN, (uint8_t *) reply->dr_domainname);

	dhcp_set_envvars(reply);

	ui_showifconfig(reply);

	dhcp_free_reply(reply);
	net_setnetvars();
	return 0;
}

static int ui_ifconfig_auto(ui_cmdline_t *cmd, char *devname)
{
	int err;
	const char *x;
	uint8_t hwaddr[6];

	net_uninit();

	err = net_init(devname);
	if (err < 0) {
		xprintf("Could not activate device %s: %s\n",
			devname, bolt_errortext(err));
		return err;
	}

	if (cmd_sw_value(cmd, "-hwaddr", &x)) {
		if (parsehwaddr(x, hwaddr) != 0) {
			xprintf("Invalid hardware address: %s\n", x);
			net_uninit();
			return BOLT_ERR_INV_PARAM;
		} else {
			net_setparam(NET_HWADDR, hwaddr);
		}
	}

	return do_dhcp_request(devname);
}

static int ui_ifconfig_getsw(ui_cmdline_t *cmd, char *swname, char *descr,
			     uint8_t *addr)
{
	const char *x = NULL;

	if (cmd_sw_value(cmd, swname, &x) == 0)
		return 0;

	if ((x == NULL) || (parseipaddr(x, addr) < 0)) {
		xprintf("Invalid %s: %s\n", descr, x ? x : "(none)");
		return -1;
	}

	return 1;
}

static int ui_ifconfig_lookup(const char *name, const char *val,
			      const struct netparam *list)
{
	const struct netparam *p = list;

	while (p->str) {
		if (strcmp(p->str, val) == 0)
			return p->num;
		p++;
	}

	xprintf("Invalid parameter for %s: Valid options are: ", name);

	p = list;
	while (p->str) {
		xprintf("%s ", p->str);
		p++;
	}

	xprintf("\n");
	return -1;
}

#define FLG_IPADDR 1
#define FLG_NETMASK 2
#define FLG_GATEWAY 4
#define FLG_NAMESERVER 8
#define FLG_DOMAIN 16
#define FLG_LOOPBACK 32
#define FLG_SPEED 64
#define FLG_HWADDR 128

static int ui_cmd_ifconfig(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *devname;
	int flags = 0;
	uint8_t ipaddr[IP_ADDR_LEN];
	uint8_t netmask[IP_ADDR_LEN];
	uint8_t gateway[IP_ADDR_LEN];
	uint8_t nameserver[IP_ADDR_LEN];
	uint8_t hwaddr[6];
	int speed = ETHER_SPEED_AUTO;
	int loopback = ETHER_LOOPBACK_OFF;
	const char *domain = NULL;
	int res;
	const char *x;

	if (argc < 1) {
		ui_showifconfig(NULL);
		return 0;
	}

	devname = cmd_getarg(cmd, 0);
	if (!devname)
		return BOLT_ERR_INV_PARAM;

	if (cmd_sw_isset(cmd, "-off"))
		return ui_ifdown();

	if (cmd_sw_isset(cmd, "-auto"))
		return ui_ifconfig_auto(cmd, devname);

	res = ui_ifconfig_getsw(cmd, "-addr", "interface IP address", ipaddr);
	if (res < 0)
		return BOLT_ERR_INV_PARAM;
	if (res > 0)
		flags |= FLG_IPADDR;

	res = ui_ifconfig_getsw(cmd, "-mask", "netmask", netmask);
	if (res < 0)
		return BOLT_ERR_INV_PARAM;
	if (res > 0)
		flags |= FLG_NETMASK;

	res = ui_ifconfig_getsw(cmd, "-gw", "gateway IP address", gateway);
	if (res < 0)
		return BOLT_ERR_INV_PARAM;
	if (res > 0)
		flags |= FLG_GATEWAY;

	res =
	    ui_ifconfig_getsw(cmd, "-dns", "name server IP address",
			      nameserver);
	if (res < 0)
		return BOLT_ERR_INV_PARAM;
	if (res > 0)
		flags |= FLG_NAMESERVER;

	if (cmd_sw_value(cmd, "-domain", &domain)) {
		if (domain)
			flags |= FLG_DOMAIN;
	}

	if (cmd_sw_value(cmd, "-speed", &x)) {
		speed = ui_ifconfig_lookup("-speed", x, speedtypes);
		if (speed >= 0)
			flags |= FLG_SPEED;
		else
			return BOLT_ERR_INV_PARAM;
	}

	if (cmd_sw_value(cmd, "-loopback", &x)) {
		loopback = ui_ifconfig_lookup("-loopback", x, loopbacktypes);
		if (loopback >= 0)
			flags |= FLG_LOOPBACK;
		else
			return BOLT_ERR_INV_PARAM;
	}

	if (cmd_sw_value(cmd, "-hwaddr", &x)) {
		if (parsehwaddr(x, hwaddr) != 0) {
			xprintf("Invalid hardware address: %s\n", x);
			return BOLT_ERR_INV_PARAM;
		} else {
			flags |= FLG_HWADDR;
		}
	}

	/*
	 * If the network is running and the device name is
	 * different, uninit the net first.
	 */

	x = (char *)net_getparam(NET_DEVNAME);

	if ((x != NULL) && (strcmp(x, devname) != 0)) {
		x = NULL;
		net_uninit();
	}

	/*
	 * Okay, initialize the network if it is not already on.  If it
	 * is OFF, the "net_devname" parameter will be NULL.
	 */

	if (x == NULL) {
		res = net_init(devname);	/* turn interface on */
		if (res < 0) {
			ui_showerror(res,
				     "Could not activate network interface '%s'",
				     devname);
			return res;
		}
	}

	/*
	 * Set the parameters
	 */

	if (flags & FLG_HWADDR)
		net_setparam(NET_HWADDR, hwaddr);
	if (flags & FLG_IPADDR)
		net_setparam(NET_IPADDR, ipaddr);
	if (flags & FLG_NETMASK)
		net_setparam(NET_NETMASK, netmask);
	if (flags & FLG_GATEWAY)
		net_setparam(NET_GATEWAY, gateway);
	if (flags & FLG_NAMESERVER)
		net_setparam(NET_NAMESERVER, nameserver);
	if (flags & FLG_DOMAIN)
		net_setparam(NET_DOMAIN, (uint8_t *) domain);
	if (flags & FLG_SPEED)
		net_setparam(NET_SPEED, (uint8_t *) &speed);
	if (flags & FLG_LOOPBACK)
		net_setparam(NET_LOOPBACK, (uint8_t *) &loopback);

	ui_showifconfig(NULL);
	net_setnetvars();

	return 0;
}

#if (CFG_CMD_LEVEL >= 4)
static int ui_cmd_arp(ui_cmdline_t *cmd, int argc, char *argv[])
{
	int idx;
	uint8_t ipaddr[IP_ADDR_LEN];
	uint8_t hwaddr[ENET_ADDR_LEN];
	char *x;
	int once = 0;

	if (cmd_sw_isset(cmd, "-d")) {
		x = cmd_getarg(cmd, 0);
		if (x == NULL)
			return ui_showusage(cmd);

		if (strcmp(x, "*") == 0) {
			while (arp_enumerate(0, ipaddr, hwaddr) >= 0)
				arp_delete(ipaddr);
		} else {
			if (parseipaddr(x, ipaddr) < 0) {
				xprintf("Invalid IP address: %s\n", x);
				return BOLT_ERR_INV_PARAM;
			}
			arp_delete(ipaddr);
		}
		return 0;
	}

	/*
	 * Get the IP address.  If NULL, display the table.
	 */

	x = cmd_getarg(cmd, 0);
	if (x == NULL) {
		idx = 0;
		while (arp_enumerate(idx, ipaddr, hwaddr) >= 0) {
			char ipstr[15];

			if (once == 0) {
				xprintf("Hardware Address   IP Address\n");
				xprintf("-----------------  ---------------\n");
				once = 1;
			}
			print_hwaddr(hwaddr);
			sprintf_ip(ipstr, ipaddr);
			xprintf("  %s\n", ipaddr);
			idx++;
		}
		if (idx == 0)
			xprintf("No ARP entries.\n");
		return 0;
	}

	if (parseipaddr(x, ipaddr) < 0) {
		xprintf("Invalid IP address: %s\n", x);
		return BOLT_ERR_INV_PARAM;
	}

	/*
	 * Get the hardware address.
	 */

	x = cmd_getarg(cmd, 1);
	if (x == NULL)
		return ui_showusage(cmd);

	if (parsehwaddr(x, hwaddr) < 0) {
		xprintf("Invalid hardware address: %s\n", x);
		return BOLT_ERR_INV_PARAM;
	}

	arp_add(ipaddr, hwaddr);

	return 0;
}
#endif

#define IP_HDR_LENGTH    20
#define ICMP_HDR_LENGTH  8
#define PING_HDR_LENGTH  (IP_HDR_LENGTH+ICMP_HDR_LENGTH)
#define MAX_PKT_LENGTH   1500

#if (CFG_CMD_LEVEL >= 2)
static int ui_cmd_ping(ui_cmdline_t *cmd, int argc, char *argv[])
{
	char *host;
	char hostip[20];
	uint8_t hostaddr[IP_ADDR_LEN];
	int res;
	int seq = 0;
	int forever = 0;
	int count = 1;
	int ttlcount = 1;
	int countreturned = 0;
	int size = 56;
	int flood = 0;
	int exitonerror = 0;
	int needexact = 0;
	int noabort = 0;
	const char *x;

	host = cmd_getarg(cmd, 0);
	if (!host)
		return -1;

	if (cmd_sw_isset(cmd, "-t"))
		forever = 1;

	/* Per traditional Unix usage, the size argument to ping is
	   the number of ICMP data bytes.  The frame on the wire will also
	   include the ethernet, IP and ICMP headers (14, 20, and
	   8 bytes respectively) and ethernet trailer (CRC, 4 bytes). */
	if (cmd_sw_value(cmd, "-s", &x)) {
		size = atoi(x);
		if (size < 0)
			size = 0;
		if (size > MAX_PKT_LENGTH - PING_HDR_LENGTH)
			size = MAX_PKT_LENGTH - PING_HDR_LENGTH;
	}

	if (cmd_sw_isset(cmd, "-f")) {
		flood = 1;
		forever = 1;
	}

	if (cmd_sw_isset(cmd, "-x"))
		exitonerror = 1;

	if (cmd_sw_value(cmd, "-c", &x)) {
		count = atoi(x);
		ttlcount = count;
		forever = 0;
	}

	if (cmd_sw_isset(cmd, "-A"))
		noabort = 1;

	if (cmd_sw_isset(cmd, "-E"))
		needexact = 1;

	if (isdigit(*host)) {
		if (parseipaddr(host, hostaddr) < 0) {
			xprintf("Invalid IP address: %s\n", host);
			return -1;
		}
	} else {
		res = dns_lookup(host, hostaddr);
		if (res < 0) {
			return ui_showerror(res,
					    "Could not resolve IP address of host %s",
					    host);
		}
	}

	sprintf_ip(hostip, hostaddr);

	if (forever)
		xprintf("Press ENTER to stop pinging\n");

	do {
		res = icmp_ping(hostaddr, seq, size);

		if (res < 0) {
			xprintf("Could not transmit echo request\n");
			return BOLT_ERR_IOERR;
		} else if (res == 0) {
			xprintf("%s (%s) is not responding (seq=%d)\n", host,
				hostip, seq);
			if (exitonerror)
				return BOLT_ERR_TIMEOUT;
		} else {
			countreturned++;
			if (!flood || ((seq % 10000) == 0)) {
				if (forever || (ttlcount > 1)) {
					xprintf("%s (%s) is alive (seq=%d)\n",
						host, hostip, seq);
				} else
					xprintf("%s (%s) is alive\n", host,
						hostip);
			}
		}

		if ((forever || (count > 1)) && !flood) {
			if (res > 0)
				bolt_sleep(BOLT_HZ);
		}

		seq++;
		count--;

	} while ((forever || (count > 0)) && (noabort || !console_status()));

	xprintf("%s (%s): %d packets sent, %d received\n", host, hostip,
		ttlcount - count, countreturned);
	return needexact ? (countreturned != ttlcount) : (countreturned == 0);
}
#endif

#if CFG_ENET
struct phy_intf {
	int   index;
	char *description;
	char *phy_type;
	char *mdio_mode;
	char *phy_speed;
	char *phy_id;
};

static inline int phy_intf_is_default(const struct phy_intf *phy,
				      const struct enet_params *e)
{
	return !enet_params_cmp(e, phy->phy_type, phy->mdio_mode,
				phy->phy_speed, phy->phy_id);
}

static inline int phy_intf_is_current(const struct phy_intf *phy, int instance)
{
	char buffer[255];
	const char *val;
	int res[4] = { 1, 1, 1, 1 };

	xsprintf(buffer, "ETH%d_PHY", instance);
	val = env_getenv(buffer);
	if (val)
		res[0] = strcmp(phy->phy_type, val);

	xsprintf(buffer, "ETH%d_MDIO_MODE", instance);
	val = env_getenv(buffer);
	if (val)
		res[1] = strcmp(phy->mdio_mode, val);

	xsprintf(buffer, "ETH%d_SPEED", instance);
	val = env_getenv(buffer);
	if (val)
		res[2] = strcmp(phy->phy_speed, val);

	xsprintf(buffer, "ETH%d_PHYADDR", instance);
	val = env_getenv(buffer);
	if (val)
		res[3] = strcmp(phy->phy_id, val);

	return !(res[0] || res[1] || res[2] || res[3]);
}

static const struct phy_intf phy_options[] = {
	/*	phy_type                mdio_mode       phy_speed       phy_id/(phyaddr) */
#if CONFIG_BRCM_GENET_VERSION < 4
	{1, "Internal 10/100Mbps PHY (built-in PHY)",
		"INT",                  "1",            "100",          "1"},
#endif

	{2, "MII to external 10/100Mbps PHY (e.g. bcm53101E)",
		"MII",                  "1",            "100",          "probe"},

	{3, "RGMII ID mode disabled for external 10/100/1000Mbps PHY (e.g. bcm5461x)",
		"RGMII_NO_ID",          "1",            "1000",         "probe"},

	{4, "RGMII to external Gigabit Switch (e.g. bcm531x5)",
		"RGMII",                "boot",         "1000",         "30"},

	{5, "RGMII ID mode disabled for external Gigabit Switch",
		"RGMII_NO_ID",          "boot",         "1000",         "30"},

	{6, "RGMII to RGMII (e.g. STB<->3383)",
		"RGMII",                "0",            "1000",         "noprobe"},

#if CONFIG_BRCM_GENET_VERSION == 4
	{7, "Internal GPHY (built-in PHY)",
		"INT",                  "1",            "1000",         __stringify(INT_PHY_ADDR) },
#endif
#if CONFIG_BRCM_GENET_VERSION == 5
	{7, "Internal EPHY (built-in PHY)",
		"INT",                  "1",            "100",         __stringify(INT_PHY_ADDR) },
#endif
};

static int phy_intf_save_env(int instance, const struct phy_intf *phy_opt)
{
	char buffer[255];
	int errcode;

	xsprintf(buffer, "ETH%d_PHY", instance);
	errcode = env_setenv(buffer, phy_opt->phy_type, ENV_FLG_NORMAL);
	if (errcode)
		return errcode;

	xsprintf(buffer, "ETH%d_MDIO_MODE", instance);
	errcode = env_setenv(buffer, phy_opt->mdio_mode, ENV_FLG_NORMAL);
	if (errcode)
		return errcode;

	xsprintf(buffer, "ETH%d_SPEED", instance);
	errcode = env_setenv(buffer, phy_opt->phy_speed, ENV_FLG_NORMAL);
	if (errcode)
		return errcode;

	xsprintf(buffer, "ETH%d_PHYADDR", instance);
	errcode = env_setenv(buffer, phy_opt->phy_id, ENV_FLG_NORMAL);
	if (errcode)
		return errcode;

	return env_save();
}

static void phy_intf_reset_env(int instance)
{
	char buffer[255];
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(env_vars); i++) {
		xsprintf(buffer, "ETH%d_%s", instance, env_vars[i]);
		env_delenv(buffer);
	}
}

/*
 * New variables exported to Linux:
 *
 * ETH0_PHY: optional
 *  INT (default) - internal PHY
 *  MII - 10/100 MII (18-wire interface)
 *  RGMII - 14-wire RGMII interface, may run at 25 MHz or 125 MHz
 *  RGMII_NO_ID - same as above, but disable phase shift on TXC clock
 *
 * ETH0_MDIO_MODE: optional
 *  0 - standard MDIO is not available (3383)
 *  1 (default) - standard MDIO is available (ethernet PHY)
 *  boot - BOLT configures it (ethernet switch)
 *
 * ETH0_SPEED: optional
 *  100
 *  1000 (default)
 */
static int ui_cmd_ephy_config(ui_cmdline_t *cmd, int argc, char *argv[])
{
	unsigned int i, j, k = 0;
	char buffer[128];
	struct phy_intf *phy_opt;
	const enet_params *e;
	unsigned int params = 0;
	unsigned int num_enet, enet, enet_start = 0;
	const char *x;
	int instance = -1;
	unsigned int expert = 0;
	unsigned int reset = 0;
	int choice = 0;
	unsigned int choice_array[(NUM_ENET + 1)][ARRAY_SIZE(phy_options)];

	if (CFG_SYSTEMPORT)
		return BOLT_ERR_INV_COMMAND;

	if (cmd_sw_value(cmd, "-i", &x))
		instance = atoi(x);

	if (cmd_sw_isset(cmd, "-e"))
		expert = 1;

	if (cmd_sw_isset(cmd, "-r"))
		reset = 1;

	if (instance == -1) {
		enet_start = 0;
		num_enet = board_num_enet();
	} else {
		enet_start = instance;
		num_enet = instance + 1;
	}

	memset(choice_array, 0, sizeof(choice_array));

	for (enet = enet_start; enet < num_enet; enet++) {
		e = board_enet(enet);
		if (!e)
			continue;

		if (reset) {
			phy_intf_reset_env(enet);
			continue;
		}

		/* We can't offer anything interesting on MoCA */
		if (strcmp(e->phy_type, "MOCA") == 0 && !expert)
			continue;

		for (i = 0; i < ARRAY_SIZE(phy_options); i++) {
			if (phy_intf_is_default(&phy_options[i], e))
				params = 1;
			else if (phy_intf_is_current(&phy_options[i], enet))
				params = 2;
			else
				params = 0;

			if (expert || params) {
				choice++;
				choice_array[enet][i] = choice;
				xprintf("%d) %s on GENET_%d %s\n",
					choice,
					phy_options[i].description,
					enet,
					help_str[params]);
			}
		}
		xprintf("\n");
	}

	if (reset) {
		env_save();
		board_init_enet();
		return 0;
	}

	if (choice == 0) {
		xprintf("Enable expert mode for more options\n");
		return 0;
	}

	console_readline("Selection: ", buffer, sizeof(buffer));
	i = atoi(buffer);
	if (i <= 0) {
		xprintf("ERROR: invalid selection\n");
		return BOLT_ERR_INV_PARAM;
	}

	choice = 0;
	for (j = enet_start; j < num_enet; j++) {
		for (k = 0; k < ARRAY_SIZE(phy_options); k++) {
			if (i == choice_array[j][k]) {
				enet = j;
				i = k;
				choice = 1;
				break;
			}
		}
	}

	if (choice == 0) {
		xprintf("ERROR: invalid selection\n");
		return BOLT_ERR_INV_PARAM;
	}

	if (i < ARRAY_SIZE(phy_options)) {
		int ret;

		phy_opt = (struct phy_intf *)&phy_options[i];
		ret = phy_intf_save_env(enet, phy_opt);
		if (ret != 0) {
			xprintf("ERROR: can't save env %s\n",
				bolt_errortext(ret));
			return ret;
		}
		board_init_enet();
	} else {
		xprintf("ERROR: invalid selection\n");
		return BOLT_ERR_INV_PARAM;
	}
	return 0;
}

static int ui_cmd_mii_read_write(ui_cmdline_t *cmd, int argc, char *argv[])
{
	unsigned int do_read = 0, do_write = 0;
	char *op, *device, *phy, *reg, *value = NULL;
	mdio_info_t *mdio;
	int res = BOLT_ERR_INV_PARAM;

	op = cmd_getarg(cmd, 0);
	if (!op)
		return BOLT_ERR_INV_PARAM;

	if (!strcmp(op, "read"))
		do_read = 1;
	else if (!strcmp(op, "write"))
		do_write = 1;
	else {
		ui_showerror(res, "invalid operation \"%s\"", op);
		return res;
	}

	device = cmd_getarg(cmd, 1);
	if (!device)
		return BOLT_ERR_INV_PARAM;

	phy = cmd_getarg(cmd, 2);
	if (!phy)
		return BOLT_ERR_INV_PARAM;

	reg = cmd_getarg(cmd, 3);
	if (!reg)
		return BOLT_ERR_INV_PARAM;

	if (do_write) {
		value = cmd_getarg(cmd, 4);
		if (!value)
			return BOLT_ERR_INV_PARAM;
	}

	mdio = mdio_init(device);
	if (!mdio) {
		ui_showerror(res, "could not get mdio context for %s", device);
		return res;
	}

	if (do_read)
		res = mdio_read(mdio, atoi(phy), atoi(reg));
	if (do_write)
		res = mdio_write(mdio, atoi(phy), atoi(reg), atoi(value));

	if (res < 0) {
		ui_showerror(res, "MII %s failed, phy=%s, reg=%s", op, phy,
				reg);
		return res;
	}

	xprintf("MII %s, phy=%s, reg=%s, value=0x%04x\n", op, phy, reg,
			do_read ? (uint16_t)res : (uint16_t)atoi(value));

	return 0;
}

#if CFG_SYSTEMPORT
static int ui_cmd_switch_config(ui_cmdline_t *cmd, int argc, char *argv[])
{
	enet_params e;
	const char *port, *mask;
	unsigned int port_mask;

	memset(&e, 0, sizeof(e));

	if (!cmd_sw_value(cmd, "-port", &port))
		return BOLT_ERR_INV_PARAM;

	if (!cmd_sw_value(cmd, "-isolate_mask", &mask))
		return BOLT_ERR_INV_PARAM;

	if (!port || !mask)
		return BOLT_ERR_INV_PARAM;

	e.switch_port = atoi(port);
	if (e.switch_port > 8)
		return BOLT_ERR_INV_PARAM;

	port_mask = xtoi(mask);

	/* Re-configure the switch with the new isolation portmask */
	sf2_port_isolate_setup(&e, port_mask);

	return 0;

}
#endif /* CFG_SYSTEMPORT */
#endif /* CFG_ENET */

#endif /* CFG_NETWORK */
