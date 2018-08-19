/*
 ===========================================================================

 Daemon GPL Source Code
 Copyright (C) 2012 Unvanquished Developers

 This file is part of the Daemon GPL Source Code (Daemon Source Code).

 Daemon Source Code is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Daemon Source Code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Daemon Source Code.  If not, see <http://www.gnu.org/licenses/>.

 In addition, the Daemon Source Code is also subject to certain additional terms.
 You should have received a copy of these additional terms immediately following the
 terms and conditions of the GNU General Public License which accompanied the Daemon
 Source Code.  If not, please request a copy in writing from id Software at the address
 below.

 If you have questions concerning this license or the applicable additional terms, you
 may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville,
 Maryland 20850 USA.

 ===========================================================================
 */

#ifndef LOGGING_H_
#define LOGGING_H_

/**
 * keyword currently used to mark a line as having to be appended to the log, but not notify about it on the HUD
 */
#define S_SKIPNOTIFY "[skipnotify]"

#ifdef ERR_FATAL
#undef ERR_FATAL // this is possibly defined in malloc.h
#endif

// parameters to the main Error routine
enum class errorParm_t
{
	ERR_FATAL, // exit the entire game with a popup window
	ERR_DROP, // print to console and disconnect from game
	ERR_SERVERDISCONNECT, // don't kill server
};

// this is only here so the functions in q_shared.c and bg_*.c can link
void QDECL NORETURN Com_Error( errorParm_t level, const char *error, ... ) PRINTF_LIKE(2);

#endif /* LOGGING_H_ */
