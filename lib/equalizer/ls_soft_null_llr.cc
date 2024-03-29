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

#include "ls_soft_null_llr.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace gr::ieee802_11::equalizer_soft;

void ls_soft_null_llr::equalize_soft(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, float *llr, boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols) {
	
	if(n == 0) 
	{
		std::memcpy(d_H_soft, in, 64 * sizeof(gr_complex)); 
	} 
	else if(n == 1) 
	{ 
                double signal = 0;
                double noise = 0;
		int start = 20; // start sub carrier of interference-1
		int stop = start+10; // stop subcarrier of interference-1


		double noise_interf = 0; // local noise variance of interfered-1 band
		double noise_non_interf = 0; // local noise variance of non-interfered-1 band
		double conv_est = 0; // noise variance conv method

		// calculation loop	
		for(int i = 0; i < 64; i++) 
		{ 

			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue; 
			} 
		// interf detection for zb ch- 2(relative)	 
			if(i >= start && i <= stop)
			{
			noise_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
			}
			else
			{
			noise_non_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2)); 
			}
			//conv_est += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
		
			noise += std::pow(std::abs(d_H_soft[i] - in[i]), 2);

			signal += std::pow(std::abs(d_H_soft[i] + in[i]), 2);

			d_H_soft[i] += in[i]; 

			d_H_soft[i] /= LONG_soft[i] * gr_complex(2, 0); 

			//CSI[i] += real(d_H_soft[i] * conj(d_H_soft[i]));

                        d_temp += std::pow(std::abs(d_H_soft[i]),2);

		}
                d_temp = d_temp/64;
                d_snr_soft = 10 * std::log10(signal / noise / 2);
//std::cout << "d_snr_soft "<<d_snr_soft << std::endl;

/*NLR for ch-2*/d_NLR = ((noise_interf/(2*(stop-start+1)))/(noise_non_interf/(2*(52-stop+start-1))));
//std::cout << "d_NLR "<<d_NLR << std::endl;

		// assignment loop
		for(int i = 0; i < 64; i++)
		{

                        d_H_Var[i] = std::pow(std::abs(d_H_soft[i]),2)/d_temp; // normalized channel 

			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue;
			}
			if(i >= start && i <= stop)
			{
				//d_N_soft_loc[i] = noise_interf/(2*(stop-start));
				//d_N_soft_loc[i] = d_NLR/8;
				d_N_soft_loc[i] = 1;

			}
			else 
			{ 
				//d_N_soft_loc[i] = noise_non_interf/(2*(52-stop+start)); 
				d_N_soft_loc[i] = 1; 
			}

			d_N_soft_conv[i] = 1;
		}
	
	} else { // from n = 2 onwards, data symbols are there
		std::ofstream myfile;
		myfile.open ("/home/john/null_llr.csv");
		int c = 0;
		for(int i = 0; i < 64; i++) 
		{
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) 				{ 
				continue;
			} else {
				symbols[c] = in[i] / d_H_soft[i]; // equalize them with chest d_H
                                bits[c] = mod_soft->decision_maker(&symbols[c]); // hard bits
				// uncomment above to get hard bits also 
				//bits[c] = 0;
				temp_symbols[c] = 7*real(in[i] * conj(d_H_soft[i]))/d_temp;
				llr[c] = temp_symbols[c]/d_N_soft_loc[i];
				//std::cout << "doing nulling" << std::endl;
				if(c >=20 && c <= 27) {llr[c] = 0;}
				myfile << (float)llr[c] << "," << std::endl;
std::cout << "m here" << std::endl;
                                c++;
                                }
		}
		myfile.close();
		
	}
}

double ls_soft_null_llr::get_snr_soft() {
	return d_snr_soft;
}
