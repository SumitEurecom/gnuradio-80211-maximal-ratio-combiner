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

#include "base_soft_dc.h"
#include <cstring>
#include <iostream>

using namespace gr::ieee802_11::equalizer_soft_dc;

const gr_complex base_soft_dc::LONG_soft_dc[] = {
	 0,  0,  0,  0,  0,  0,  1,  1, -1, -1,
	 1,  1, -1,  1, -1,  1,  1,  1,  1,  1,
	 1, -1, -1,  1,  1, -1,  1, -1,  1,  1,
	 1,  1,  0,  1, -1, -1,  1,  1, -1,  1,
	-1,  1, -1, -1, -1, -1, -1,  1,  1, -1,
	-1,  1, -1,  1, -1,  1,  1,  1,  1,  0,
	 0,  0,  0,  0
};

const gr_complex base_soft_dc::POLARITY_soft_dc[127] = {
	 1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
	-1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1,
	 1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
	-1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
	-1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
	-1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
	-1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
	-1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1 };
