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

#ifndef INCLUDED_IEEE802_11_EQUALIZER_LS_SBMRC_H
#define INCLUDED_IEEE802_11_EQUALIZER_LS_SBMRC_H

#include "base_sbmrc.h"
#include <vector>

namespace gr {
namespace ieee802_11 {
namespace equalizer_sbmrc {

class ls_sbmrc: public base_sbmrc {
public:
	virtual void equalize_sbmrc(gr_complex *in, gr_complex *in_1, int n, gr_complex *symbols, gr_complex *symbols_oai, gr_complex *symbols_1, gr_complex *symbols_oai_1, float *noise_vec, int scaling, int threshold, uint8_t *bits, float *llr_out, float *llr_b1, float *llr_b2, float *llr_sbmrc,  boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols, int llr_type);

	//virtual void equalize_soft_dc(gr_complex *in, gr_complex *in_1, int n, gr_complex *symbols, gr_complex *symbols_1, gr_complex *symbols_oai, gr_complex *symbols_oai_1, float *noise_vec, float *noise_vec_1, int scaling, int threshold, uint8_t *bits, uint8_t *bits_1, float *llr, float *llr_1, boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols);

	virtual double get_snr_sbmrc();
private:
	gr_complex d_H_soft[64];
	gr_complex d_H_soft_1[64];
	float d_N_soft_loc[64];
	float d_N_soft_loc_1[64];
	float d_N_soft_conv[64];
	float d_N_soft_conv_1[64];
	//double CSI[64] = {};
	double d_H_Var[64] = {}; // normalized channel
	double d_H_Var_1[64] = {}; // normalized channel
	//float d_threshold = 3; // configurable param according to noise level 
	double d_snr_soft;
	int d_interference = 0; // interference present or not	
	float d_NLR = 0;
	float d_NLR_1 = 0;
        double d_temp = 0;
        double d_temp_1 = 0;
        double temp_symbols[48] = {};
        double temp_symbols_1[48] = {};
	
};

} /* namespace channel_estimation */
} /* namespace ieee802_11 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_11_EQUALIZER_LS_SBMRC_H */

