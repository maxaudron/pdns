/*
    PowerDNS Versatile Database Driven Nameserver
    Copyright (C) 2002  PowerDNS.COM BV

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation
    

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
// $Id$ 
/* (C) 2002 POWERDNS.COM BV */
#ifndef DNS_HH
#define DNS_HH

#include "utility.hh"
#include "qtype.hh"
#include <time.h>
#include <sys/types.h>
class DNSBackend;

struct SOAData
{
  string qname;
  string nameserver;
  string hostmaster;
  uint32_t ttl;
  uint32_t serial;
  uint32_t refresh;
  uint32_t retry;
  uint32_t expire;
  uint32_t default_ttl;
  int domain_id;
  DNSBackend *db;
};


class RCode
{
public:
  enum { NoError=0, FormErr=1, ServFail=2, NXDomain=3, NotImp=4, Refused=5 };
};

class Opcode
{
public:
  enum { Query=0, IQuery=1, Status=2, Notify=4, Update=5 };
};


//! This class represents a resource record
class DNSResourceRecord
{
public:
  DNSResourceRecord() : qclass(1), priority(0), d_place(ANSWER) {};
  ~DNSResourceRecord(){};

  string serialize() const;
  int unSerialize(const string &str);

  // data
  
  QType qtype; //!< qtype of this record, ie A, CNAME, MX etc
  uint16_t qclass; //!< class of this record
  string qname; //!< the name of this record, for example: www.powerdns.com
  string content; //!< what this record points to. Example: 10.1.2.3
  uint16_t priority; //!< For qtype's that support a priority or preference. Currently only MX
  uint32_t ttl; //!< Time To Live of this record
  int domain_id; //!< If a backend implements this, the domain_id of the zone this record is in
  time_t last_modified; //!< For autocalculating SOA serial numbers - the backend needs to fill this in
  enum Place {QUESTION=0, ANSWER=1, AUTHORITY=2, ADDITIONAL=3}; //!< Type describing the positioning of a DNSResourceRecord within, say, a DNSPacket
  Place d_place; //!< This specifies where a record goes within the packet

  bool operator<(const DNSResourceRecord &b) const
  {
    if(qname < b.qname)
      return true;
    if(qname == b.qname)
      return(content < b.content);
    return false;
  }


private:
  string escape(const string &str) const;
};

# pragma pack ( push )
# pragma pack ( 1 )
struct dnsrecordheader
{
  uint16_t d_type;
  uint16_t d_class;
  uint32_t d_ttl;
  uint16_t d_clen;
};
# pragma pack( pop )
typedef enum  {
        ns_t_invalid = 0,       /* Cookie. */
        ns_t_a = 1,             /* Host address. */
        ns_t_ns = 2,            /* Authoritative server. */
        ns_t_md = 3,            /* Mail destination. */
        ns_t_mf = 4,            /* Mail forwarder. */
        ns_t_cname = 5,         /* Canonical name. */
        ns_t_soa = 6,           /* Start of authority zone. */
        ns_t_mb = 7,            /* Mailbox domain name. */
        ns_t_mg = 8,            /* Mail group member. */
        ns_t_mr = 9,            /* Mail rename name. */
        ns_t_null = 10,         /* Null resource record. */
        ns_t_wks = 11,          /* Well known service. */
        ns_t_ptr = 12,          /* Domain name pointer. */
        ns_t_hinfo = 13,        /* Host information. */
        ns_t_minfo = 14,        /* Mailbox information. */
        ns_t_mx = 15,           /* Mail routing information. */
        ns_t_txt = 16,          /* Text strings. */
        ns_t_rp = 17,           /* Responsible person. */
        ns_t_afsdb = 18,        /* AFS cell database. */
        ns_t_x25 = 19,          /* X_25 calling address. */
        ns_t_isdn = 20,         /* ISDN calling address. */
	ns_t_rt = 21,           /* Router. */
        ns_t_nsap = 22,         /* NSAP address. */
        ns_t_nsap_ptr = 23,     /* Reverse NSAP lookup (deprecated). */
        ns_t_sig = 24,          /* Security signature. */
        ns_t_key = 25,          /* Security key. */
        ns_t_px = 26,           /* X.400 mail mapping. */
        ns_t_gpos = 27,         /* Geographical position (withdrawn). */
        ns_t_aaaa = 28,         /* Ip6 Address. */
        ns_t_loc = 29,          /* Location Information. */
        ns_t_nxt = 30,          /* Next domain (security). */
        ns_t_eid = 31,          /* Endpoint identifier. */
        ns_t_nimloc = 32,       /* Nimrod Locator. */
        ns_t_srv = 33,          /* Server Selection. */
        ns_t_atma = 34,         /* ATM Address */
        ns_t_naptr = 35,        /* Naming Authority PoinTeR */
        ns_t_kx = 36,           /* Key Exchange */
        ns_t_cert = 37,         /* Certification record */
        ns_t_a6 = 38,           /* IPv6 address (deprecates AAAA) */
        ns_t_dname = 39,        /* Non-terminal DNAME (for IPv6) */
        ns_t_sink = 40,         /* Kitchen sink (experimentatl) */
        ns_t_opt = 41,          /* EDNS0 option (meta-RR) */
        ns_t_tsig = 250,        /* Transaction signature. */
        ns_t_ixfr = 251,        /* Incremental zone transfer. */
        ns_t_axfr = 252,        /* Transfer zone of authority. */
        ns_t_mailb = 253,       /* Transfer mailbox records. */
        ns_t_maila = 254,       /* Transfer mail agent records. */
        ns_t_any = 255,         /* Wildcard match. */
};

#ifndef WIN32
#include <endian.h>
#else
#define BYTE_ORDER 1
#define LITTLE_ENDIAN 1
#endif

struct dnsheader {
        unsigned        id :16;         /* query identification number */
#if BYTE_ORDER == BIG_ENDIAN
                        /* fields in third byte */
        unsigned        qr: 1;          /* response flag */
        unsigned        opcode: 4;      /* purpose of message */
        unsigned        aa: 1;          /* authoritive answer */
        unsigned        tc: 1;          /* truncated message */
        unsigned        rd: 1;          /* recursion desired */
                        /* fields in fourth byte */
        unsigned        ra: 1;          /* recursion available */
        unsigned        unused :1;      /* unused bits (MBZ as of 4.9.3a3) */
        unsigned        ad: 1;          /* authentic data from named */
        unsigned        cd: 1;          /* checking disabled by resolver */
        unsigned        rcode :4;       /* response code */
#endif
#if BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN
                        /* fields in third byte */
        unsigned        rd :1;          /* recursion desired */
        unsigned        tc :1;          /* truncated message */
        unsigned        aa :1;          /* authoritive answer */
        unsigned        opcode :4;      /* purpose of message */
        unsigned        qr :1;          /* response flag */
                        /* fields in fourth byte */
        unsigned        rcode :4;       /* response code */
        unsigned        cd: 1;          /* checking disabled by resolver */
        unsigned        ad: 1;          /* authentic data from named */
        unsigned        unused :1;      /* unused bits (MBZ as of 4.9.3a3) */
        unsigned        ra :1;          /* recursion available */
#endif
                        /* remaining bytes */
        unsigned        qdcount :16;    /* number of question entries */
        unsigned        ancount :16;    /* number of answer entries */
        unsigned        nscount :16;    /* number of authority entries */
        unsigned        arcount :16;    /* number of resource entries */
};


#define L theL()
extern time_t s_starttime;

#endif
