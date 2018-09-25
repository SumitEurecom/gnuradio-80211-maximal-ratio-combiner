/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
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
#include <ieee802-11/sync_short_dc_sep_corr.h>
#include <gnuradio/io_signature.h>
#include "utils.h"

#include <iostream>

using namespace gr::ieee802_11;

static const int MIN_GAP = 480;
static const int MAX_SAMPLES = 540 * 80;
static int ios1[] = {sizeof(gr_complex), sizeof(gr_complex),
sizeof(gr_complex), sizeof(gr_complex), sizeof(float)};
static std::vector<int> iosig1(ios1, ios1+sizeof(ios1)/sizeof(int));
class sync_short_dc_sep_corr_impl : public sync_short_dc_sep_corr {

public:
sync_short_dc_sep_corr_impl(double threshold, unsigned int min_plateau, bool log, bool debug) :
		block("sync_short_dc",
			//gr::io_signature::makev(5, 5, get_input_sizes()),
			gr::io_signature::makev(6, 6, iosig1),
			gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(gr_complex))),
		d_log(log),
		d_debug(debug),
		d_state(SEARCH),
		d_plateau(0),
		d_freq_offset(0),
		d_copied(0),
		MIN_PLATEAU(min_plateau),
		d_threshold(threshold) {

	set_tag_propagation_policy(block::TPP_DONT);
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];
	const gr_complex *in_1 = (const gr_complex*)input_items[1];
	const gr_complex *in_abs = (const gr_complex*)input_items[2];
	const gr_complex *in_abs_1 = (const gr_complex*)input_items[3];
	const float *in_cor = (const float*)input_items[4];
	const float *in_cor_1 = (const float*)input_items[5];
	gr_complex *out = (gr_complex*)output_items[0];
	gr_complex *out_1 = (gr_complex*)output_items[1];

int noutput = noutput_items;
int ninput = std::min(std::min(ninput_items[2] ,std::min(ninput_items[0], ninput_items[1])), std::min(std::min(ninput_items[4], ninput_items[3]), ninput_items[5]));
	dout << "SHORT noutput : " << noutput << " ninput: " << ninput_items[0] << std::endl;

	switch(d_state) {

	case SEARCH: {
		int i;

		for(i = 0; i < ninput; i++) {
// Add the two separate correlation values here and then compare against the threshold 
			if((in_cor[i]+in_cor_1[i]) > d_threshold) {
				if(d_plateau < MIN_PLATEAU) {
					d_plateau++;

				} else {
					d_state = COPY;
					d_copied = 0;
					d_freq_offset = arg(in_abs[i]) / 16;
					d_freq_offset_1 = arg(in_abs_1[i]) / 16;
					d_plateau = 0;
		                        insert_tag(nitems_written(0), d_freq_offset, d_freq_offset_1, nitems_read(0) + i);
					dout << "SHORT Frame!" << std::endl;
					break;
				}
			} else {
				d_plateau = 0;
			}
		}

		consume_each(i); // Tell the scheduler how_many_items were consumed on each input stream.


		return 0;
	}

	case COPY: {

		int o = 0;
		while( o < ninput && o < noutput && d_copied < MAX_SAMPLES) {
			if((in_cor[o]+in_cor_1[o]) > d_threshold) {
				if(d_plateau < MIN_PLATEAU) {
					d_plateau++;

				// there's another frame
				} else if(d_copied > MIN_GAP) {
					d_copied = 0;
					d_plateau = 0;
					d_freq_offset = arg(in_abs[o]) / 16;
					d_freq_offset_1 = arg(in_abs_1[o]) / 16;
					insert_tag(nitems_written(0) + o, d_freq_offset, d_freq_offset_1, nitems_read(0) + o);
					dout << "SHORT Frame!" << std::endl;
					break;
				}

			} else {
				d_plateau = 0;
			}

			out[o] = in[o] * exp(gr_complex(0, -d_freq_offset * d_copied));
			out_1[o] = in_1[o] * exp(gr_complex(0, -d_freq_offset_1 * d_copied));
			o++;
			d_copied++;
		}

		if(d_copied == MAX_SAMPLES) {
			d_state = SEARCH;
		}

		dout << "SHORT copied " << o << std::endl;

		consume_each(o);
		return o;
	}
	}

	throw std::runtime_error("sync short: unknown state");
	return 0;
}

void insert_tag(uint64_t item, double freq_offset, double freq_offset_1, uint64_t input_item) {
	mylog(boost::format("frame start at in: %2% out: %1%") % item % input_item);

	const pmt::pmt_t key = pmt::string_to_symbol("wifi_start");
	const pmt::pmt_t value = pmt::from_double(freq_offset);
	const pmt::pmt_t value_1 = pmt::from_double(freq_offset_1);
	const pmt::pmt_t srcid = pmt::string_to_symbol(name());
	add_item_tag(0, item, key, value, srcid);
	add_item_tag(1, item, key, value_1, srcid);
}

static std::vector<int> get_input_sizes(){
    std::vector<int> input_sizes;
    input_sizes.push_back(sizeof(gr_complex));
    input_sizes.push_back(sizeof(gr_complex));
    input_sizes.push_back(sizeof(gr_complex));
    input_sizes.push_back(sizeof(gr_complex));
    input_sizes.push_back(sizeof(float));

    return input_sizes;
    }
private:
	enum {SEARCH, COPY} d_state;
	int d_copied;
	int d_plateau;
	float d_freq_offset, d_freq_offset_1;
	const double d_threshold;
	const bool d_log;
	const bool d_debug;
	const unsigned int MIN_PLATEAU;
};

sync_short_dc_sep_corr::sptr
sync_short_dc_sep_corr::make(double threshold, unsigned int min_plateau, bool log, bool debug) {
	return gnuradio::get_initial_sptr(new sync_short_dc_sep_corr_impl(threshold, min_plateau, log, debug));
}
