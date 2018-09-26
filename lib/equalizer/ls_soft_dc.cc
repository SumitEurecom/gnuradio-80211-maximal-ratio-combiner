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

#include "ls_soft_dc.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace gr::ieee802_11::equalizer_soft_dc;

void ls_soft_dc::equalize_soft_dc(gr_complex *in, gr_complex *in_1, int n, gr_complex *symbols, gr_complex *symbols_1, gr_complex *symbols_oai, gr_complex *symbols_oai_1, float *noise_vec, float *noise_vec_1, int scaling, int threshold, uint8_t *bits, uint8_t *bits_1, float *llr, float *llr_1,  boost::shared_ptr<gr::digital::constellation> mod_soft, int d_frame_symbols) {

	//std::cout << "yo scaling is " << scaling << std::endl;
	
	if(n == 0) 
	{
		std::memcpy(d_H_soft, in, 64 * sizeof(gr_complex)); // copy 64 samples from channel 1
		std::memcpy(d_H_soft_1, in, 64 * sizeof(gr_complex)); // copy 64 samples from channel 2 
	} 
	else if(n == 1) 
	{ 
                double signal = 0;
                double signal_1 = 0;
                //double norm_lts1 = 0;
                //double norm_lts2 = 0;
                double noise = 0;
                double noise_1 = 0;
		int start = 27; // start sub carrier of interference-1
		int stop = 39; // stop subcarrier of interference-1
		double noise_interf = 0; // local noise variance of interfered-1 band
		double noise_interf_1 = 0; // local noise variance of interfered-1 band
		double noise_non_interf = 0; // local noise variance of non-interfered-1 band
		double noise_non_interf_1 = 0; // local noise variance of non-interfered-1 band
		// calculation loop	
		for(int i = 0; i < 64; i++) 
		{ 

			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue; 
			} 
	 		// LNV Estimates
			if(i >= start && i <= stop)
			{
			noise_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2));
			noise_interf_1 += (std::pow(std::abs(d_H_soft_1[i] - in[i]), 2));
			}
			else
			{
			noise_non_interf += (std::pow(std::abs(d_H_soft[i] - in[i]), 2)); 
			noise_non_interf_1 += (std::pow(std::abs(d_H_soft_1[i] - in[i]), 2)); 
			}

			d_H_soft[i] += in[i]; 
			d_H_soft_1[i] += in_1[i]; 

			d_H_soft[i] /= LONG_soft_dc[i] * gr_complex(2, 0); 
			d_H_soft_1[i] /= LONG_soft_dc[i] * gr_complex(2, 0); 

                        d_temp += std::pow(std::abs(d_H_soft[i]),2);
                        d_temp_1 += std::pow(std::abs(d_H_soft_1[i]),2);

		}
                d_temp = d_temp/64;
                d_temp_1 = d_temp_1/64;
               
		d_snr_soft = 10 * std::log10(signal / noise / 2);
		d_snr_soft_1 = 10 * std::log10(signal_1 / noise_1 / 2);

//std::cout << "d_snr_soft "<<d_snr_soft << std::endl;

/*NLR for ch-2*/d_NLR = ((noise_interf/(2*(stop-start+1)))/(noise_non_interf/(2*(52-stop+start-1))));
		d_NLR_1 = ((noise_interf_1/(2*(stop-start+1)))/(noise_non_interf_1/(2*(52-stop+start-1))));
//std::cout << "Before Detection d_NLR-> "<<d_NLR << "thr"<< threshold <<std::endl;
//TODO shall I detect interf on one channel or both ? 
		if(d_NLR > threshold) 
		{
			d_interference = 1;
			//std::cout << "interference detected: ZigBee on Ch-17" << "d_NLR " << d_NLR <<std::endl;
		}
		else
		{
			d_interference = 0;
		}

		// assignment loop
		for(int i = 0; i < 64; i++)
		{

                        d_H_Var[i] = std::pow(std::abs(d_H_soft[i]),2)/d_temp; // normalized channel 
                        d_H_Var_1[i] = std::pow(std::abs(d_H_soft_1[i]),2)/d_temp_1; // normalized channel 

			if((i == 32) || (i < 6) || ( i > 58)) 
			{
				continue;
			}
			if(i >= start && i <= stop)
			{
				d_N_soft_loc[i] = noise_interf/(2*(stop-start));
				d_N_soft_loc_1[i] = noise_interf_1/(2*(stop-start));
				//d_N_soft_loc[i] = d_NLR/8;
				//d_N_soft_loc[i] = 1;

			}
			else 
			{ 
				d_N_soft_loc[i] = noise_non_interf/(2*(52-stop+start)); 
				d_N_soft_loc_1[i] = noise_non_interf_1/(2*(52-stop+start)); 
				//d_N_soft_loc[i] = 1; 
			}

			//d_N_soft_conv[i] = conv_est/(2*52);
			d_N_soft_conv[i] = 1;
			d_N_soft_conv_1[i] = 1;
		}
		std::memcpy(noise_vec, d_N_soft_loc, sizeof(float)*64);// I need only one noise vec as of now 
	//std::memcpy(noise_vec_1, d_N_soft_loc_1, sizeof(float)*64);// I need only one noise vec as of now 

	} else { // from n = 2 onwards, data symbols are there

		if(d_interference )
		{
                	if(n > 2)
			{
				if((n - d_frame_symbols) == 2) // frame ends, reset interference flag
				{
					d_interference = 0; 
				}
			}
		}

		int c = 0;
		for(int i = 0; i < 64; i++) 
		{
			if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) 				{ 
				continue;
			} else {
				symbols[c] = in[i] / d_H_soft[i]; // equalize them with chest d_H
				symbols_1[c] = in_1[i] / d_H_soft_1[i]; // equalize them with chest d_H
                                //bits[c] = mod_soft->decision_maker(&symbols[c]); // hard bits
				// uncomment above to get hard bits also 
				bits[c] = 0;
				bits_1[c] = 0;
				temp_symbols[c] = 7*real(in[i] * conj(d_H_soft[i]))/d_temp;
				temp_symbols_1[c] = 7*real(in_1[i] * conj(d_H_soft_1[i]))/d_temp_1;
				//symbols_oai[c] = (in[i] * conj(d_H_soft[i]))/gr_complex(d_temp,0);
symbols_oai[c] = (in[i] * conj(d_H_soft[i]))/gr_complex(std::pow(std::abs(d_H_soft[i]),2),0);
symbols_oai_1[c] = (in_1[i] * conj(d_H_soft_1[i]))/gr_complex(std::pow(std::abs(d_H_soft_1[i]),2),0);
/* the commented section valid only when interference detection happens*/                      
/*          
				if(d_interference)
				{
				llr[c] = temp_symbols[c]/d_N_soft_loc[i];//*CSI[i]; //soft bits +llr scaling
				if(c >= 20 && c <= 29) {llr[c] = 0;}
				//llr[c] = 4*real(symbols[c])/d_N_soft_loc[i];//*CSI[i]; //soft bits +llr scaling
				}
				else
				{ 
                                llr[c] = temp_symbols[c]/d_N_soft_conv[i];//*CSI[i];
				//llr[c] = 4*real(symbols[c])/d_N_soft_conv[i];//*CSI[i]; //soft bits +llr scaling
				}
*/

/*when there is deterministic interference, the code below is applicable */

				llr[c] = temp_symbols[c]/d_N_soft_conv[i];
				llr_1[c] = temp_symbols_1[c]/d_N_soft_conv_1[i];
				
                                if (d_interference)
				if(scaling)
				{
					{
						if(c >= 19 && c <= 30) 
							{llr[c] = 0; llr_1[c] = 0;}
					}			
				}
//TODO soft decision calc for future mod_soft->calc_soft_dec(symbols[c], 1.0);
                                c++;
                                }
		}

	}
}

double ls_soft_dc::get_snr_soft_dc() {
	return d_snr_soft;
}

