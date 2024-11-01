//--------------------------------------------------------------------------
// Copyright (C) 2019-2021 Cisco and/or its affiliates. All rights reserved.
//
// This file may contain proprietary rules that were created, tested and
// certified by Sourcefire, Inc. (the "VRT Certified Rules") as well as
// rules that were created by Sourcefire and other third parties and
// distributed under the GNU General Public License (the "GPL Rules").
// The VRT Certified Rules contained in this file are the property of
// Sourcefire, Inc. Copyright 2005 Sourcefire, Inc. All Rights Reserved.
// The GPL Rules created by Sourcefire, Inc. are the property of
// Sourcefire, Inc. Copyright 2002-2005 Sourcefire, Inc. All Rights
// Reserved. All other GPL Rules are owned and copyrighted by their
// respective owners (please see www.snort.org/contributors for a list
// of owners and their respective copyrights). In order to determine what
// rules are VRT Certified Rules or GPL Rules, please refer to the VRT
// Certified Rules License Agreement.
//--------------------------------------------------------------------------
// os-windows_kb957280-active-directory-baseobject.cc author Brandon Stultz <brastult@cisco.com>

#include "main/snort_types.h"
#include "framework/so_rule.h"
#include "framework/cursor.h"
#include "protocols/packet.h"
#include "utils/util_ber.h"

//#define DEBUG
#ifdef DEBUG
#define DEBUG_SO(code) code
#else
#define DEBUG_SO(code)
#endif

using namespace snort;

static const char* rule_14646 = R"[Snort_SO_Rule](
alert tcp $EXTERNAL_NET any -> $HOME_NET 389 (
	msg:"OS-WINDOWS Active Directory malformed baseObject denial of service attempt";
	soid:14646;
	flow:to_server,established;
	content:"|30|",depth 1;
	byte_jump:0,0,from_beginning;
	ber_data:0x30;
	ber_skip:0x02;
	ber_data:0x63;
	so:eval;
	metadata:policy max-detect-ips drop;
	service:ldap;
	reference:cve,2008-4023;
	reference:url,technet.microsoft.com/en-us/security/bulletin/MS08-060;
	classtype:attempted-dos;
	gid:3; sid:14646; rev:8;
)
)[Snort_SO_Rule]";

static const unsigned rule_14646_len = 0;

static IpsOption::EvalStatus eval(void*, Cursor& c, Packet* p)
{
   const uint8_t *cursor_normal = c.start();

   BerReader ber(c);

   BerElement str;

   if(!ber.read(cursor_normal, str))
      return IpsOption::NO_MATCH;

   if(str.type != BerType::STRING)
      return IpsOption::NO_MATCH;

   if(str.length > 1200)
      return IpsOption::MATCH;

   return IpsOption::NO_MATCH;
}

static SoEvalFunc ctor(const char* /*so*/, void** pv)
{
    *pv = nullptr;
    return eval;
}

static const SoApi so_14646 =
{
    { // base api info
        PT_SO_RULE,
        sizeof(SoApi),
        SOAPI_VERSION,
        8, // version of this file
        API_RESERVED,
        API_OPTIONS,
        "14646", // name
        "OS-WINDOWS Active Directory malformed baseObject denial of service attempt", // help
        nullptr,  // mod_ctor
        nullptr   // mod_dtor
    },
    // so rule api info
    (uint8_t*)rule_14646,
    rule_14646_len,
    nullptr, // pinit
    nullptr, // pterm
    nullptr, // tinit
    nullptr, // tterm
    ctor, // ctor
    nullptr  // dtor
};

const BaseApi* pso_14646 = &so_14646.base;

