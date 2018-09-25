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
 *
 */


#ifndef INCLUDED_IEEE802_11_SOFT_FRAME_EQUALIZER_DC_H
#define INCLUDED_IEEE802_11_SOFT_FRAME_EQUALIZER_DC_H

#include <ieee802-11/api.h>
#include <gnuradio/block.h>

enum Equalizer_soft_dc {
	LS_sd   = 0,
	LMS_sd  = 1,
	COMB_sd = 2,
	STA_sd  = 3,
};

namespace gr {
namespace ieee802_11 {

class IEEE802_11_API soft_frame_equalizer_dc : virtual public gr::block
{

public:
	typedef boost::shared_ptr<soft_frame_equalizer_dc> sptr;
	static sptr make(Equalizer_soft_dc algo, double freq, double bw, int scaling, int threshold,
			bool log, bool debug);
	virtual void set_algorithm(Equalizer_soft_dc algo) = 0;
	virtual void set_bandwidth(double bw) = 0;
	virtual void set_frequency(double freq) = 0;
};

} // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_SOFT_FRAME_EQUALIZER_DC_H */
