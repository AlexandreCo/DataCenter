/*
    This file is part of ThermRF.

    ThermRF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ThermRF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2012  Alexandre Coffignal <alexandre.coffignal[@]gmail.com>

*/
#ifndef _DS1624
#define _DS1624
unsigned int ds1624_uiRead(char cNum);
void ds1624_vInit(char cNum);

#define DS_TH 	14	//slave address 
#endif