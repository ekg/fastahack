/***************************************************************************
 *  libdisorder: A Library for Measuring Byte Stream Entropy
 *  Copyright (C) 2010 Michael E. Locasto
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the:
 *       Free Software Foundation, Inc.
 *       59 Temple Place, Suite 330
 *       Boston, MA  02111-1307  USA
 *
 * $Id$
 **************************************************************************/

#ifndef __DISORDER_H_
#define __DISORDER_H_

/** Max number of bytes (i.e., tokens) */
#define LIBDO_MAX_BYTES      256

/** A convienance value for clients of this library. Feel free to change
 * if you plan to use a larger buffer. You can also safely ignore it, as
 * libdisorder does not use this value internally; it relies on the
 * client-supplied `length' parameter.
 *
 * NB: Might become deprecated because it is potentially misleading and
 * has zero relationship to any library internal state.
 */
#define LIBDO_BUFFER_LEN   16384

/** 
 * Given a pointer to an array of bytes, return a float indicating the
 * level of entropy in bits (a number between zero and eight),
 * assuming a space of 256 possible byte values. The second argument
 * indicates the number of bytes in the sequence. If this sequence
 * runs into unallocated memory, this function should fail with a
 * SIGSEGV.
 */
float    shannon_H(char*, long long);

/** Report the number of (unique) tokens seen. This is _not_ the
    number of individual events seen. For example, if the library sees
    the string `aaab', the number of events is 4 and the number of
    tokens is 2. */
int      get_num_tokens(void);

/** Returns maximum entropy for byte distributions log2(256)=8 bits*/
float    get_max_entropy(void);

/** Returns the ratio of entropy to maxentropy */
float    get_entropy_ratio(void);

#endif
