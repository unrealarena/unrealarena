/*
 * CBSE GPL Source Code
 * Copyright (C) 2016-2018  Unreal Arena
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


// THIS FILE IS AUTO GENERATED, EDIT AT YOUR OWN RISK

/*
 * This file contains:
 *   - Declarations of the specific entities.
 */

#ifndef CBSE_ENTITIES_H_
#define CBSE_ENTITIES_H_

#include "CBSEComponents.h"

// ///////////////// //
// Specific Entities //
// ///////////////// //

/** A specific entity. */
class EmptyEntity: public Entity {
	public:
		/** Initialization parameters for EmptyEntity. */
		struct Params {
			gentity_t* oldEnt;
		};

		/** Default constructor of EmptyEntity. */
		EmptyEntity(Params params);

		/** Default destructor of EmptyEntity. */
		virtual ~EmptyEntity() = default;


	private:
		/** EmptyEntity's message handler vtable. */
		static const MessageHandler messageHandlers[];

		/** EmptyEntity's component offset table. */
		static const int componentOffsets[];
};

/** A specific entity. */
class ClientEntity: public Entity {
	public:
		/** Initialization parameters for ClientEntity. */
		struct Params {
			gentity_t* oldEnt;
			gclient_t* Client_clientData;
		};

		/** Default constructor of ClientEntity. */
		ClientEntity(Params params);

		/** Default destructor of ClientEntity. */
		virtual ~ClientEntity() = default;

		TeamComponent c_TeamComponent; /**< ClientEntity's TeamComponent instance. */
		ClientComponent c_ClientComponent; /**< ClientEntity's ClientComponent instance. */

	private:
		/** ClientEntity's message handler vtable. */
		static const MessageHandler messageHandlers[];

		/** ClientEntity's component offset table. */
		static const int componentOffsets[];
};

/** A specific entity. */
class SpectatorEntity: public Entity {
	public:
		/** Initialization parameters for SpectatorEntity. */
		struct Params {
			gentity_t* oldEnt;
			team_t Team_team;
			gclient_t* Client_clientData;
		};

		/** Default constructor of SpectatorEntity. */
		SpectatorEntity(Params params);

		/** Default destructor of SpectatorEntity. */
		virtual ~SpectatorEntity() = default;

		TeamComponent c_TeamComponent; /**< SpectatorEntity's TeamComponent instance. */
		ClientComponent c_ClientComponent; /**< SpectatorEntity's ClientComponent instance. */
		SpectatorComponent c_SpectatorComponent; /**< SpectatorEntity's SpectatorComponent instance. */

	private:
		/** SpectatorEntity's message handler vtable. */
		static const MessageHandler messageHandlers[];

		/** SpectatorEntity's component offset table. */
		static const int componentOffsets[];
};

/** A specific entity. */
class QPlayerEntity: public Entity {
	public:
		/** Initialization parameters for QPlayerEntity. */
		struct Params {
			gentity_t* oldEnt;
			gclient_t* Client_clientData;
		};

		/** Default constructor of QPlayerEntity. */
		QPlayerEntity(Params params);

		/** Default destructor of QPlayerEntity. */
		virtual ~QPlayerEntity() = default;

		TeamComponent c_TeamComponent; /**< QPlayerEntity's TeamComponent instance. */
		ClientComponent c_ClientComponent; /**< QPlayerEntity's ClientComponent instance. */
		HealthComponent c_HealthComponent; /**< QPlayerEntity's HealthComponent instance. */
		KnockbackComponent c_KnockbackComponent; /**< QPlayerEntity's KnockbackComponent instance. */

	private:
		/** QPlayerEntity's message handler vtable. */
		static const MessageHandler messageHandlers[];

		/** QPlayerEntity's component offset table. */
		static const int componentOffsets[];
};

/** A specific entity. */
class UPlayerEntity: public Entity {
	public:
		/** Initialization parameters for UPlayerEntity. */
		struct Params {
			gentity_t* oldEnt;
			gclient_t* Client_clientData;
		};

		/** Default constructor of UPlayerEntity. */
		UPlayerEntity(Params params);

		/** Default destructor of UPlayerEntity. */
		virtual ~UPlayerEntity() = default;

		TeamComponent c_TeamComponent; /**< UPlayerEntity's TeamComponent instance. */
		ClientComponent c_ClientComponent; /**< UPlayerEntity's ClientComponent instance. */
		HealthComponent c_HealthComponent; /**< UPlayerEntity's HealthComponent instance. */
		KnockbackComponent c_KnockbackComponent; /**< UPlayerEntity's KnockbackComponent instance. */

	private:
		/** UPlayerEntity's message handler vtable. */
		static const MessageHandler messageHandlers[];

		/** UPlayerEntity's component offset table. */
		static const int componentOffsets[];
};


#endif // CBSE_ENTITIES_H_
