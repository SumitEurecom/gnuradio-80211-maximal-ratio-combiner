/*
 * Copyright (C) 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ls_soft.h"
#include <cstring>
#include <iostream>

using namespace gr::ieee802_11::equalizer_soft;

void ls_soft::equalize_soft(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, float *llr, boost::shared_ptr<gr::digital::constellation> mod_soft) {

	if(n == 0) {
		std::memcpy(d_H_soft, in, 64 * sizeof(gr_complex)); // first lts copied in d_H

	} else if(n == 1) {
		double signal = 0;
		double noise = 0;
		for(int i = 0; i < 64; i++) {
			if((i == 32) || (i < 6) || ( i > 58)) { // skip null padded subs and the dc 
				continue;
			}
			noise += std::pow(std::abs(d_H_soft[i] - in[i]), 2);
			signal += std::pow(std::abs(d_H_soft[i] + in[i]), 2);
			d_H_soft[i] += in[i]; // add d_H with in array i.e. second lts 
			d_H_soft[i] /= LONG_soft[i] * gr_complex(2, 0); // channel estimation for current frame
		}

		d_snr_soft = 10 * std::log10(signal / noise / 2);

	} else { // from n = 3 onwards, data symbols are there

		int c = 0;
		for(int i = 0; i < 64; i++) {
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) { // skip the pilots, zero padded subs and the dc, only equalize 48 daya syms 
				continue;
			} else {
				symbols[c] = in[i] / d_H_soft[i]; // equalize them with chest d_H
				bits[c] = mod_soft->decision_maker(&symbols[c]);
				llr[c] = -4*real(symbols[c]);
				//std::cout << (unsigned int)bits[c] << "--" << symbols[c] << std::endl;
                                c++;
			}
		}
	}
}

double ls_soft::get_snr_soft() {
	return d_snr_soft;
}
