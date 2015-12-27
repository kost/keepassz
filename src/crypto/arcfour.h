/*
  Copyright (C) 2003-2008 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _ARCFOUR_H_
#define _ARCFOUR_H_

class CArcFour{
	public:
		void encrypt(const quint8* src, quint8* dst, uint length);
		inline void decrypt(const quint8* src, quint8* dst, uint length){encrypt(src,dst,length);} //just for readability
		void setKey(quint8* key, uint length);
		
	private:
		quint8* RawKey;
		uint RawKeyLength;
};

#endif
