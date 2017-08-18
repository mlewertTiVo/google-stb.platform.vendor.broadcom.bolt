/***************************************************************************
 * Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#include "lib_types.h"
#include "lib_printf.h"

#include "common.h"
#include "avs_bsu.h"
#include "timer.h"
#include "arch_ops.h"
#include "avs_bsu.h"
#if CFG_SPLASH
#include "splash-api.h"
#include "splash-media.h"
#endif
#include "board.h"


/* Set the over temperature alarm hysteresis.
* You *MUST* check with Broadcom if you wish to
* use alternate values.
*/
#ifndef OT_ALARM_HYST_HI
#define OT_ALARM_HYST_HI 125
#endif

#ifndef OT_ALARM_HYST_LO
#define OT_ALARM_HYST_LO 110
#endif


/* Poll for the current chip temperature every N seconds. */
#define OT_POLL_INTERVAL 2

#define OT_ALARM_PROMPT "["ANSI_RED"OVERTEMP ALARM %03d"ANSI_RESET"] BOLT> "
#define FROM_FW_TEMP(X) ((X) / 1000) /* 1000th degrees C */


static bolt_timer_t ovtimer;
static int otalarm;
static char otmsg[40] = {'\0'};


static void ot_splash_image_change(void)
{
#if CFG_SPLASH
	struct SplashMediaInfo *smi;
	enum SplashMediaType smt;
	struct splash_rgb rgb = {0, 0, 255};

	smt = (otalarm) ? SplashMediaType_eOverTemp : SplashMediaType_eBoot;

	smi = splash_open_media(smt, SplashMediaFormat_eBmp, NULL);
	if (smi == NULL || smi->buf == NULL) {
		err_msg("splash: failed to open media %d", (int)smt);
		return;
	}

	splash_api_replace_bmp(smi->buf, &rgb);
#endif
}


/**********************************************************************
  *  overtemp_check()
  *
  *  This is the core of the SSBL over temperature 'park' and monitoring
  * feature. At every call the temperature is checked and if we go under
  * or over thermal limits an actions are taken at the point of
  * transition.
  *
  * At the moment these action are:
  *  1. Change the cpu speed (to limit power dissipation.)
  *  2. Inform the user via console messages/changing the default prompt.
  *
  *  Input parameters:
  *	 nothing
  *
  *  Return value:
  *	 nothing
  ********************************************************************* */
static void overtemp_check(void)
{
	int rc;
	unsigned int voltage;
	signed int temperature;
	bool fw_running;

	rc = avs_get_data(&voltage, &temperature, &fw_running);
	if (!rc && voltage) {
		temperature = FROM_FW_TEMP(temperature);
#if (CFG_CMD_LEVEL >= 5)
		xprintf("[chip temperature: %dC]\n", temperature);
#endif
		if (!otalarm && (temperature >= OT_ALARM_HYST_HI)) {
			err_msg("[OVERTEMP ALARM %03d]", temperature);
			sprintf(otmsg, OT_ALARM_PROMPT, temperature);
			arch_set_cpu_clk_ratio(CPU_CLK_RATIO_QUARTER);
			otalarm = 1;
			ot_splash_image_change();
		} else if (otalarm && (temperature <= OT_ALARM_HYST_LO)) {
			info_msg("[OVERTEMP ALARM OFF %03d]", temperature);
			arch_set_cpu_clk_ratio(CPU_CLK_RATIO_ONE);
			otalarm = 0;
			ot_splash_image_change();
		}
	} else
		err_msg("AVS: temperature monitoring disrupted");
}


/**********************************************************************
  *  overtemp_task()
  *
  *  If enabled, this (polled) task is always running in BOLT to take
  * chip temperature measurements at OT_POLL_INTERVAL seconds.
  *
  *  Input parameters:
  *	 arg: this value is not used.
  *
  *  Return value:
  *	 nothing
  ********************************************************************* */
static void overtemp_task(void *arg)
{
	if (!TIMER_EXPIRED(ovtimer))
		return;

	overtemp_check();

	TIMER_SET(ovtimer, OT_POLL_INTERVAL * BOLT_HZ);
}


/**********************************************************************
  *  overtemp_park()
  *
  *  As long as the chip is in an over temp condition we will never
  * leave this function. Its task is to block further BOLT activity
  * until normal temperature conditions return. Its should be called
  * early enough in the startup cycle to minimize the number of
  * peripherals being initalized and activated (sources of heat.)
  *
  *  Input parameters:
  *	 nothing
  *
  *  Return value:
  *	 nothing
  ********************************************************************* */
static void overtemp_park(void)
{
	xprintf("AVS: park check");

	do {
		overtemp_check();

		if (otalarm) {
			/* Disable reboot on overtemp as we
			could go overtemp again (reboot) and
			up here in an infinite cycle of fail. */
			avs_do_reset_on_overtemp(0);
			xprintf(".");
			/* sleep also POLL()'s peripherals */
			bolt_msleep(OT_POLL_INTERVAL * 1000);
		}

	} while (otalarm);

	/* re-enable the overtemp reset now that we are at normal
	* temperature and we won't get caught in a reboot loop.
	*/
	avs_do_reset_on_overtemp(1);
	xprintf("\n");
}


/* API --------------------------------------------------------------------- */


/**********************************************************************
  *  bolt_overtemp_init()
  *
  *  Setup the overtemp monitor task. Before beginning the task the
  * hardware overtemp feature is checked and a park check is done
  * before progressing.
  *
  *  Input parameters:
  *	 nothing
  *
  *  Return value:
  *	 nothing
  ********************************************************************* */
void bolt_overtemp_init(void)
{
	int rc;
	unsigned int voltage;
	signed int temperature;
	bool fw_running;
	struct fsbl_info *inf = board_info();

	if (inf && (inf->saved_board.hardflags & FSBL_HARDFLAG_OTPARK_MASK)) {
		warn_msg("AVS: park/overtemp check disabled");
		return;
	}

	/* Check if this AVS feature is functional. */
	rc = avs_get_data(&voltage, &temperature, &fw_running);
	if (rc || (voltage == 0)) {
		warn_msg("AVS: temperature will not be monitored");
		return;
	}

	overtemp_park();

	/* Enable overtemp_task()
	*/
	TIMER_SET(ovtimer, OT_POLL_INTERVAL * BOLT_HZ);
	bolt_bg_add(overtemp_task, NULL);

	xprintf("AVS: temperature monitoring enabled\n");
}


/**********************************************************************
  *  bolt_overtemp_prompt()
  *
  *  If we are in an overtemp condition then return the message
  * overtemp_check() generates for the user command line prompt.
  *  If at normal operating temperature then return a NULL to signal
  * that some other (default BOLT) prompt must be used.
  *
  *  Input parameters:
  *	 nothing
  *
  *  Return value:
  *	 c-string or NULL
  ********************************************************************* */
char *bolt_overtemp_prompt(void)
{
	return (otalarm) ? otmsg : NULL;
}
