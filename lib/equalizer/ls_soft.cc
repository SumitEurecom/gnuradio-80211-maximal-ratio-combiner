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

void ls_soft::equalize_soft(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, float *llr, boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols) {
	
	if(n == 0) {
std::cout << "n->" << n <<" interference->" << d_interference << " d_frame_symbols->" << d_frame_symbols << std::endl;
		std::memcpy(d_H_soft, in, 64 * sizeof(gr_complex)); // first lts copied in d_H

	} else if(n == 1) { // the second lts now 

                //imt = 1; // check point var, set in one section, to be accessed in another section during multiple calls of equalize_soft
                double signal = 0;
		double noise = 0;
		int start = 5; // start sub carrier of interference
		int stop = start+11; // stop subcarrier of interference
		int noise_interf = 0; // noise variance of interfered band
		int noise_non_interf = 0; // noise variance of non-interfered band
		int conv_est = 0; // noise variance conv method
		int temp1 = 0;
		int temp2 = 0;
	
		for(int i = 0; i < 64; i++) 
		{ 
		// skip null padded subs and the dc 		
		if((i == 32) || (i < 6) || ( i > 58)) {continue;}

                d_N_soft_indv[i] = (std::pow(std::abs(d_H_soft[i] - in[i]), 2))/2;// always do it

                if(d_N_soft_indv[i] > 100) 
		{d_interference = 1;
std::cout << "n->" << n <<" interference->" << d_interference << " d_frame_symbols->" << d_frame_symbols << std::endl;
		}
		// local noise variance estimate : when interference starts
		if(d_interference)
		{
			if(i > start && i <= stop)
			{
			//std::cout << "interference band" << std::endl;
                        //std::cout << i << std::endl;
			temp1 = (std::pow(std::abs(d_H_soft[i] - in[i]), 2))/(2*(stop-start+1));
			noise_interf += temp1;
			}
			else
			{
			//std::cout << "non interference" << std::endl;
                        //std::cout << i << std::endl;
			temp2 = (std::pow(std::abs(d_H_soft[i] - in[i]), 2))/(2*(52-stop+start-1)); 
			noise_non_interf += temp2;
			}
		}

		// conventional noise variance estimate : when no interference, always do this
		//{
			conv_est += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
		//}

			noise += std::pow(std::abs(d_H_soft[i] - in[i]), 2);
			signal += std::pow(std::abs(d_H_soft[i] + in[i]), 2);
			d_H_soft[i] += in[i]; // add d_H with in array i.e. second lts 
			d_H_soft[i] /= LONG_soft[i] * gr_complex(2, 0); // channel estimation for current frame
		}

                if(d_interference){ // populate noise variance vector when interference
		for(int i = 0; i < 64; i++)
			{
				if((i == 32) || (i < 6) || ( i > 58)) {continue;}
				if(i > start && i < stop){d_N_soft_loc[i] = noise_interf;}
				else { d_N_soft_loc[i] = noise_non_interf; }
				//std::cout << " interference " << interference << "d_N_soft_loc[i] " << 					d_N_soft_loc[i] << std::endl; // to print local est
			}
		}
                else{
		for(int i = 0; i < 64; i++) // populate noise variance vector when no interference
			{
				if((i == 32) || (i < 6) || ( i > 58)) {continue;}
				d_N_soft_conv[i] = conv_est/(2*52);
				//std::cout << "interference " << interference << "d_N_soft_conv[i] " << 					d_N_soft_conv[i] << std::endl; // to print conv est
			}
		}


		d_snr_soft = 10 * std::log10(signal / noise / 2);

	} else { // from n = 2 onwards, data symbols are there
		if(d_interference && ((n - d_frame_symbols) == 2)){d_interference = 0; 
                 //std::cout << "interference var reset, check again in new frame" << std::endl;
		}
std::cout << "n->" << n <<" interference->" << d_interference << " d_frame_symbols->" << d_frame_symbols << std::endl;
                //std::cout << "n--" << n << "i-- " << imt << "d_frame_symbols" << d_frame_symbols << std::endl;
		int c = 0;
		for(int i = 0; i < 64; i++) {
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) { // skip the pilots, zero padded subs and the dc, only equalize 48 data syms 
				continue;
			} else {
				symbols[c] = in[i] / d_H_soft[i]; // equalize them with chest d_H
				bits[c] = mod_soft->decision_maker(&symbols[c]); // hard bits
                                if(d_interference){
					llr[c] = (-4*real(symbols[c]))/d_N_soft_loc[i]; //soft bits +llr scaling
                                        //std::cout << "llr" << llr[c] << std::endl;
					}
				else{ 
					llr[c] = (-4*real(symbols[c]))/d_N_soft_conv[i];
					//std::cout << "llr" << llr[c] << std::endl;
				    }

//TODO soft decision calc for future mod_soft->calc_soft_dec(symbols[c], 1.0);
				//std::cout << (unsigned int)bits[c] << "--" << symbols[c] << std::endl;
                                c++;
                                //std::cout << "c--" << c <<"--i--"<< i <<std::endl;
			}
		}
	}
}

double ls_soft::get_snr_soft() {
	return d_snr_soft;
}
