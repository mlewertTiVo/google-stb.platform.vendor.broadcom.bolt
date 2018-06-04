/***************************************************************************
 *     Copyright (c) 2015, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************/

#ifndef __OVERTEMP_H__
#define __OVERTEMP_H__

/* Maximum supported temperature allowed */
#define OVERTEMP_MAX_TEMPERATURE 125 /* degrees C */

/* Allow this to be over-ridden with compile time define */
#ifdef AVS_MAX_TEMPERATURE
#undef OVERTEMP_MAX_TEMPERATURE
#define OVERTEMP_MAX_TEMPERATURE AVS_MAX_TEMPERATURE
#endif

/* Set high and low degrees of over temperature parking.
 * You *MUST* check with Broadcom if you wish to
 * use alternate values.
 */
#ifndef OVERTEMP_ALARM_PARK_HIGH
#define OVERTEMP_ALARM_PARK_HIGH 120
#endif

#ifndef OVERTEMP_ALARM_PARK_LOW
#define OVERTEMP_ALARM_PARK_LOW 110
#endif

/* Check the AON scratch area for magic number and proper values for handling
 * an overtemp reset.  The format of the AON scratch area looks like this:
 *
 * 31:24 - unused
 * 23:16 - parking high
 * 15:08 - parking low
 * 07:00 - overtemp threshold
 */
static bool overtemp_are_validparams(uint32_t params)
{
	uint8_t temp_thresh, park_high, park_low;

	park_high = params >> 16;
	park_low = params >> 8;
	temp_thresh = params;

	if (temp_thresh == 0)
		return false;
	if (temp_thresh > OVERTEMP_MAX_TEMPERATURE)
		return false;
	if (park_high > temp_thresh)
		return false;
	if (park_low > park_high)
		return false;

	return true;
}

static inline uint8_t overtemp_get_threshold(uint32_t params)
{
	if (overtemp_are_validparams(params))
		return params;

	return OVERTEMP_MAX_TEMPERATURE;
}

static inline void overtemp_get_parkhighlow(uint32_t params, uint8_t *high,
	uint8_t *low)
{
	if (overtemp_are_validparams(params)) {
		*high = params >> 16;
		*low = params >> 8;
		return;
	}

	*high = OVERTEMP_ALARM_PARK_HIGH;
	*low = OVERTEMP_ALARM_PARK_LOW;
}

void bolt_overtemp_init(void);
char *bolt_overtemp_prompt(void);

#endif /* __OVERTEMP_H__ */

