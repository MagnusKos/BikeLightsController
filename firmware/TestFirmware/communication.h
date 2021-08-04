#pragma once

/* "From device" buffer structure:
 * | SIG | H | L | R | S | R | D | 
 * | 1 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 * SIG - 2 bytes, signature
 * H - 1 byte, high beam PWM
 * L - 1 byte, low beam PWM
 * R - 1 pyte, rear lights PWM
 * S - 1 byte, state bits
 * R - 1 byte, readings bits
 * D - 1 byte, some debug data, dunno yet
 * 
 */

#define COM_TX_SIG_MSB  0
#define COM_TX_SIG_LSB  1
#define COM_TX_HIGH     2
#define COM_TX_LOW      3
#define COM_TX_REAR     4
#define COM_TX_STATE    5
#define COM_TX_READINGS 6
#define COM_TX_DBG      7

#define COM_SIG_MSB  0x91
#define COM_SIG_LSB  0x92

#define COM_BUFF_SIZE   8
