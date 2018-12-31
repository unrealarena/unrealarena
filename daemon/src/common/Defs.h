/*
 * Daemon BSD Source Code
 * Copyright (c) 2018, Unreal Arena
 * Copyright (c) 2013-2016, Daemon Developers
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#ifdef UNREALARENA
#define PRODUCT_NAME        "Unreal Arena"
/** Case, No spaces */
#define PRODUCT_NAME_UPPER  "UNREALARENA"
/** No case, No spaces */
#define PRODUCT_NAME_LOWER  "unrealarena"

#define PRODUCT_VERSION     "0.1-4"
#else
#define PRODUCT_NAME        "Unvanquished"
/** Case, No spaces */
#define PRODUCT_NAME_UPPER  "UNVANQUISHED"
/** No case, No spaces */
#define PRODUCT_NAME_LOWER  "unvanquished"

#define PRODUCT_VERSION     "0.51"
#endif

/** Default base package */
#define DEFAULT_BASE_PAK    PRODUCT_NAME_LOWER

/** URI scheme and server browser filter */
#ifdef UNREALARENA
#define GAMENAME_STRING     "qu"
#else
#define GAMENAME_STRING     "unv"
#endif
#define GAMENAME_FOR_MASTER PRODUCT_NAME_UPPER

#define MAX_MASTER_SERVERS  5
#ifdef UNREALARENA
#define MASTER1_SERVER_NAME "master.unrealarena.net"
#define MASTER2_SERVER_NAME ""
#define MASTER3_SERVER_NAME ""
#define MASTER4_SERVER_NAME ""
#define MASTER5_SERVER_NAME ""
#else
#define MASTER1_SERVER_NAME "master.unvanquished.net"
#define MASTER2_SERVER_NAME "master2.unvanquished.net"
#define MASTER3_SERVER_NAME ""
#define MASTER4_SERVER_NAME ""
#define MASTER5_SERVER_NAME ""
#endif

#define IRC_SERVER          "irc.freenode.org"
#define IRC_CHANNEL         "unv-lobby"

#ifdef UNREALARENA
#define WWW_BASEURL         "dl.unrealarena.net/pkg"
#else
#define WWW_BASEURL         "dl.unvanquished.net/pkg"
#endif

#define AUTOEXEC_NAME       "autoexec.cfg"

#define CONFIG_NAME         "autogen.cfg"
#define KEYBINDINGS_NAME    "keybindings.cfg"
#define TEAMCONFIG_NAME     "teamconfig.cfg"
#define SERVERCONFIG_NAME   "autogen_server.cfg"

#define UNNAMED_PLAYER      "UnnamedPlayer"
#define UNNAMED_SERVER      PRODUCT_NAME " " PRODUCT_VERSION " Server"

/** file containing our RSA public and private keys */
#define RSAKEY_FILE        "pubkey"

#endif // COMMON_DEFS_H_
