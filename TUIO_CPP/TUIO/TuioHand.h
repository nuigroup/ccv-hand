/*
/*
Modified by: Thiago de Freitas Oliveira Araújo
NUI GROUP - Google Summer of Code 2009
Handling hand format

 TUIO C++ Library - part of the reacTIVision project
 http://reactivision.sourceforge.net/
 
 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_TUIOHAND_H
#define INCLUDED_TUIOHAND_H

#include <math.h>
#include "TuioContainer.h"

namespace TUIO {
	
	/**
	 * The TuioHand class encapsulates /tuio/2Dhnd TUIO Hans.
	 *
	 * @author Thiago de Freitas Oliveira Araújo
	 * @version 1.4 Modified
	* @version 1.4 Original - by Martin Kaltenbrunner
	 */ 
	class TuioHand: public TuioContainer {
		
	protected:
		/**
		 * The individual hand ID number that is assigned to each TuioHand.
		 */ 
		int hand_id;
		
	public:
		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided 
		 * Session ID, Cursor ID, X and Y coordinate to the newly created TuioHand.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	si	the Session ID  to assign
		 * @param	ci	the Hand ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		TuioHand (TuioTime ttime, long si, int ci, float xp, float yp):TuioContainer(ttime,si,xp,yp) {
			hand_id = ci;
		};

		/**
		 * This constructor takes the provided Session ID, Hand ID, X and Y coordinate 
		 * and assigs these values to the newly created TuioHand.
		 *
		 * @param	si	the Session ID  to assign
		 * @param	ci	the Hand ID  to assign
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 */
		TuioHand (long si, int ci, float xp, float yp):TuioContainer(si,xp,yp) {
			hand_id = ci;
		};
		
		/**
		 * This constructor takes the atttibutes of the provided TuioHand 
		 * and assigs these values to the newly created TuioHand.
		 *
		 * @param	thand	the TuioHand to assign
		 */
		TuioHand (TuioHand *thand):TuioContainer(thand) {
			hand_id = thand->getHandID();
		};
		
		/**
		 * The destructor is doing nothing in particular. 
		 */
		~TuioHand(){};
		
		/**
		 * Returns the Hand ID of this TuioHand.
		 * @return	the Hand ID of this TuioHand
		 */
		int getHandID() {
			return hand_id;
		};
	};
};
#endif
