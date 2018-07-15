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
		virtual ~EmptyEntity();


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
		virtual ~ClientEntity();

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
			gclient_t* Client_clientData;
		};

		/** Default constructor of SpectatorEntity. */
		SpectatorEntity(Params params);

		/** Default destructor of SpectatorEntity. */
		virtual ~SpectatorEntity();

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
		virtual ~QPlayerEntity();

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
		virtual ~UPlayerEntity();

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
