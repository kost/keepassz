/***************************************************************************
 *   Copyright (C) 2005-2008 by Felix Geyer                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *

 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "Application_X11.h"
#include "lib/AutoTypeGlobalX11.h"
#include "lib/HelperX11.h"

KeepassApplication::KeepassApplication(int& argc, char** argv) : QApplication(argc, argv), remove_invalid(0){
}

bool KeepassApplication::x11EventFilter(XEvent* event){
	if (autoType == NULL)
		return QApplication::x11EventFilter(event);
	
#ifdef GLOBAL_AUTOTYPE
	if (remove_invalid == 0) {
		AutoTypeGlobalX11* autoTypeGlobal = static_cast<AutoTypeGlobalX11*>(autoType);
		remove_invalid = ControlMask | ShiftMask | autoTypeGlobal->maskAlt() |
				autoTypeGlobal->maskAltGr() | autoTypeGlobal->maskMeta();
	}
	
	if (event->type==KeyPress && autoType->getShortcut().key!=0 &&
			event->xkey.keycode == XKeysymToKeycode(event->xkey.display,HelperX11::getKeysym(autoType->getShortcut().key)) &&
			(event->xkey.state&remove_invalid) == HelperX11::getShortcutModifierMask(autoType->getShortcut()) &&
			focusWidget()==NULL)
	{
		EventOccurred = true;
		autoType->performGlobal();
		return true;
	}
#endif
	
	if (event->type == MappingNotify) {
		dynamic_cast<AutoTypeX11*>(autoType)->updateKeymap();
		remove_invalid = 0;
	}
	
	return QApplication::x11EventFilter(event);
}
