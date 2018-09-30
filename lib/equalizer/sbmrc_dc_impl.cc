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

#include "sbmrc_dc_impl.h"
#include "equalizer/base_soft_dc.h"
//#include "equalizer/comb.h"
//#include "equalizer/lms.h"
#include "equalizer/ls_soft_dc.h"
//#include "equalizer/sta.h"
#include "utils.h"
#include <gnuradio/io_signature.h>
//#define llr_out_from_equalizer
//#define s_decode_signal_field
namespace gr {
namespace ieee802_11 {

sbmrc_dc::sptr
sbmrc_dc::make(Equalizer_sbmrc algo, double freq, double bw, int scaling, int threshold, bool log, bool debug) {
	return gnuradio::get_initial_sptr
		(new sbmrc_dc_impl(algo, freq, bw, scaling, threshold, log, debug));
}

//static int ios1[] = {48, 48*sizeof(float), 48, 48*sizeof(float)};
//static std::vector<int> iosig1(ios1, ios1+sizeof(ios1)/sizeof(int));


sbmrc_dc_impl::sbmrc_dc_impl(Equalizer_sbmrc algo, double freq, double bw, int scaling, int threshold, bool log, bool debug) :
	gr::block("sbmrc_dc",
			gr::io_signature::make2(2, 2, 64 * sizeof(gr_complex), 64 * sizeof(gr_complex)),
			gr::io_signature::make(1, 1, 48*sizeof(float))),
	d_current_symbol(0), d_log(log), d_debug(debug), d_equalizer(NULL),
	d_freq(freq), d_bw(bw), d_scaling(scaling), d_threshold(threshold), d_frame_bytes(0), d_frame_symbols(0),
	d_freq_offset_from_synclong(0.0) {

// only for one branch! 
	message_port_register_out(pmt::mp("symbols")); 
//TODO 	message_port_register_out(pmt::mp("symbols_1")); 
	//message_port_register_out(pmt::mp("noise_vec"));
        //message_port_register_out(pmt::mp("noise_vec_1"));

	d_bpsk  = constellation_bpsk::make(); // constellation object for bpsk of class constellation_bpsk
	d_qpsk  = constellation_qpsk::make(); // constellation object for bpsk of class constellation_qpsk
	d_16qam = constellation_16qam::make(); // constellation object for bpsk of class constellation_16qam
	d_64qam = constellation_64qam::make(); //// constellation object for bpsk of class constellation_64qam

	d_frame_mod = d_bpsk;

	set_tag_propagation_policy(block::TPP_DONT);
	set_algorithm(algo);
}

sbmrc_dc_impl::~sbmrc_dc_impl() {
}


void
sbmrc_dc_impl::set_algorithm(Equalizer_sbmrc algo) {
	gr::thread::scoped_lock lock(d_mutex);
	delete d_equalizer;

	switch(algo) {
/*
	case COMB:
		dout << "Comb" << std::endl;
		d_equalizer = new equalizer::comb();
		break; */
	case LS_sb:
		dout << "LS" << std::endl;
		d_equalizer = new equalizer_soft_dc::ls_soft_dc(); // defined in base_soft.h
		break;
/*	case LMS:
		dout << "LMS" << std::endl;
		d_equalizer = new equalizer::lms();
		break;
	case STA:
		dout << "STA" << std::endl;
		d_equalizer = new equalizer::sta();
		break; */
	default:
		throw std::runtime_error("Algorithm not implemented");
	}
}

void
sbmrc_dc_impl::set_bandwidth(double bw) {
	gr::thread::scoped_lock lock(d_mutex);
	d_bw = bw;
}

void
sbmrc_dc_impl::set_frequency(double freq) {
	gr::thread::scoped_lock lock(d_mutex);
	d_freq = freq;
}

void
sbmrc_dc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required) {
	ninput_items_required[0] = noutput_items;
	ninput_items_required[1] = noutput_items;
}

int
sbmrc_dc_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {

	gr::thread::scoped_lock lock(d_mutex);

	const gr_complex *in   = (const gr_complex *) input_items[0]; // input stream 1 
	const gr_complex *in_1 = (const gr_complex *) input_items[1]; // input stream 2 
	//uint8_t *out = (uint8_t *) output_items[0]; // hd o/p b1
	float *out1            = (float *) output_items[1]; // added llr output
	//uint8_t *pout = (uint8_t *) output_items[2]; // hd o/p b2
	//float *pout1 = (float *) output_items[3]; // sd o/p b2 

	int i = 0;
	int o = 0;

	gr_complex symbols[48]; // equalized symbols b1
	gr_complex current_symbol[64]; // unequalized symbols b1 
	gr_complex symbols_oai[48]; // equalized symbols method OAI b1
	gr_complex symbols_1[48]; // equalized symbols b2
	gr_complex current_symbol_1[64]; // unequalized symbols b2 
	gr_complex symbols_oai_1[48]; // equalized symbols method OAI b2
	float      noise_vec[64]; // noise variance vector b1
	float      noise_vec_1[64]; // noise variance vector b2 

	dout << "FRAME EQUALIZER: input " << ninput_items[0] << "  output " << noutput_items << std::endl;

	while((i < ninput_items[0]) && (o < noutput_items)) { // do this till 64 input items are consumed and 48 output items are outputted

get_tags_in_window(tags,   0, i, i + 1, pmt::string_to_symbol("wifi_start")); // this tag is coming from sync_short_dc.cc b1
get_tags_in_window(tags_1, 1, i, i + 1, pmt::string_to_symbol("wifi_start")); // this tag is coming from sync_short_dc.cc b2

// new frame
	if(tags.size() && tags_1.size()) { // if got the tag wifi_start on both branches, time to decode SIGNAL field
	std::cout << "Got tags on both branches." << std::endl;	
	d_current_symbol = 0; // counter index for parsing the symbols in the current frame
	d_frame_symbols = 0; // it is total no of data ofdm symbols in that frame, its populated after decoding SIGNAL field
	d_frame_mod = d_bpsk; // SIGNAL field is bpsk
	// retrieve freq offset from b-1
	d_freq_offset_from_synclong   = pmt::to_double(tags.front().value)   * d_bw / (2 * M_PI); 
	// retrieve freq offset from b-2
	d_freq_offset_from_synclong_1 = pmt::to_double(tags_1.front().value) * d_bw / (2 * M_PI);

	d_epsilon0   = pmt::to_double(tags.front().value)   * d_bw / (2 * M_PI * d_freq);
	d_epsilon0_1 = pmt::to_double(tags_1.front().value) * d_bw / (2 * M_PI * d_freq);

	d_er = 0;
	d_er_1 = 0;

        dout << "epsilon: " << d_epsilon0 << std::endl;

}

		// not interesting -> skip
	if(d_current_symbol > (d_frame_symbols + 2)) { // d_frame_symbols + lts1 + lts2
		//>std::cout << "skipping -- symInd--" << d_current_symbol << std::endl;
		i++;
		continue;
	}
                //>std::cout << "copy 64 samples to memory -- symInd--" << d_current_symbol << std::endl;
	std::memcpy(current_symbol,   in   + i*64, 64*sizeof(gr_complex));
	std::memcpy(current_symbol_1, in_1 + i*64, 64*sizeof(gr_complex));

		// compensate sampling offset
                //TODO loop unrolling
	for(int i = 0; i < 64; i++) 
		{

current_symbol[i]   *= exp(gr_complex(0, 2*M_PI*d_current_symbol*80*(d_epsilon0   + d_er)  *(i-32)/64));
current_symbol_1[i] *= exp(gr_complex(0, 2*M_PI*d_current_symbol*80*(d_epsilon0_1 + d_er_1)*(i-32)/64));

		}

		gr_complex p = equalizer_soft_dc::base_soft_dc::POLARITY_soft_dc[(d_current_symbol - 2) % 127];
		gr_complex sum = (current_symbol[11] *  p) +
				 (current_symbol[25] *  p) +
				 (current_symbol[39] *  p) +
				 (current_symbol[53] * -p);

		double beta, beta_1;
		if(d_current_symbol < 2) {
			beta = arg(
					current_symbol[11] -
					current_symbol[25] +
					current_symbol[39] +
					current_symbol[53]);
			beta_1 = arg(
					current_symbol_1[11] -
					current_symbol_1[25] +
					current_symbol_1[39] +
					current_symbol_1[53]);

		} else {
			beta = arg(
					(current_symbol[11] *  p) +
					(current_symbol[39] *  p) +
					(current_symbol[25] *  p) +
					(current_symbol[53] * -p));
			beta_1 = arg(
					(current_symbol_1[11] *  p) +
					(current_symbol_1[39] *  p) +
					(current_symbol_1[25] *  p) +
					(current_symbol_1[53] * -p));

		}

		double er = arg(
				(conj(d_prev_pilots[0]) * current_symbol[11] *  p) +
				(conj(d_prev_pilots[1]) * current_symbol[25] *  p) +
				(conj(d_prev_pilots[2]) * current_symbol[39] *  p) +
				(conj(d_prev_pilots[3]) * current_symbol[53] * -p));

		double er_1 = arg(
				(conj(d_prev_pilots_1[0]) * current_symbol_1[11] *  p) +
				(conj(d_prev_pilots_1[1]) * current_symbol_1[25] *  p) +
				(conj(d_prev_pilots_1[2]) * current_symbol_1[39] *  p) +
				(conj(d_prev_pilots_1[3]) * current_symbol_1[53] * -p));

		er   *= d_bw / (2 * M_PI * d_freq * 80);
		er_1 *= d_bw / (2 * M_PI * d_freq * 80);

		d_prev_pilots[0]   = current_symbol[11]   *  p;
		d_prev_pilots[1]   = current_symbol[25]   *  p;
		d_prev_pilots[2]   = current_symbol[39]   *  p;
		d_prev_pilots[3]   = current_symbol[53]   * -p;

		d_prev_pilots_1[0] = current_symbol_1[11] *  p;
		d_prev_pilots_1[1] = current_symbol_1[25] *  p;
		d_prev_pilots_1[2] = current_symbol_1[39] *  p;
		d_prev_pilots_1[3] = current_symbol_1[53] * -p;

		// compensate residual frequency offset
                //TODO loop unrolling
		for(int i = 0; i < 64; i++) 
                {
			current_symbol[i]   *= exp(gr_complex(0, -beta));
			current_symbol_1[i] *= exp(gr_complex(0, -beta_1));
		}

		// update estimate of residual frequency offset
		if(d_current_symbol >= 2) { // i.e. when lts1, lts2 and SIGNAL field are already processed

			double alpha   = 0.1;
			double alpha_1 = 0.1;
			d_er   = (1-alpha)   * d_er   + alpha   * er;
			d_er_1 = (1-alpha_1) * d_er_1 + alpha_1 * er_1;
		}

		// do equalization
                //>std::cout << "equalizer called for symind-- " << d_current_symbol << std::endl; 
                //TODO make dc equalizer 
d_equalizer->equalize_soft_dc(current_symbol, current_symbol_1 , d_current_symbol, symbols, symbols_1, symbols_oai, symbols_oai_1, noise_vec, noise_vec_1, d_scaling, d_threshold, out1 + o * 48, d_frame_mod, d_frame_symbols);

// note that value inside out1 mat not be between 7 t0 -8, need to be chopped.

/*check point-2*/
#ifdef llr_out_from_equalizer
if(d_current_symbol == 2)
{
for(int i = 0; i < 48; i++)
{
std::cout << "llr," << i << "," << (float)(out1+o)[i] << std::endl;  
 // the out1 got float vals of my scaled llrs
}
}
std::cout << "----------------" << std::endl;
#endif

		// signal field
		if(d_current_symbol == 2) 
                { // 0 is lts1, 1 is lts2, 2 is SIGNAL
		//s_decode_signal_field(out1 + o * 48);
//		if(s_decode_signal_field(out1 + o * 48) && s_decode_signal_field(pout1 + o * 48)) // now it uses soft bits for decoding SIGNAL field
		//if(decode_signal_field(out + o * 48)) 
                // take both the channels 
		if(s_decode_signal_field_dc(out1 + o * 48))
		{
 
 //>std::cout << "decoded the signal field symInd--" << d_current_symbol << std::endl;
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("frame_bytes"), pmt::from_uint64(d_frame_bytes));
	dict = pmt::dict_add(dict, pmt::mp("encoding"), pmt::from_uint64(d_frame_encoding));
	dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::from_double(d_equalizer->get_snr_soft_dc()));
	dict = pmt::dict_add(dict, pmt::mp("freq"), pmt::from_double(d_freq));
	dict = pmt::dict_add(dict, pmt::mp("freq_offset"), pmt::from_double(d_freq_offset_from_synclong));
		add_item_tag(0, nitems_written(0) + o,
	       			pmt::string_to_symbol("wifi_start"),
				dict,
				pmt::string_to_symbol(alias()));
		}
		}

		if(d_current_symbol > 2) 
		{
                //>std::cout << "increasing output pointer--symind--" << d_current_symbol << std::endl;
			o++;
			pmt::pmt_t pdu = pmt::make_dict();
	message_port_pub(pmt::mp("symbols"), pmt::cons(pmt::make_dict(), pmt::init_c32vector(48, symbols)));
    //message_port_pub(pmt::mp("noise_vec"), pmt::cons(pmt::make_dict(), pmt::init_f32vector(64, noise_vec)));
    //message_port_pub(pmt::mp("noise_vec_1"), pmt::cons(pmt::make_dict(), pmt::init_f32vector(64, noise_vec_1)));
		}

		i++;
		d_current_symbol++;
	}

	consume(0, i);
	consume(1, i);
        //std::cout << "nitems_written(0)" << nitems_written(0) << "nitems_written(1)" << nitems_written(1) << std::endl;
	return o;
}

bool
sbmrc_dc_impl::s_decode_signal_field(float *rx_scaled_llr) {
/*
	for(int i = 0; i < 48; i++)
	{
	std::cout << "fllr," << i << "," << (int)(rx_scaled_llr)[i] << "," << (int)(rx_bits)[i]<< std::endl;  
	}
*/

	s_deinterleave(rx_scaled_llr);
/*
	for(int i = 0; i < 48; i++)
	{
	std::cout << "dlv," << i << "," << (int)(s_deinterleaved)[i] << "," << (int)(d_deinterleaved)[i]<< std::endl;  
	}
*/
	memset(s_decoded_bytes,0,sizeof(s_decoded_bytes)); // 24 or 3 ? 
	memset(s_decoded_bits,0,sizeof(s_decoded_bits)); // 24 
        s_decoder.oai_decode(s_deinterleaved,s_decoded_bytes,s_decoded_bits,24);

	return parse_signal(s_decoded_bits); // either use s_decoded or s_dcoded_bytes
	//return true;	

}

bool
sbmrc_dc_impl::s_decode_signal_field_dc(float *rx_scaled_llr_b1) {


	s_deinterleave_dc(rx_scaled_llr_b1);
	memset(s_decoded_bytes,0,sizeof(s_decoded_bytes)); // 24 or 3 ? 
	memset(s_decoded_bits,0,sizeof(s_decoded_bits)); // 24 
        s_decoder.oai_decode(s_deinterleaved,s_decoded_bytes,s_decoded_bits,24);

	return parse_signal(s_decoded_bits);
}

void
sbmrc_dc_impl::s_deinterleave(float *rx_scaled_llr) {
//TODO here convert the float llr to char llr lying from -8 to 7
	for(int i = 0; i < 48; i++) {
                if(rx_scaled_llr[i] > 7) 
		rx_scaled_llr[i] = 7;
		else if (rx_scaled_llr[i] < -8)
  		rx_scaled_llr[i] = -8;
		else
		rx_scaled_llr[i] = (char)rx_scaled_llr[i];
		s_deinterleaved[i] = rx_scaled_llr[interleaver_pattern[i]]; // is this correct to typecast here
		//{std::cout << "oai,"<<(int)s_deinterleaved[i] << std::endl;}
	}
}

void
sbmrc_dc_impl::s_deinterleave_dc(float *rx_scaled_llr_b1) {
//TODO here convert the float llr to char llr lying from -8 to 7
	for(int i = 0; i < 48; i++) {
                if(rx_scaled_llr_b1[i] > 7) 
		rx_scaled_llr_b1[i] = 7;
		else if (rx_scaled_llr_b1[i] < -8) 
  		rx_scaled_llr_b1[i] = -8;
		else
		rx_scaled_llr_b1[i] = (char)rx_scaled_llr_b1[i];
		s_deinterleaved[i] = rx_scaled_llr_b1[interleaver_pattern[i]]; 
		//{std::cout << "oai,"<<(int)s_deinterleaved[i] << std::endl;}
	}
}

bool
sbmrc_dc_impl::parse_signal(uint8_t *decoded_bits) {

	int r = 0;
	d_frame_bytes = 0;
	bool parity = false;
        bool result; 
	for(int i = 0; i < 17; i++) {
                //std::cout << "decoded_bits," << (int)decoded_bits[i] << std::endl;
		parity ^= decoded_bits[i];
		
		if((i < 4) && decoded_bits[i]) {
			r = r | (1 << i);
		}

		if(decoded_bits[i] && (i > 4) && (i < 17)) {
			d_frame_bytes = d_frame_bytes | (1 << (i-5));
		}
	}

	if(parity != decoded_bits[17]) {
		dout << "SIGNAL: wrong parity" << std::endl;
		//std::cout << "SIGNAL: wrong parity" << std::endl;
		result = false;
		return result;
	}

	switch(r) {
	case 11:
		d_frame_encoding = 0;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 24);
                //std::cout << d_frame_symbols << std::endl;
		d_frame_mod = d_bpsk;
		dout << "Encoding: 3 Mbit/s   ";
		result = true;
		break;
	case 15:
		d_frame_encoding = 1;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 36);
		d_frame_mod = d_bpsk;
		dout << "Encoding: 4.5 Mbit/s   ";
		result = false;
		break;
	case 10:
		d_frame_encoding = 2;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 48);
		d_frame_mod = d_qpsk;
		dout << "Encoding: 6 Mbit/s   ";
		result = false;
		break;
	case 14:
		d_frame_encoding = 3;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 72);
		d_frame_mod = d_qpsk;
		dout << "Encoding: 9 Mbit/s   ";
		result = false;
		break;
	case 9:
		d_frame_encoding = 4;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 96);
		d_frame_mod = d_16qam;
		dout << "Encoding: 12 Mbit/s   ";
		result = false;
		break;
	case 13:
		d_frame_encoding = 5;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 144);
		d_frame_mod = d_16qam;
		dout << "Encoding: 18 Mbit/s   ";
		result = false;
		break;
	case 8:
		d_frame_encoding = 6;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 192);
		d_frame_mod = d_64qam;
		dout << "Encoding: 24 Mbit/s   ";
		result = false;
		break;
	case 12:
		d_frame_encoding = 7;
		d_frame_symbols = (int) ceil((16 + 8 * d_frame_bytes + 6) / (double) 216);
		d_frame_mod = d_64qam;
		dout << "Encoding: 27 Mbit/s   ";
		result = false;
		break;
	default:
		dout << "unknown encoding" << std::endl;
		return result;
	}

	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_frame_encoding % d_frame_bytes % d_frame_symbols);
	return true;
}

const int
sbmrc_dc_impl::interleaver_pattern[48] = {
	 0, 3, 6, 9,12,15,18,21,
	24,27,30,33,36,39,42,45,
	 1, 4, 7,10,13,16,19,22,
	25,28,31,34,37,40,43,46,
	 2, 5, 8,11,14,17,20,23,
	26,29,32,35,38,41,44,47};

} /* namespace ieee802_11 */
} /* namespace gr */
