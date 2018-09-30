/*
 * Copyright (C) 2015 Bastian Bloessl <bloessl@ccs-labs.org>
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

#ifndef INCLUDED_IEEE802_11_EQUALIZER_BASE_DC_H
#define INCLUDED_IEEE802_11_EQUALIZER_BASE_DC_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
namespace ieee802_11 {
namespace equalizer_soft_dc { // my own namespace

class base_soft_dc { // my own base equalizer abstract class 
public:

virtual ~base_soft_dc() {}; 

virtual void equalize_soft_dc(gr_complex *in, gr_complex *in_1, int n, gr_complex *symbols, gr_complex *symbols_1, gr_complex *symbols_oai, gr_complex *symbols_oai_1, float *noise_vec, float *noise_vec_1, int scaling, int threshold, float *llr, boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols) = 0; // my own base class method, have to be implemented in the derieved class

virtual double get_snr_soft_dc() = 0;

static const gr_complex POLARITY_soft_dc[127];

protected:
	static const gr_complex LONG_soft_dc[64];
};

} /* namespace channel_estimation */
} /* namespace ieee802_11 */
} /* namespace gr */

#endif /* INCLUDED_IEEE802_11_EQUALIZER_BASE_DC_H */
