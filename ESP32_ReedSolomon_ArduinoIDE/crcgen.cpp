/*****************************
 * Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
 *
 * ... license unchanged ...
 *
 * CRC-CCITT generator simulator for byte wide data.
 *
 * CRC-CCITT = x^16 + x^12 + x^5 + 1
 *
 ******************************/

#include <Arduino.h>
#include "ecc.h"

// Forward declaration (matches definition below)
rs_uint16_t crchware(rs_uint16_t data, rs_uint16_t genpoly, rs_uint16_t accum);

/* Computes the CRC-CCITT checksum on array of byte data, length len */
rs_uint16_t crc_ccitt(unsigned char *msg, int len) {
	int i;
	rs_uint16_t acc = 0;

	for (i = 0; i < len; i++) {
		acc = crchware((rs_uint16_t)msg[i], (rs_uint16_t)0x1021, acc);
	}

	return (acc);
}

/* models crc hardware (minor variation on polynomial division algorithm) */
rs_uint16_t crchware(rs_uint16_t data, rs_uint16_t genpoly, rs_uint16_t accum) {
	static rs_uint16_t i;
	data <<= 8;
	for (i = 8; i > 0; i--) {
		if ((data ^ accum) & 0x8000)
			accum = ((accum << 1) ^ genpoly) & 0xFFFF;
		else
			accum = (accum << 1) & 0xFFFF;
		data = (data << 1) & 0xFFFF;
	}
	return (accum);
}