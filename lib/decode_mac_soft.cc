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
#include <ieee802-11/decode_mac_soft.h>

#include "utils.h"
#include "viterbi_decoder.h"

#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;

#define LINKTYPE_IEEE802_11 105 /* http://www.tcpdump.org/linktypes.html */

class decode_mac_soft_impl : public decode_mac_soft {

public:
decode_mac_soft_impl(bool log, bool debug) :
	block("decode_mac_soft",
			gr::io_signature::make(1, 1, 48*sizeof(float)), // takes float llr input (only bpsk now)
			gr::io_signature::make(0, 0, 0)),
	d_log(log),
	d_debug(debug),
	d_snr(0),
	d_nom_freq(0.0),
	d_freq_offset(0.0),
	d_ofdm(BPSK_1_2),
	d_frame(d_ofdm, 0),
	d_frame_complete(true) {

	message_port_register_out(pmt::mp("out"));
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const float *in = (const float*)input_items[0]; // the soft bits from ls_soft.ccc 

	int i = 0;

	std::vector<gr::tag_t> tags;
	const uint64_t nread = this->nitems_read(0);

	dout << "Decode MAC: input " << ninput_items[0] << std::endl;

	while(i < ninput_items[0]) {

		get_tags_in_range(tags, 0, nread + i, nread + i + 1, // find the moment when wifi starts
			pmt::string_to_symbol("wifi_start"));

		if(tags.size()) { // if "wifi_start" tag found
			if (d_frame_complete == false) {
				dout << "Warning: starting to receive new frame before old frame was complete" << std::endl;
				dout << "Already copied " << copied << " out of " << d_frame.n_sym << " symbols of last frame" << std::endl;
			}
			d_frame_complete = false;

			pmt::pmt_t dict = tags[0].value;
			int len_data = pmt::to_uint64(pmt::dict_ref(dict, pmt::mp("frame_bytes"), pmt::from_uint64(MAX_PSDU_SIZE+1)));
			int encoding = pmt::to_uint64(pmt::dict_ref(dict, pmt::mp("encoding"), pmt::from_uint64(0)));
			d_snr = pmt::to_double(pmt::dict_ref(dict, pmt::mp("snr"), pmt::from_double(0)));
			d_nom_freq = pmt::to_double(pmt::dict_ref(dict, pmt::mp("freq"), pmt::from_double(0)));
			d_freq_offset = pmt::to_double(pmt::dict_ref(dict, pmt::mp("freq_offset"), pmt::from_double(0)));

			ofdm_param ofdm = ofdm_param((Encoding)encoding);
			frame_param frame = frame_param(ofdm, len_data);

			// check for maximum frame size
			if(frame.n_sym <= MAX_SYM && frame.psdu_size <= MAX_PSDU_SIZE) {
				d_ofdm = ofdm;
				d_frame = frame;
				copied = 0;
				dout << "Decode MAC: frame start -- len " << len_data
					<< "  symbols " << frame.n_sym << "  encoding "
					<< encoding << std::endl;
			} else {
				dout << "Dropping frame which is too large (symbols or bits)" << std::endl;
			}
		}

		if(copied < d_frame.n_sym) {
			dout << "copy one symbol, copied " << copied << " out of " << d_frame.n_sym << std::endl;
			std::memcpy(d_rx_soft_symbols + (copied * 48), in, 48);
			copied++;

			if(copied == d_frame.n_sym) {
				dout << "received complete frame - decoding" << std::endl;
		// at this point everything is copied in "d_rx_soft_symbols" now call decode function
                // input to decode() is "d_rx_soft_symbols"
				decode();
				in += 48;
				i++;
				d_frame_complete = true;
				break;
			}
		}

		in += 48;
		i++;
	}

	consume(0, i);

	return 0;
}

void decode() {

	for(int i = 0; i < d_frame.n_sym * 48; i++) {
		for(int k = 0; k < d_ofdm.n_bpsc; k++) { // n_bpsc = 1 for bpsk 
			//d_rx_soft_bits[i*d_ofdm.n_bpsc + k] = !!(d_rx_soft_symbols[i] & (1 << k)); 
			d_rx_soft_bits[i*d_ofdm.n_bpsc + k] = d_rx_soft_symbols[i];
                        // TODO above will work only for bpsk becz d_ofdm.n_bpsc = 1 for bpsk
		}
	}
        // the above loops puts "d_rx_soft_symbols" into "d_rx_soft_bits" 
        // before calling viterbi, call the deinterleaver with input "d_rx_soft_bits"
	// the output of deinterleaver is "d_deinterleaved_soft_bits" which is now the input for viterbi

	deinterleave();
	// TODO uint8_t *decoded = d_decoder.decode(&d_ofdm, &d_frame, d_deinterleaved_soft_bits);
	// TODO descramble(decoded);
	//print_output();

	// skip service field
	boost::crc_32_type result;
	result.process_bytes(out_bytes + 2, d_frame.psdu_size);
	if(result.checksum() != 558161692) {
		dout << "checksum wrong -- dropping" << std::endl;
		return;
	}

	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_ofdm.encoding % d_frame.psdu_size % d_frame.n_sym);

	// create PDU
	pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_frame.psdu_size - 4);
	pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
	dict = pmt::dict_add(dict, pmt::mp("snr"), pmt::from_double(d_snr));
	dict = pmt::dict_add(dict, pmt::mp("nomfreq"), pmt::from_double(d_nom_freq));
	dict = pmt::dict_add(dict, pmt::mp("freqofs"), pmt::from_double(d_freq_offset));
	dict = pmt::dict_add(dict, pmt::mp("dlt"), pmt::from_long(LINKTYPE_IEEE802_11));
	message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
}

void deinterleave() { // this shud work for all mods, no need to change I guess! 

	int n_cbps = d_ofdm.n_cbps; // for bpsk it is 48 
	int first[n_cbps]; // all elements will be int, dont change the type 
	int second[n_cbps]; // all elements will be int, dont change the type
	int s = std::max(d_ofdm.n_bpsc / 2, 1); // for bpsk its 1

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s); 
	}

	for(int i = 0; i < n_cbps; i++) {
		second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
	}

	int count = 0;
	for(int i = 0; i < d_frame.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			d_deinterleaved_soft_bits[i * n_cbps + second[first[k]]] = d_rx_soft_bits[i * n_cbps + k];
		}
	}
}


void descramble (uint8_t *decoded_bits) {

	int state = 0;
	std::memset(out_bytes, 0, d_frame.psdu_size+2);

	for(int i = 0; i < 7; i++) {
		if(decoded_bits[i]) {
			state |= 1 << (6 - i);
		}
	}
	out_bytes[0] = state;

	int feedback;
	int bit;

	for(int i = 7; i < d_frame.psdu_size*8+16; i++) {
		feedback = ((!!(state & 64))) ^ (!!(state & 8));
		bit = feedback ^ (decoded_bits[i] & 0x1);
		out_bytes[i/8] |= bit << (i%8);
		state = ((state << 1) & 0x7e) | feedback;
	}
}

void print_output() {

	dout << std::endl;
	dout << "psdu size" << d_frame.psdu_size << std::endl;
	for(int i = 2; i < d_frame.psdu_size+2; i++) {
		dout << std::setfill('0') << std::setw(2) << std::hex << ((unsigned int)out_bytes[i] & 0xFF) << std::dec << " ";
		if(i % 16 == 15) {
			dout << std::endl;
		}
	}
	dout << std::endl;
	for(int i = 2; i < d_frame.psdu_size+2; i++) {
		if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
			dout << ((char) out_bytes[i]);
		} else {
			dout << ".";
		}
	}
	dout << std::endl;
}

private:
	bool d_debug;
	bool d_log;

	frame_param d_frame;
	ofdm_param d_ofdm;
	double d_snr;  // dB
	double d_nom_freq;  // nominal frequency, Hz
	double d_freq_offset;  // frequency offset, Hz
	viterbi_decoder d_decoder;

	float d_rx_soft_symbols[48 * MAX_SYM]; // float type to store llrs coming from ls_soft.cc
	float d_rx_soft_bits[MAX_ENCODED_BITS]; // float type to store llrs to be fed to deinterleaver
	float d_deinterleaved_soft_bits[MAX_ENCODED_BITS]; // float type to store deinterleaved llrs
	uint8_t out_bytes[MAX_PSDU_SIZE + 2]; 

	int copied;
	bool d_frame_complete;
};

decode_mac_soft::sptr
decode_mac_soft::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new decode_mac_soft_impl(log, debug));
}
