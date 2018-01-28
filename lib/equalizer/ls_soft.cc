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
	
	if(n == 0) 
	{
//>std::cout <<"EQ copied input to d_H_soft --" << " symInd->" << n << std::endl;
		std::memcpy(d_H_soft, in, 64 * sizeof(gr_complex)); // first lts copied in d_H

	} 
	else if(n == 1) 
	{ // the second lts now 

//>std::cout <<"EQ do chest now -- "<< " symInd->" << n << std::endl;
                double signal = 0;
		double noise = 0;
		int start = 21; // start sub carrier of interference
		int stop = start+7; // stop subcarrier of interference
		double noise_interf = 0; // local noise variance of interfered band
		double noise_non_interf = 0; // local noise variance of non-interfered band
		double conv_est = 0; // noise variance conv method
		double temp1 = 0;
		double temp2 = 0;
		double temp3 = 0;
		double temp4 = 0;
// calculation loop	
		for(int i = 0; i < 64; i++) // loop noise vars calc
		{ 
			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue; // skip nulls and dc
			} 
			 
			if(i >= start && i <= stop)
			{
			noise_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
			}
			else
			{
			noise_non_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2)); 
			}
			conv_est += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
		
			noise += std::pow(std::abs(d_H_soft[i] - in[i]), 2);
			signal += std::pow(std::abs(d_H_soft[i] + in[i]), 2);
			d_H_soft[i] += in[i]; // add d_H with in array i.e. second lts 
			d_H_soft[i] /= LONG_soft[i] * gr_complex(2, 0); // channel estimation for current frame
		}
		
		d_snr_soft = 10 * std::log10(signal / noise / 2);
		//std::cout << signal << "  " << noise << std::endl;                
//		d_NLR = (10 * std::log10((noise_interf/(2*(stop-start+1)))/(noise_non_interf/(2*(52-stop+start-1))))); // Noise Level Ratio

		d_NLR = ((noise_interf/(2*(stop-start+1)))/(noise_non_interf/(2*(52-stop+start-1))));
		//std::cout << "NLR " << (double)d_NLR << " "<< "SNR" << " "<< (double)d_snr_soft <<std::endl;

// assignment loop
		for(int i = 0; i < 64; i++)
		{
			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue;
			}
			if(i > start && i < stop)
			{
				d_N_soft_loc[i] = noise_interf/(2*(stop-start+1));
			}
			else 
			{ 
				d_N_soft_loc[i] = noise_non_interf/(2*(52-stop+start-1)); 
			}

			d_N_soft_conv[i] = conv_est/(2*52);
		}
	
		if(d_NLR > d_threshold) 
		{
			d_interference = 1;
			std::cout << "interference detected" << std::endl;
			std::cout << "NLR->" << d_NLR <<" n->" << n <<" if->" << d_interference << " nfs->" << d_frame_symbols << " SNR->"<< d_snr_soft <<std::endl;
		}

	} else { // from n = 2 onwards, data symbols are there
                //std::cout << "new symbol " << std::endl;
//>std::cout <<"EQ equalize and demap-- " <<" symInd->" << n << std::endl;
		if(d_interference )
		{
                	if(n > 2)
			{
				if((n - d_frame_symbols) == 2)
				{
					d_interference = 0; 
                 			std::cout << "reset "<< "n->" << n << " d_frame_symbols->" << d_frame_symbols << std::endl;
				}
			}
		}

                //std::cout << "n--" << n << "i-- " << imt << "d_frame_symbols" << d_frame_symbols << std::endl;
		int c = 0;
		for(int i = 0; i < 64; i++) {
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) { // skip the pilots, zero padded subs and the dc, only equalize 48 data syms 
				continue;
			} else {
				//std::cout << "EQ equalized for symInd -->" << n << std::endl;
				symbols[c] = in[i] / d_H_soft[i]; // equalize them with chest d_H
				//std::cout << "symbol" << symbols[c] << std::endl;
				//std::cout << "EQ demap for symInd -->" << n << std::endl;
				bits[c] = mod_soft->decision_maker(&symbols[c]); // hard bits
                                //std::cout << "bits[c]" << (int)bits[c] << std::endl;
                                if(d_interference){
					llr[c] = (-4*real(symbols[c]))/d_N_soft_loc[i]; //soft bits +llr scaling
                                        //std::cout << llr[c] << std::endl;
					}
				else{ 
					//llr[c] = (-4*real(symbols[c]))/d_N_soft_conv[i];
					llr[c] = bits[c] == 0 ? -8 : 7; // llr at high snr 
//>std::cout << "symInd " << n-2 << " "<< (float)llr[c] << " " << (int)bits[c] << std::endl;
//std::cout << n << ","<< (float)llr[c] << "," << (int)bits[c] << std::endl;

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
