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

#ifndef INCLUDED_IEEE802_11_EQUALIZER_LS_H
#define INCLUDED_IEEE802_11_EQUALIZER_LS_H

#include "base_soft.h"
#include <vector>

namespace gr {
namespace ieee802_11 {
namespace equalizer_soft {

class ls_soft: public base_soft {
public:
	virtual void equalize_soft(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, float *llr, boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols);
	virtual double get_snr_soft();
private:
	gr_complex d_H_soft[64];
	float d_N_soft_loc[64];
	float d_N_soft_conv[64];
	float d_threshold = 9;
	double d_snr_soft;
	int d_interference = 0; // interference present or not	
	int imt = 0;
	float d_NLR = 0;
	float d_NLR2 = 0;
	
};

} /* namespace channel_estimation */
} /* namespace ieee802_11 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_11_EQUALIZER_LS_H */

