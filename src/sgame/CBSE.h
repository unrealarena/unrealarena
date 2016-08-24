/*
 * CBSE Source Code
 * Copyright (C) 2016  Unreal Arena
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


// This header includes the rest of the CBSE system.
// It also provides to the CBSE backend all types that it needs to know about.

#ifndef CBSE_H_
#define CBSE_H_

// Add here any definitions, forward declarations and includes that provide all
// the types used in the entities definition file (and thus the CBSE backend).
// Make sure none of the includes in this file includes any header that is part
// of the CBSE system.
// You can also define helper macros for use in all components here.
// ----------------



// ----------------

// Include the backend. These should be the last lines in this header.
#ifndef CBSE_INCLUDE_TYPES_ONLY
#include "backend/CBSEEntities.h"
#endif // CBSE_INCLUDE_TYPES_ONLY

#endif // CBSE_H_
