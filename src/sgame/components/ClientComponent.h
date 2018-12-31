/*
 * CBSE GPL Source Code
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


#ifndef CLIENT_COMPONENT_H_
#define CLIENT_COMPONENT_H_

#include "../backend/CBSEBackend.h"
#include "../backend/CBSEComponents.h"

class ClientComponent: public ClientComponentBase {

	public:
		// ///////////////////// //
		// Autogenerated Members //
		// ///////////////////// //

		/**
		 * @brief Default constructor of the ClientComponent.
		 * @param entity The entity that owns the component instance.
		 * @param clientData An initialization parameter.
		 * @param r_TeamComponent A TeamComponent instance that this component depends on.
		 * @note This method is an interface for autogenerated code, do not modify its signature.
		 */
		ClientComponent(Entity& entity, gclient_t* clientData, TeamComponent& r_TeamComponent);


		// ///////////////////// //

		gclient_t* GetClientData() {
			return clientData;
		}

	private:

};

#endif // CLIENT_COMPONENT_H_

