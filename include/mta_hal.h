/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2023 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**
* @file mta_hal.h
* @brief The mta_hal provides an interface to get/set Media Terminal Adapter features
*/


#ifndef __MTA_HAL_H__
#define __MTA_HAL_H__

#include <stdint.h>
#include <netinet/in.h>

/**********************************************************************
               CONSTANT DEFINITIONS
**********************************************************************/
/**
* @defgroup mta_hal MTA HAL
*
* This module provides the function call prototypes and structure definitions used for the  MTA hardware abstraction layer.
*
* @defgroup MTA_HAL_TYPES  MTA HAL Data Types
* @ingroup  mta_hal
*
* @defgroup MTA_HAL_APIS   MTA HAL  APIs
* @ingroup  mta_hal
*
**/


/**
 * @addtogroup MTA_HAL_TYPES
 *
 * The definitions below are the whole vocabulary of this interface: the scalar
 * aliases and the two status codes every entry point uses, the size limits that
 * bound its arguments, the structures a caller allocates or reads, and the
 * status enumerations it interprets. There is no opaque handle: a caller
 * constructs or inspects these types directly.
 *
 * The five scalar aliases (`ULONG`, `CHAR`, `UCHAR`, `BOOLEAN`, `INT`) and the
 * three constants (`TRUE`, `FALSE`, `ENABLE`) exist so that this interface does
 * not depend on the caller having its own equivalent definitions. Every one is
 * wrapped in `#ifndef`, so a definition the caller established before including
 * this header wins; a caller that does so is responsible for keeping its own
 * definition compatible with the one given here, because the declarations and
 * structures below are expressed in terms of these names.
 *
 * @{
 */

#ifndef ULONG
/** Alias for `unsigned long`. Carries counts, indices, identifiers, byte sizes
 *  and durations throughout this interface. Its width is that of the target's
 *  `unsigned long`, which is 8 bytes on the 64-bit Linux targets this interface
 *  is built for and 4 bytes on a 32-bit target; a caller must not assume either. */
#define ULONG unsigned long
#endif

#ifndef CHAR
/** Alias for `char`. Used for the fixed-width text fields of the structures
 *  below and for the string buffers the entry points exchange. */
#define CHAR  char
#endif

#ifndef UCHAR
/** Alias for `unsigned char`. Underlies `BOOLEAN` and the octet view of
 *  `ANSC_IPV4_ADDRESS`. */
#define UCHAR unsigned char
#endif

#ifndef BOOLEAN
/** Single-octet truth value holding `TRUE` or `FALSE`. Passed by value to the
 *  interface's setters and by pointer to its getters. */
#define BOOLEAN  UCHAR
#endif

#ifndef INT
/** Alias for `int`. The return type of every status-returning entry point in
 *  this interface, carrying `RETURN_OK` or `RETURN_ERR`. */
#define INT   int
#endif

#ifndef TRUE
/** Boolean true: the non-zero value of a `BOOLEAN`. */
#define TRUE     1
#endif

#ifndef FALSE
/** Boolean false: the zero value of a `BOOLEAN`. */
#define FALSE    0
#endif

#ifndef ENABLE
/** Enabled state, numerically identical to `TRUE`. No declaration in this
 *  header references it; it is retained for callers written against it. */
#define ENABLE   1
#endif

#ifndef RETURN_OK
/** Success. The operation completed and every out-parameter documented for the
 *  call has been written. One of only two status codes this interface defines. */
#define RETURN_OK   0
#endif

#ifndef RETURN_ERR
/** Failure. The call did not succeed and no out-parameter may be relied on;
 *  this interface does not state how far the operation progressed before
 *  failing. It defines no other failure code and states no mapping from any
 *  particular condition to this one, so the return value identifies no cause. */
#define RETURN_ERR   -1
#endif

#ifndef IPV4_ADDRESS_SIZE
/** Number of octets in an IPv4 address, and the length of the `Dot` array in
 *  `ANSC_IPV4_ADDRESS`. */
#define  IPV4_ADDRESS_SIZE                          4
#endif

#ifndef MTA_HAL_SHORT_VALUE_LEN
/** Byte size of each short fixed-width text field of `MTAMGMT_MTA_CALLS`, which
 *  is where this interface uses it. It is the whole of the bound on such a
 *  field: this interface does not state whether a value occupying all 16 bytes
 *  carries a terminator, so a caller reads the field against this declared width
 *  rather than searching for one. */
#define  MTA_HAL_SHORT_VALUE_LEN   16
#endif

#ifndef MTA_HAL_LONG_VALUE_LEN
/** Byte size of a long text field, 64. The structures in this header spell that
 *  width as a literal rather than through this macro, so no declaration here
 *  references it; it is published for callers that size their own buffers to the
 *  interface's long field width. As with the short fields, this interface states
 *  no termination convention for a value that fills such a field. */
#define  MTA_HAL_LONG_VALUE_LEN   64
#endif

#ifndef ANSC_IPV4_ADDRESS
/** Expands to an anonymous union giving two views of one IPv4 address: `Dot`, an
 *  array of `IPV4_ADDRESS_SIZE` (4) octets in network byte order, and `Value`, a
 *  `uint32_t` holding the same four octets as a single integer. The two overlay
 *  the same storage, so writing one view changes the other; a caller reads
 *  whichever suits it and must not assume the integer view is in host byte order.
 *  Every IPv4-valued member of the structures below is declared with this macro. */
#define  ANSC_IPV4_ADDRESS                                                                  \
         union                                                                              \
         {                                                                                  \
            unsigned char           Dot[IPV4_ADDRESS_SIZE];                                 \
            uint32_t                Value;                                                  \
         }
#endif

/* DECT */

/** Maximum number of DECT handsets that may be registered against the MTA, 5.
 *  It is the upper bound on the number of `MTAMGMT_MTA_HANDSETS_INFO` entries
 *  `mta_hal_GetHandsets()` can report, and therefore the largest handset
 *  population a caller has to be able to present. */
#define DECT_MAX_HANDSETS 5

/**********************************************************************
                STRUCTURE DEFINITIONS
**********************************************************************/
/**
 * @brief Structure representing DECT (Digital Enhanced Cordless Telecommunications) module information for an MTA (Multimedia Terminal Adapter).
 *
 * This structure stores configuration and status information about the DECT module of an MTA, including registration status,
 * hardware and software versions, RFPI (Radio Fixed Part Identity), and authentication PIN. It's used for managing and monitoring
 * the DECT module associated with an MTA.
 *
 * @note Every text member is a fixed 64-byte field. `PIN` holds an authentication PIN,
 *       while `mta_hal_GetDectPIN()` and `mta_hal_SetDectPIN()` carry a PIN through a
 *       `char *` parameter for which this interface states a capacity of 128 bytes. It
 *       does not state that the field and that parameter are the same storage, and does
 *       not state what happens to this 64-byte field when a longer value is set, so the
 *       field's width must not be read as a bound on those calls.
 */
typedef struct _MTAMGMT_MTA_DECT
{
    ULONG RegisterDectHandset;    /**< Registration status of the DECT handset. Currently, these values are set to 0 in the CCSP code. */
    ULONG DeregisterDectHandset;  /**< Deregistration status of the DECT handset. Currently, these values are set to 0 in the CCSP code. */
    char HardwareVersion[64];     /*!< Hardware version of the DECT module, as text in a 64-byte field. This interface states no format for it and publishes no value meaning "unknown", so a caller compares it for equality or displays it rather than parsing or ordering it. */
    char RFPI[64];                /**< RFPI (Radio Fixed Part Identity) value of the DECT module from the EEPROM. Unique identifier for the DECT base station. */
    char SoftwareVersion[64];     /*!< Software version of the DECT module, as text in a 64-byte field, on the same terms as `HardwareVersion`: no format is stated and no value marks it unknown. */
    char PIN[64];                 /**< Authentication PIN for base module (CMBS) <-> handset communication. Used for securing communications between devices. */
} MTAMGMT_MTA_DECT, *PMTAMGMT_MTA_DECT;

/**
 * @brief Structure to hold the details of registered or subscribed MTA handsets.
 * 
 * This structure is used to store information about the handsets associated with an MTA (Multimedia Terminal Adapter).
 * It includes details such as the instance number, status, last active time, name, firmware version, and operating and supported TNs.
 *
 * @note Every text member is a fixed 64-byte field. `DECT_MAX_HANDSETS` (5)
 *       bounds how many instances of this structure can exist, and therefore how
 *       many entries `mta_hal_GetHandsets()` can report.
 */
typedef struct _MTAMGMT_MTA_HANDSETS_INFO
{
    ULONG InstanceNumber;                        /**< Instance number of the MTA Handset. Unique identifier for each handset. */
    BOOLEAN Status;                              /**< Status of the MTA Handset. Indicates if the handset is active or inactive. */
    char LastActiveTime[64];                     /**< Last Active Time of the MTA Handset. Records the last time the handset was active. */
    char HandsetName[64];                        /*!< Name the handset is configured with, as text in a 64-byte field. This interface does not say who assigns it, does not require it to be unique across handsets, and publishes no value meaning "unnamed", so a caller identifies a handset by `InstanceNumber` rather than by this member. */
    char HandsetFirmware[64];                    /*!< Firmware version running on the handset, as text in a 64-byte field. This interface states no format for it, so a caller compares it for equality or displays it rather than ordering it. */
    char OperatingTN[64];                        /**< Operating TN. Indicates the Telephone Number (TN) the handset is operating on. Currently, only TN1 is assigned to DECT phones. */
    char SupportedTN[64];                        /**< Supported TN. Indicates the Telephone Number (TN) supported by the handset. Currently, only TN1 is assigned to DECT phones. */
} MTAMGMT_MTA_HANDSETS_INFO, *PMTAMGMT_MTA_HANDSETS_INFO;

/**
 * @brief Structure representing DHCP information for an MTA (Multimedia Terminal Adapter).
 * 
 * This structure contains details about DHCP (Dynamic Host Configuration Protocol) information,
 * including IP addresses, subnet mask, gateway, DNS information, lease times, and other DHCP options.
 *
 * @note The address members are `ANSC_IPV4_ADDRESS` unions rather than text.
 *       `BootFileName` is a 256-byte field; every other text member is 64 bytes.
 */
typedef struct _MTAMGMT_MTA_DHCP_INFO
{
    ANSC_IPV4_ADDRESS IPAddress;               /**< IP Address assigned to the MTA. */
    CHAR BootFileName[256];                     /**< Boot file name received from the DHCP server. */
    CHAR FQDN[64];                             /*!< Fully qualified domain name obtained with the IPv4 lease, as text in a 64-byte field. This interface does not state whether it names the MTA itself or the server that supplied the lease, states no format beyond it being a domain name, and publishes no value meaning "none supplied" - so a caller must not read an empty field as a distinct outcome. */
    ANSC_IPV4_ADDRESS SubnetMask;              /**< Subnet mask for the IP address. */
    ANSC_IPV4_ADDRESS Gateway;                 /**< Default gateway IP address. */
    ULONG LeaseTimeRemaining;                  /**< Remaining lease time in seconds. */
    CHAR RebindTimeRemaining[64];              /**< Remaining time to rebind in seconds. */
    CHAR RenewTimeRemaining[64];               /**< Remaining time to renew in seconds. */
    ANSC_IPV4_ADDRESS PrimaryDNS;              /**< Primary DNS server IP address. */
    ANSC_IPV4_ADDRESS SecondaryDNS;            /**< Secondary DNS server IP address. */
    CHAR DHCPOption3[64];                      /*!< Value of the custom DHCP option 3 carried with the MTA's IPv4 lease, as text in a 64-byte field. This interface does not say whether the value originates with the DHCP server or with local configuration, and states no encoding for it - whether the bytes are rendered as text, as hex or as an address - and publishes no value meaning "the option was absent", so a caller establishes the encoding with its implementation and must not read an empty field as a distinct outcome. The three option members below carry the same caveats. */
    CHAR DHCPOption6[64];                      /*!< Value of the custom DHCP option 6, on the same terms as `DHCPOption3`. */
    CHAR DHCPOption7[64];                      /*!< Value of the custom DHCP option 7, on the same terms as `DHCPOption3`. */
    CHAR DHCPOption8[64];                      /*!< Value of the custom DHCP option 8, on the same terms as `DHCPOption3`. Note that the same four member names appear in `MTAMGMT_MTA_DHCPv6_INFO`, where DHCPv6 numbers these options differently, so the values are not interchangeable between the two structures. */
    CHAR PCVersion[64];                        /*!< Version string obtained with the lease, as text in a 64-byte field. This interface does not expand the abbreviation "PC", does not say which component the version describes, and states no format for it, so a caller treats the contents as an opaque vendor string. */
    CHAR MACAddress[64];                       /**< MAC address of the MTA. */
    ANSC_IPV4_ADDRESS PrimaryDHCPServer;       /**< Primary DHCP server IP address. */
    ANSC_IPV4_ADDRESS SecondaryDHCPServer;     /**< Secondary DHCP server IP address. */
} MTAMGMT_MTA_DHCP_INFO, *PMTAMGMT_MTA_DHCP_INFO;

/**
 * @brief Structure representing DHCPv6 information for an MTA (Multimedia Terminal Adapter).
 * 
 * This structure contains details about DHCPv6 (Dynamic Host Configuration Protocol for IPv6) information,
 * including IPv6 addresses, prefixes, gateways, DNS information, lease times, and other DHCP options.
 *
 * @note Unlike the IPv4 structure above, every address here is text: each is
 *       sized by `INET6_ADDRSTRLEN`, which the `<netinet/in.h>` this header
 *       includes supplies. `BootFileName` is a 256-byte field; every other text
 *       member is 64 bytes.
 */
typedef struct _MTAMGMT_MTA_DHCPv6_INFO
{
    CHAR IPV6Address[INET6_ADDRSTRLEN];                 /**< IPv6 Address assigned to the MTA. */
    CHAR BootFileName[256];                             /**< Boot file name received from the DHCPv6 server. */
    CHAR FQDN[64];                                     /*!< Fully qualified domain name obtained with the IPv6 lease, as text in a 64-byte field, with the same caveats as the IPv4 structure's member of the same name: this interface does not say which entity it names and publishes no value meaning "none supplied". */
    CHAR Prefix[INET6_ADDRSTRLEN];                     /**< Network prefix associated with the IPv6 address. */
    CHAR Gateway[INET6_ADDRSTRLEN];                    /**< Default gateway IPv6 address. */
    ULONG LeaseTimeRemaining;                          /**< Remaining lease time in seconds. */
    CHAR RebindTimeRemaining[64];                      /**< Remaining time to rebind in seconds*/
    CHAR RenewTimeRemaining[64];                       /**< Remaining time to renew in seconds */
    CHAR PrimaryDNS[INET6_ADDRSTRLEN];                 /**< Primary DNS server IPv6 address. */
    CHAR SecondaryDNS[INET6_ADDRSTRLEN];               /**< Secondary DNS server IPv6 address. */
    CHAR DHCPOption3[64];                              /*!< Value of the custom DHCP option 3 carried with the MTA's IPv6 lease, as text in a 64-byte field. As in the IPv4 structure, this interface does not say where the value originates and states no encoding and publishes no value meaning "the option was absent"; and because DHCPv6 numbers its options independently of DHCPv4, this member is not the IPv6 counterpart of `MTAMGMT_MTA_DHCP_INFO::DHCPOption3`. The three option members below carry the same caveats. */
    CHAR DHCPOption6[64];                              /*!< Value of the custom DHCP option 6, on the same terms as the member above. */
    CHAR DHCPOption7[64];                              /*!< Value of the custom DHCP option 7, on the same terms as the member above. */
    CHAR DHCPOption8[64];                              /*!< Value of the custom DHCP option 8, on the same terms as the member above. */
    CHAR PCVersion[64];                                /*!< Version string obtained with the lease, as text in a 64-byte field. This interface does not expand the abbreviation "PC", does not say which component the version describes, and states no format for it, so a caller treats the contents as an opaque vendor string. */
    CHAR MACAddress[64];                               /**< The telephony IPv6 MAC address for this device. */
    CHAR PrimaryDHCPv6Server[INET6_ADDRSTRLEN];        /**< Primary DHCPv6 server IPv6 address. */
    CHAR SecondaryDHCPv6Server[INET6_ADDRSTRLEN];      /**< Secondary DHCPv6 server IPv6 address. */
} MTAMGMT_MTA_DHCPv6_INFO, *PMTAMGMT_MTA_DHCPv6_INFO;

/**
 * @brief Structure representing the service flow for an MTA (Multimedia Terminal Adapter).
 *
 * This structure contains detailed information about the service flow, including parameters for QoS (Quality of Service),
 * bandwidth allocation, traffic scheduling, and more. It is used to define the characteristics of a specific service flow
 * for an MTA, allowing for management and configuration of network traffic.
 *
 * @note `ServiceClassName` is a 256-byte field, `Direction` 16 bytes and
 *       `TrafficType` 64 bytes. `mta_hal_GetServiceFlow()` returns an array of
 *       these, one element per service flow.
 */
typedef struct _MTAMGMT_MTA_SERVICE_FLOW
{
    ULONG SFID;                              /**< Service Flow ID. Unique identifier for the service flow. */
    CHAR ServiceClassName[256];              /**< Name of the service class. Used to identify the type of service the flow is associated with. */
    CHAR Direction[16];                      /**< Direction of the service flow. Can be 'Upstream' or 'Downstream'. */
    ULONG ScheduleType;                      /*!< Scheduling mechanism the flow is served with. This interface publishes no set of values for it and names no external enumeration, so a caller cannot map a value to a scheduling type from this header alone and must establish the encoding with its implementation. */
    BOOLEAN DefaultFlow;                     /*!< TRUE when this element is the default service flow, FALSE otherwise. This interface does not state whether exactly one element of the array `mta_hal_GetServiceFlow()` returns carries TRUE, so a caller must not rely on finding precisely one. */
    ULONG NomGrantInterval;                  /*!< Nominal interval between grants for this flow. This interface states no unit for the value and no valid range beyond that of `ULONG`, and publishes no value meaning "not applicable to this scheduling type" - so a caller establishes the unit with its implementation before comparing or converting it. The eight numeric members below are stated on the same terms. */
    ULONG UnsolicitGrantSize;                /*!< Size of a grant issued without a request. No unit is stated. */
    ULONG TolGrantJitter;                    /*!< Largest deviation from the nominal grant interval the flow tolerates. No unit is stated. */
    ULONG NomPollInterval;                   /*!< Nominal interval between polls for this flow. No unit is stated. */
    ULONG MinReservedPkt;                    /*!< Minimum reserved packet size for the flow. No unit is stated. */
    ULONG MaxTrafficRate;                    /*!< Peak traffic rate the flow is allowed. No unit is stated. */
    ULONG MinReservedRate;                   /*!< Minimum data rate reserved for the flow. No unit is stated. */
    ULONG MaxTrafficBurst;                   /*!< Largest burst of traffic the flow is allowed. No unit is stated. */
    CHAR TrafficType[64];                    /**< Type of traffic. Can be 'SIP', 'RTP', or other types depending on the application. */
    ULONG NumberOfPackets;                   /**< Packet count. Number of packets that have been processed by this service flow. */
} MTAMGMT_MTA_SERVICE_FLOW, *PMTAMGMT_MTA_SERVICE_FLOW;

/**
 * @brief Structure representing call details for an MTA (Multimedia Terminal Adapter).
 *
 * This structure stores comprehensive information about individual calls, including codec details, call timings,
 * signal quality metrics, jitter buffer configurations, and packet statistics, among other parameters.
 * It provides a detailed snapshot of both the local and remote parameters of a call, aiding in quality analysis and troubleshooting.
 */
typedef struct _MTAMGMT_MTA_CALLS
{
    CHAR Codec[64];                                    /*!< Name of the codec the local end used for this call, as text in a 64-byte field. This interface publishes no set of permitted names and no value meaning "unknown", so a caller compares the string against names it has established with its implementation rather than against a vocabulary defined here. */
    CHAR RemoteCodec[64];                              /*!< Name of the codec the far end used for this call, with the same representation and the same absence of a defined vocabulary as `Codec`. It need not equal `Codec`: this interface does not state that the two ends negotiated the same codec. */
    CHAR CallStartTime[64];                            /*!< Time at which the call started, as text in a 64-byte field. This interface states no format, no time zone and no epoch for it, and publishes no value meaning "unknown", so a caller must not parse it or order calls by it until it has established the format with its implementation. */
    CHAR CallEndTime[64];                              /*!< Time at which the call ended, in the same unspecified text format as `CallStartTime`. This interface does not state what the member holds while a call is still in progress, so its content is not a reliable test of whether the call has ended. */
    CHAR CWErrorRate[MTA_HAL_SHORT_VALUE_LEN];         /*!< Rate of code word errors reported for this call, as text in a 16-byte field. The neighbouring `CWErrors` reports a count of code word errors; this interface does not state what this member expresses that count as - it gives no denominator, no interval over which the rate is computed, no unit and no scale - and it publishes no value meaning "not measured". It is not a ratio of signal to noise: that quantity is the separate `SNR` member below. A caller therefore renders or forwards the text and establishes the representation with its implementation before comparing, averaging or thresholding it. */
    CHAR PktLossConcealment[MTA_HAL_SHORT_VALUE_LEN];  /*!< Packet loss concealment for the local end of this call, as text in a 16-byte field. Concealment is the treatment applied to compensate for packets that did not arrive, not a measure of how many were lost, and this interface does not state what the member reports about it: it names no quantity, no unit, no scale and no interval, publishes no set of permitted values, and defines no value meaning "not measured" or "none applied". Loss itself is reported elsewhere in this structure - `LossRate` as a fraction scaled by 256 and `PacketLoss` as a count - so a caller must not read this member as either of those, and establishes what it holds with its implementation before interpreting it. */
    BOOLEAN JitterBufferAdaptive;                      /*!< TRUE when the local end used an adaptive jitter buffer for this call, FALSE when it used a fixed one - the interface names no third possibility and no value for "not known". `JitterBufRate` reports the buffer's adjustment rate; this interface does not state what that member holds when this one is FALSE, so a caller does not read the two as a pair without establishing that with its implementation. */
    BOOLEAN Originator;                                /**< Indicates if the local side is the originating side of the call. */
    ANSC_IPV4_ADDRESS RemoteIPAddress;                 /*!< IPv4 address of the far end of the call, in the two-view `ANSC_IPV4_ADDRESS` union: read it as the four octets of `Dot` in network byte order, or as `Value`, which is the same storage seen as a `uint32_t` and therefore not in host byte order. This interface publishes no value meaning "unknown" or "not applicable", so a caller cannot distinguish an unset member from the address 0.0.0.0 and must decide what an all-zero value means for its own purposes. */
    ULONG CallDuration;                                /**< Duration of the call in minutes. */
    CHAR CWErrors[MTA_HAL_SHORT_VALUE_LEN];            /**< Code Word Errors on this channel. */
    CHAR SNR[MTA_HAL_SHORT_VALUE_LEN];                 /*!< Signal-to-noise ratio reported for this call, as text in a 16-byte field. This interface does not say what the ratio is measured between, and - unlike the neighbouring `DownstreamPower` and `UpstreamPower`, which it states in dBmV - gives no unit and no valid range for it, so a caller establishes both with its implementation before comparing values. */
    CHAR MicroReflections[MTA_HAL_SHORT_VALUE_LEN];    /**< Micro Reflections. Return loss measurement. */
    CHAR DownstreamPower[MTA_HAL_SHORT_VALUE_LEN];     /**< Downstream power in dBmV. */
    CHAR UpstreamPower[MTA_HAL_SHORT_VALUE_LEN];       /**< Upstream power in dBmV. */
    CHAR EQIAverage[MTA_HAL_SHORT_VALUE_LEN];          /*!< The average of the call's EQI readings, as text in a 16-byte field. This interface does not expand the abbreviation EQI, name the quantity it measures, or state its unit, scale, direction of goodness or valid range, and publishes no value meaning "not measured" - so a caller renders or forwards the text and does not compare, average or threshold it until it has established those with its implementation. The three EQI members below are stated on the same terms and share the same representation. */
    CHAR EQIMinimum[MTA_HAL_SHORT_VALUE_LEN];          /*!< The smallest of the call's EQI readings. Which end of the undefined scale is the better one is not stated, so a caller must not read this as the worst reading of the call. */
    CHAR EQIMaximum[MTA_HAL_SHORT_VALUE_LEN];          /*!< The largest of the call's EQI readings, with the same caveat as `EQIMinimum` about which end of the scale is better. */
    CHAR EQIInstantaneous[MTA_HAL_SHORT_VALUE_LEN];    /*!< A single EQI reading rather than an aggregate. This interface does not state when it was taken, over what interval any of the four are computed, or how often readings occur, so a caller must not treat it as current at the moment of the call and must not assume it falls between `EQIMinimum` and `EQIMaximum`. */
    CHAR MOS_LQ[MTA_HAL_SHORT_VALUE_LEN];              /**< Mean Opinion Score of Listening Quality. Scale: 10-50. */
    CHAR MOS_CQ[MTA_HAL_SHORT_VALUE_LEN];              /**< Mean Opinion Score of Conversational Quality. Scale: 10-50. */
    CHAR EchoReturnLoss[MTA_HAL_SHORT_VALUE_LEN];      /**< Residual Echo Return Loss, in dB. */
    CHAR SignalLevel[MTA_HAL_SHORT_VALUE_LEN];         /**< Voice signal relative level, in dB. */
    CHAR NoiseLevel[MTA_HAL_SHORT_VALUE_LEN];          /**< Noise relative level, in dB. */
    CHAR LossRate[MTA_HAL_SHORT_VALUE_LEN];            /**< Fraction of RTP data packet loss * 256. */
    CHAR DiscardRate[MTA_HAL_SHORT_VALUE_LEN];         /**< Fraction of RTP data packet discarded * 256. */
    CHAR BurstDensity[MTA_HAL_SHORT_VALUE_LEN];        /**< Fraction of bursting data packet * 256. */
    CHAR GapDensity[MTA_HAL_SHORT_VALUE_LEN];          /**< Fraction of packets within inter-burst gap * 256. */
    CHAR BurstDuration[MTA_HAL_SHORT_VALUE_LEN];       /**< Mean duration of bursts, in milliseconds. */
    CHAR GapDuration[MTA_HAL_SHORT_VALUE_LEN];         /**< Mean duration of gaps, in milliseconds. */
    CHAR RoundTripDelay[MTA_HAL_SHORT_VALUE_LEN];      /**< Most recent measured RTD, in milliseconds. */
    CHAR Gmin[MTA_HAL_SHORT_VALUE_LEN];                /*!< Local gap threshold, which this interface describes through its remote counterpart `RemoteGmin` as the threshold used in burst calculations, as text in a 16-byte field. It states neither the unit nor the range of the value, and does not say which of the burst and gap members above it parameterises or how, so a caller reports it alongside those members rather than recomputing any of them from it. */
    CHAR RFactor[MTA_HAL_SHORT_VALUE_LEN];             /**< Voice quality evaluation for this RTP session. */
    CHAR ExternalRFactor[MTA_HAL_SHORT_VALUE_LEN];     /**< Voice quality evaluation for a segment on the network external to this RTP session. */
    CHAR JitterBufRate[MTA_HAL_SHORT_VALUE_LEN];       /**< Adjustment rate of the jitter buffer, in milliseconds. */
    CHAR JBNominalDelay[MTA_HAL_SHORT_VALUE_LEN];      /**< Nominal jitter buffer length, in milliseconds. */
    CHAR JBMaxDelay[MTA_HAL_SHORT_VALUE_LEN];          /**< Maximum jitter buffer length, in milliseconds. */
    CHAR JBAbsMaxDelay[MTA_HAL_SHORT_VALUE_LEN];       /**< Absolute maximum delay, in milliseconds. */
    CHAR TxPackets[MTA_HAL_SHORT_VALUE_LEN];           /**< Count of transmitted packets. */
    CHAR TxOctets[MTA_HAL_SHORT_VALUE_LEN];            /**< Count of transmitted octet packets. */
    CHAR RxPackets[MTA_HAL_SHORT_VALUE_LEN];           /**< Count of received packets. */
    CHAR RxOctets[MTA_HAL_SHORT_VALUE_LEN];            /**< Count of received octet packets. */
    CHAR PacketLoss[MTA_HAL_SHORT_VALUE_LEN];          /**< Count of lost packets. */
    CHAR IntervalJitter[MTA_HAL_SHORT_VALUE_LEN];      /**< Stat variance of packet interarrival time, in milliseconds. */
    CHAR RemoteIntervalJitter[MTA_HAL_SHORT_VALUE_LEN];        /**< Interval Jitter at the remote side. Statistical variance of packet inter-arrival time in milliseconds. */
    CHAR RemoteMOS_LQ[MTA_HAL_SHORT_VALUE_LEN];                /**< Mean Opinion Score (Listening Quality) at the remote side. A measure of voice call quality as experienced by the listener. */
    CHAR RemoteMOS_CQ[MTA_HAL_SHORT_VALUE_LEN];                /**< Mean Opinion Score (Conversational Quality) at the remote side. A measure of voice call quality in conversation. */
    CHAR RemoteEchoReturnLoss[MTA_HAL_SHORT_VALUE_LEN];        /**< Echo Return Loss at the remote side. Measures the amount of echo on the line. */
    CHAR RemoteSignalLevel[MTA_HAL_SHORT_VALUE_LEN];           /**< Signal Level at the remote side. Measures the strength of the signal. */
    CHAR RemoteNoiseLevel[MTA_HAL_SHORT_VALUE_LEN];            /**< Noise Level at the remote side. Measures the level of background noise. */
    CHAR RemoteLossRate[MTA_HAL_SHORT_VALUE_LEN];              /**< Loss Rate at the remote side. Fraction of RTP data packet loss. */
    CHAR RemotePktLossConcealment[MTA_HAL_SHORT_VALUE_LEN];    /*!< Packet loss concealment at the far end, on the same terms as `PktLossConcealment`: the member names the concealment applied rather than a measure of loss, and this interface states no quantity, unit, scale, interval or permitted value set for it, and no value meaning "not measured". Remote loss itself is reported by `RemoteLossRate`. This interface also does not state how the local end obtains the far end's value or over what interval it was produced, so a caller does not compare it with the local member without establishing that with its implementation. */
    CHAR RemoteDiscardRate[MTA_HAL_SHORT_VALUE_LEN];           /**< Discard Rate at the remote side. Fraction of RTP data packets discarded during transmission. */
    CHAR RemoteBurstDensity[MTA_HAL_SHORT_VALUE_LEN];          /**< Burst Density at the remote side. Fraction of packets in a burst compared to total packets. */
    CHAR RemoteGapDensity[MTA_HAL_SHORT_VALUE_LEN];            /**< Gap Density at the remote side. Fraction of packets within inter-burst gaps. */
    CHAR RemoteBurstDuration[MTA_HAL_SHORT_VALUE_LEN];         /**< Burst Duration at the remote side. The average duration of packet bursts is in milliseconds. */
    CHAR RemoteGapDuration[MTA_HAL_SHORT_VALUE_LEN];           /**< Gap Duration at the remote side. The average duration of gaps between packet bursts in milliseconds. */
    CHAR RemoteRoundTripDelay[MTA_HAL_SHORT_VALUE_LEN];       /**< Round-trip delay at the remote side. Most recent measured round-trip time in milliseconds. */
    CHAR RemoteGmin[MTA_HAL_SHORT_VALUE_LEN];                 /**< Gmin at the remote side. Specifies the gap threshold used in burst calculations. */
    CHAR RemoteRFactor[MTA_HAL_SHORT_VALUE_LEN];              /**< R-Factor at the remote side. Voice quality evaluation metric for the remote RTP session. */
    CHAR RemoteExternalRFactor[MTA_HAL_SHORT_VALUE_LEN];      /**< External R-Factor at the remote side. Voice quality evaluation for segments on the network external to the remote RTP session. */
    BOOLEAN RemoteJitterBufferAdaptive;                       /*!< TRUE when the far end used an adaptive jitter buffer, FALSE when it used a fixed one, on the same terms as `JitterBufferAdaptive`. This interface does not state how the local end learns it, so a caller must not assume the value was reported by the far end rather than inferred. */
    CHAR RemoteJitterBufRate[MTA_HAL_SHORT_VALUE_LEN];        /**< Adjustment rate of the remote jitter buffer in milliseconds. */
    CHAR RemoteJBNominalDelay[MTA_HAL_SHORT_VALUE_LEN];       /**< Nominal jitter buffer length at the remote side in milliseconds. */
    CHAR RemoteJBMaxDelay[MTA_HAL_SHORT_VALUE_LEN];           /**< Maximum jitter buffer length at the remote side in milliseconds. */
    CHAR RemoteJBAbsMaxDelay[MTA_HAL_SHORT_VALUE_LEN];        /**< Absolute maximum delay at the remote side in milliseconds. */
} MTAMGMT_MTA_CALLS, *PMTAMGMT_MTA_CALLS;

/**
 * @brief Structure representing line table information for an MTA (Multimedia Terminal Adapter).
 *
 * This structure stores information about a specific line on an MTA, including its status, test results, call information,
 * and other related details. It's used to monitor and manage the state and configuration of MTA lines.
 *
 * @note The four GR909 result members are 128-byte fields; `RingerEquivalency`
 *       and `CAName` are 64 bytes. `pCalls` and `CallsNumber` travel together:
 *       `CallsNumber` is the number of `MTAMGMT_MTA_CALLS` elements `pCalls`
 *       addresses, and `pCalls` must not be dereferenced when it is zero. This
 *       structure does not state which side allocates or releases `pCalls`.
 */
typedef struct _MTAMGMT_MTA_LINETABLE_INFO
{
    ULONG InstanceNumber;                         /*!< Identifier of this line within the MTA line table, and the value a caller passes as `InstanceNumber` to mta_hal_GetCalls() and mta_hal_ClearCalls(). Range is that of `ULONG`; this interface states neither the numbering base nor whether the values are contiguous, so a caller obtains one by reading the table with mta_hal_LineTableGetEntry() rather than by computing it. Note that it is not the zero-based `Index` argument of that call. */
    ULONG LineNumber;                             /*!< Number of the physical voice line this entry describes. `MTA_LINENUMBER` (8) is the line count this interface accounts for, but nothing here binds this member to that macro: the interface states neither its numbering base nor whether it equals `InstanceNumber` or the table index, and publishes no value meaning "unassigned". A caller therefore uses it for display and for correlation with vendor records, and uses `InstanceNumber` for the calls into this interface. */
    ULONG Status;                                 /**< Line status. 1 = OnHook; 2 = OffHook. */
    CHAR HazardousPotential[128];                 /**< Result of the HEMF (High Electric and Magnetic Fields) test. E.g., 'Passed', 'Not Started'. */
    CHAR ForeignEMF[128];                         /**< Result of the FEMF (Foreign Electromagnetic Fields) test. E.g., 'Passed', 'Not Started'. */
    CHAR ResistiveFaults[128];                    /**< Status of resistive faults test. E.g., 'Not Started'. */
    CHAR ReceiverOffHook[128];                    /**< Status of the receiver off-hook test. E.g., 'Not Started'. */
    CHAR RingerEquivalency[64];                   /**< Ringer equivalency number. Typically indicates the load a ringer places on the line. */
    CHAR CAName[64];                              /**< Circuit Assurance (CA) name associated with this line. */
    ULONG CAPort;                                 /**< Circuit Assurance (CA) port number. */
    ULONG MWD;                                    /**< Message Waiting Indicator. Indicates the presence of a voicemail or similar message. */
    ULONG CallsNumber;                            /*!< Number of `MTAMGMT_MTA_CALLS` elements the array at `pCalls` holds. Zero means the line has no call records, and `pCalls` must not be dereferenced in that case. This interface does not state whether the records are ordered, nor over what period they accumulate. */
    PMTAMGMT_MTA_CALLS pCalls;                    /*!< Address of an array of `CallsNumber` `MTAMGMT_MTA_CALLS` elements holding this line's call records, supplied by the implementation. It must not be dereferenced when `CallsNumber` is zero. This interface does not state which side allocates or releases it, nor how long it stays valid, so a caller neither frees it nor assumes it survives the next MTA HAL call, and copies any record it needs to keep. */
    ULONG CallsUpdateTime;                        /*!< Time at which the implementation last refreshed the records `pCalls` addresses. This interface states no epoch, unit or resolution for the value, so a caller may compare two readings of this member to detect a refresh but must not convert it to a wall-clock time, and no value here means "never updated". */
    ULONG OverCurrentFault;                       /**< Over-current fault status. 1 = Normal, 2 = Fault. */
} MTAMGMT_MTA_LINETABLE_INFO, *PMTAMGMT_MTA_LINETABLE_INFO;

/**
 * @brief Structure representing the call processing information for an MTA (Multimedia Terminal Adapter).
 *
 * This structure holds information about the call processing state, line card (LC) state, and loop current status of the MTA.
 * It's used to monitor and manage the call processing behavior and line card conditions of an MTA.
 *
 * @note All three members are 64-byte text fields carrying one of the states
 *       listed against each. The interface does not define numeric equivalents,
 *       so a caller compares the strings.
 */
typedef struct _MTAMGMT_MTA_CALLP
{
    CHAR LCState[64];                /**< Line Card State. Indicates the current state of the line card. Possible values: 'In Use', 'Idle', 'Out of Service'. */
    CHAR CallPState[64];             /**< Call Processing State. Represents the current state of call processing. Possible values: 'In Service', 'OOS (Out of Service)'. */
    CHAR LoopCurrent[64];            /**< Loop Current Status. Represents the status of the loop current. Possible values: 'Normal', 'Boosted'. */
} MTAMGMT_MTA_CALLP, *PMTAMGMT_MTA_CALLP;

/**
 * @brief Structure representing the DSX (Digital Signal Cross-connect) log entries for an MTA (Multimedia Terminal Adapter).
 *
 * This structure stores log entries related to DSX operations, including time of the log, description of the event,
 * and other vendor-specific identifiers and levels. It's used for monitoring and debugging DSX operations.
 *
 * @note `Time` is a 64-byte field and `Description` a 128-byte field, and those
 *       widths are the whole of what this interface bounds: it does not state
 *       what an implementation does with a longer value, so a caller must not
 *       assume a log line is preserved in full, nor that it is truncated in any
 *       particular way. `ID` and `Level` are vendor-defined and this interface
 *       assigns them no fixed meaning.
 */
typedef struct _MTAMGMT_MTA_DSXLOG
{
    CHAR Time[64];              /*!< Time at which the DSX event was recorded, as text in a 64-byte field. This interface states no format for it - unlike `MTAMGMT_MTA_MTALOG_FULL::Time`, which gives "1998-05-14" as an example - so a caller displays it rather than parsing it, and does not assume the two log types agree on a format. */
    CHAR Description[128];      /**< Description of the log entry. Provides details about the DSX operation or event. */
    ULONG ID;                   /**< Identifier for the log entry. A unique value is provided by the vendor to identify the log entry. */
    ULONG Level;                /**< Log level of the entry. Specifies the severity or importance of the log.  */
} MTAMGMT_MTA_DSXLOG, *PMTAMGMT_MTA_DSXLOG;

/**
 * @brief Structure representing the full log entries for an MTA (Multimedia Terminal Adapter).
 *
 * This structure holds comprehensive information about log entries, including an index, event identifier,
 * event level, timestamp, and a detailed description of the event. It's designed for in-depth monitoring and
 * analysis of events in an MTA environment.
 *
 * @note `EventLevel` and `Time` are 64-byte fields. `pDescription` is a pointer
 *       rather than a field, so it carries no length bound and this structure
 *       does not state which side allocates or releases the text it addresses.
 */
typedef struct _MTAMGMT_MTA_MTALOG_FULL
{
    ULONG Index;                  /**< Index of the log entry. Unique identifier within the log. Example values: 1, 2. */
    ULONG EventID;                /**< Identifier of the event. A unique value to identify the type of event. Example values: 1, 2. */
    CHAR EventLevel[64];          /**< Level of the event. Represents the severity or importance of the event. Example values: "1", "2". */
    CHAR Time[64];                /**< Timestamp of the log entry. Indicates when the event occurred. Example format: "1998-05-14". */
    CHAR* pDescription;           /**< Pointer to the description of the log entry. Provides detailed information about the event. Example values: "this is a log for matLog1", "this is a log for matLog2". */
} MTAMGMT_MTA_MTALOG_FULL, *PMTAMGMT_MTA_MTALOG_FULL;

/**
 * @brief Structure representing battery information for an MTA (Multimedia Terminal Adapter).
 *
 * This structure stores detailed information about the battery associated with an MTA, including model number,
 * serial number, part number, and charger firmware revision. It's used to identify and manage the battery
 * component of an MTA.
 *
 * @note All four members are 32-byte text fields, which is the shortest text
 *       width this interface uses. The contents are vendor-assigned identifiers,
 *       so a caller must treat them as opaque strings rather than parse them.
 */
typedef struct _MTAMGMT_MTA_BATTERY_INFO
{
    CHAR ModelNumber[32];                  /**< Model number of the battery. Identifies the battery model. */
    CHAR SerialNumber[32];                 /**< Serial number of the battery. Unique identifier for the battery. */
    CHAR PartNumber[32];                   /**< Part number of the battery. Identifies the part specification. */
    CHAR ChargerFirmwareRevision[32];      /**< Charger firmware revision. Identifies the firmware version of the battery charger. */
} MTAMGMT_MTA_BATTERY_INFO, *PMTAMGMT_MTA_BATTERY_INFO;


/** Number of MTA voice lines this interface accounts for, 8. It is the total
 *  line count a caller needs in order to size a per-line buffer, such as the
 *  array it passes to `mta_hal_getLineRegisterStatus()`, whose `array_size`
 *  argument this interface describes as the total line number. No declaration in
 *  this header references the macro itself, so an implementation is not obliged
 *  by it to report exactly eight line-table entries: the authoritative entry
 *  count for the line table is whatever `mta_hal_LineTableGetNumberOfEntries()`
 *  reports. */
#define MTA_LINENUMBER 8

/**
 * @brief Represents the MTA Operational Status.
 *
 * This enumeration is used to distinguish between different MTA Operational status/
 * Config file status/ DHCP status/ line register status.
 *
 * The same five values serve four different questions - operational status, config
 * file status, DHCP status and per-line register status - so a value carries no
 * meaning on its own: it is read against the call that produced it. Five
 * declarations report it - `mta_hal_getDhcpStatus()` (twice, once per address
 * family), `mta_hal_getConfigFileStatus()`, `mta_hal_getLineRegisterStatus()`,
 * `mta_hal_getMtaOperationalStatus()` and the
 * `mta_hal_getLineRegisterStatus_callback` typedef.
 *
 * @note This interface does not specify which transitions between these values
 *       are legal, nor in what order they occur; the values are readable status,
 *       not a state machine a caller may drive or predict.
 */
typedef  enum {
	MTA_INIT=0,         /*!< The subject of the question has not started. Read against mta_hal_getMtaOperationalStatus() it means the MTA is not yet operational; this interface does not state what it means for the config-file, DHCP or line-register questions, so a caller treats it as "not started" for whichever of the four it asked. */
	MTA_START=1,        /*!< The subject of the question is under way and has neither completed nor failed. A caller polls the same call again rather than treating this as an outcome; this interface states no interval at which to poll and no bound on how long the value may persist. */
	MTA_COMPLETE=2,     /*!< The subject of the question completed successfully, and it is the only one of the five values that denotes success. Read against mta_hal_getMtaOperationalStatus() this interface glosses it as the MTA being operational; for the config-file, DHCP and line-register questions it offers no per-question gloss, so a caller reads it as success for whichever of the four it asked and no further. */
	MTA_ERROR=3,        /*!< The subject of the question failed. It is a successful answer, not a call failure: a caller distinguishes the two by the RETURN_OK or RETURN_ERR the call returned. This interface does not state why the failure occurred or whether it is retryable. */
	MTA_REJECTED=4      /*!< The subject of the question was refused rather than merely failing - a configuration file the MTA declined to accept is the case mta_hal_getConfigFileStatus() documents. Like MTA_ERROR it is a successful answer, and this interface does not state what a rejection means for the other three questions the enumeration serves. */
} MTAMGMT_MTA_STATUS;

/**
 * @brief Represents the MTA Provisioning Status.
 *
 * Reported by `mta_hal_getMtaProvisioningStatus()`. It answers only whether the
 * MTA has been provisioned, which is a narrower question than the five-valued
 * `MTAMGMT_MTA_STATUS` above answers, and each value carries a consequence for
 * the MTA IP address a caller will observe.
 */
typedef enum{
        MTA_PROVISIONED=0,      /**< MTA Provisioned. If status is MTA_PROVISIONED then a valid MTA IP should be obtained when Device.DeviceInfo.X_COMCAST-COM_MTA_IP is queried. */
        MTA_NON_PROVISIONED=1   /**< MTA Non Provisioned. If status is MTA_NON_PROVISIONED, then MTA IP should be 0.0.0.0. */
} MTAMGMT_MTA_PROVISION_STATUS;
/**
 * @}
 */

/**
 * @addtogroup MTA_HAL_APIS
 *
 * The entry points below are the whole of the MTA HAL contract. Five properties
 * hold across all of them, and each per-function block below states its own case
 * rather than relying on a reader having found this paragraph first.
 *
 * @par Lifecycle
 * `mta_hal_InitDB()` makes the shared databases available locally and
 * `mta_hal_start_provisioning()` starts IP provisioning for the lines; a caller
 * invokes both once during boot before it relies on any other entry point. This
 * interface declares no teardown, deinitialisation or close call of any kind, so
 * there is nothing for a caller to release and no way to return the
 * implementation to its pre-initialisation state.
 *
 * @par Status codes
 * Every entry point but two returns `INT`, carrying `RETURN_OK` or `RETURN_ERR`
 * and nothing else. Those two codes are the only ones this interface defines, and
 * it states no mapping from any particular condition to `RETURN_ERR`, so that code
 * identifies no cause: a caller must not read it as a rejected argument, as an
 * absent capability, or as a vendor or hardware fault. Where a caller must respond
 * differently to different causes, it has to establish the cause by other means,
 * such as re-reading a count or a status enumeration before retrying. The two exceptions are
 * `mta_hal_LineTableGetNumberOfEntries()`, which returns a `ULONG` count, and
 * `mta_hal_LineRegisterStatus_callback_register()`, which returns nothing.
 *
 * @par Battery calls in particular
 * A failed battery read tells a caller nothing about why it failed, so it cannot be
 * read as "no battery is fitted": with only `RETURN_ERR` available, this interface
 * states no mapping from any condition to the code. A caller that must know whether
 * a battery is present calls `mta_hal_BatteryGetInstalled()` and treats that
 * answer, not a failed capacity or status read, as the presence test.
 * `mta_hal_BatteryGetStatus()` is the one battery call whose published value domain
 * includes an absence marker, "Missing", which it reports on a successful call.
 *
 * @par Blocking and threading
 * The calls are synchronous: each returns only once the operation has completed
 * or failed, and this interface states that a call may block while the MTA
 * hardware is not ready. It is not thread safe - a caller must serialise its
 * calls, even though the calls may be issued from more than one process.
 *
 * @par Memory
 * A caller allocates and releases the memory it hands in, and an implementation
 * releases whatever it allocates internally. Where an entry point returns a
 * pointer the implementation itself produced, the ownership of that memory is
 * stated - or stated to be unspecified - in that entry point's own block below,
 * because it is a per-call property and this interface does not settle it
 * globally.
 *
 * @{
 */


/**********************************************************************************
 *
 *  MTA Subsystem level function prototypes
 *
**********************************************************************************/

/**
* @brief Retrieves the global information for all shared DBs and makes them accessible locally.
*
* This is the first call a caller makes into this interface. It brings the shared
* MTA databases into local reach so that the read and write entry points below have
* something to operate on. It takes no argument and reports no detail: the whole of
* its result is the returned status.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The shared databases are available locally and the rest of
*                      this interface may be used.
* @retval RETURN_ERR - The call failed. This interface does not state whether any part
*                      of the initialisation took effect, so no other entry point
*                      should be relied on. The return value identifies no cause, so
*                      it cannot distinguish an MTA that has not finished coming up
*                      from one that will not come up at all: a caller that retries
*                      decides how long to go on retrying on that basis rather than on
*                      a cause read out of this code.
*
* @pre None. This call is the precondition of the others, not the other way round.
* @post On success the shared databases are reachable locally for the lifetime of
*       the process. This interface declares no matching teardown call, so there is
*       nothing to release and no way to undo the effect.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not yet
*       ready, which at boot is exactly when a caller invokes it.
* @warning Not thread safe. Complete this call before issuing any other MTA HAL
*          call from any thread, and serialise MTA HAL calls thereafter.
* @see mta_hal_start_provisioning
*/
INT   mta_hal_InitDB(void);

/**
* @brief Reports the MTA's current IPv4 DHCP lease, addressing and option values.
*
* Fills one caller-supplied `MTAMGMT_MTA_DHCP_INFO` with the addresses, subnet mask,
* gateway, DNS servers, lease and rebind timers, boot file name and DHCP options the
* MTA obtained on its IPv4 interface. It is a snapshot: nothing in this interface
* notifies a caller when any of it changes, so a caller that tracks the lease
* re-reads it.
*
* @param[out] pInfo pointer to PMTAMGMT_MTA_DHCP_INFO structure that will hold all DHCP info for MTA, to be returned.
*                   \n The caller allocates the structure and retains ownership of
*                   it; the implementation writes through the pointer. This interface
*                   does not state whether the implementation retains the pointer
*                   after the call returns, so a caller keeps the structure valid
*                   rather than reading the return as permission to release it. Must
*                   not be NULL. Its
*                   address members are `ANSC_IPV4_ADDRESS` unions, not text.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pInfo` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pInfo`; it may have been partly
*                      written. This interface defines one failure code and does not
*                      state what it reports, so the return value identifies no
*                      cause: a caller must not read it as "no lease has been
*                      obtained", as "the argument was rejected", or as any other
*                      particular condition. Where the distinction matters it
*                      establishes provisioning state with
*                      mta_hal_getMtaProvisioningStatus() or DHCP state with
*                      mta_hal_getDhcpStatus() instead of inferring a cause here.
*
* @pre mta_hal_InitDB() has returned RETURN_OK. Values are only meaningful once the
*      MTA has been provisioned; before that a caller should expect failure or an
*      empty lease rather than treat the call as broken.
* @post On success every member of `*pInfo` has been written. On failure the
*       contents are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other
*          MTA HAL call, including calls made from another process.
* @see mta_hal_GetDHCPV6Info
*/
INT mta_hal_GetDHCPInfo(PMTAMGMT_MTA_DHCP_INFO pInfo);

/**
* @brief Reports the MTA's current IPv6 DHCPv6 lease, addressing and option values.
*
* The IPv6 counterpart of mta_hal_GetDHCPInfo(). It fills one caller-supplied
* `MTAMGMT_MTA_DHCPv6_INFO` with the address, prefix, gateway, DNS servers, lease
* and rebind timers, boot file name and DHCP options obtained on the MTA's IPv6
* interface. Unlike the IPv4 structure, every address here is text sized by
* `INET6_ADDRSTRLEN` rather than a four-octet union.
*
* @param[out] pInfo pointer to PMTAMGMT_MTA_DHCPv6_INFO that will hold all DHCP info for MTA, to be returned.
*                   \n The caller allocates the structure and retains ownership of
*                   it; the implementation writes through the pointer. This interface
*                   does not state whether the implementation retains the pointer
*                   after the call returns, so a caller keeps the structure valid
*                   rather than reading the return as permission to release it. Must
*                   not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pInfo` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pInfo`. As with the IPv4 call, the
*                      single failure code identifies no cause, so failure must not be
*                      read as the MTA holding no IPv6 lease. A caller that needs to
*                      know consults mta_hal_getDhcpStatus(), whose second output
*                      reports IPv6 state specifically.
*
* @pre mta_hal_InitDB() has returned RETURN_OK. Meaningful only where the MTA is
*      provisioned in `MTA_IPV6` or `MTA_DUAL_STACK` mode; in `MTA_IPV4` mode a
*      caller should expect no IPv6 lease to report.
* @post On success every member of `*pInfo` has been written. On failure the
*       contents are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other
*          MTA HAL call, including calls made from another process.
* @see mta_hal_GetDHCPInfo
*/

INT mta_hal_GetDHCPV6Info(PMTAMGMT_MTA_DHCPv6_INFO pInfo);

/**
* @brief Reports how many entries the MTA line table currently holds.
*
* This is the bound a caller needs before walking the line table: valid indices for
* mta_hal_LineTableGetEntry() run from 0 up to one less than the value returned
* here. It is the only entry point in this interface that returns a value rather
* than a status code.
*
* @return The number of line-table entries, as an unsigned count. `MTA_LINENUMBER`
*         (8) is the line count this interface publishes, so a caller should expect
*         a value no greater than that, and 0 where no line table has been built
*         yet. Because the return type is unsigned, no negative value is available
*         to signal failure, and this interface does not specify how a failed read
*         is reported through the count - a caller must not read 0 as an error
*         indication, nor assume that a successful call has occurred.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post None. The call reports state and changes none.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The count can change between this call and a subsequent
*       mta_hal_LineTableGetEntry(), so an index validated against an earlier count
*       is not guaranteed valid later.
* @warning Not thread safe. The caller serialises this call against every other
*          MTA HAL call, including calls made from another process.
* @see mta_hal_LineTableGetEntry
*/
ULONG mta_hal_LineTableGetNumberOfEntries(void);

/**
* @brief Reads one line-table entry, giving the state, test results and call
*        summary of a single MTA voice line.
*
* Together with mta_hal_LineTableGetNumberOfEntries() this is how a caller walks the
* MTA's voice lines: obtain the count, then read each entry by index. The entry
* carries the line's hook status, its GR909 test results, its ringer and circuit
* assurance details, and a pointer to the calls associated with the line.
*
* @param[in] Index - index to the table entry, counting from 0.
*                    \n The valid range is bounded by what
*                    mta_hal_LineTableGetNumberOfEntries() reports, not by the width
*                    of `ULONG`: an index at or above that count identifies no entry.
*                    This interface states no other constraint on the argument.
* @param[out] pEntry - Structure variable containing MTA Line table entry info, to be returned.
*                     \n The caller allocates the `MTAMGMT_MTA_LINETABLE_INFO` and
*                     retains ownership of it; the implementation writes through the
*                     pointer. This interface does not state whether the
*                     implementation retains that pointer after the call returns, so
*                     a caller keeps the structure valid rather than reading the
*                     return as permission to release it. Must not be NULL. Its `pCalls`
*                     member is a pointer the implementation supplies, valid for
*                     `CallsNumber` elements; this interface does not state which
*                     side releases it, so a caller must not free it and must not
*                     assume it stays valid past the next MTA HAL call.
*
* @returns The status of the operation
* @retval RETURN_OK  - `*pEntry` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pEntry`. The return value
*                      identifies no cause, so failure must not be read as the index
*                      being out of range or as the argument having been rejected. A
*                      caller re-reads the entry count with
*                      mta_hal_LineTableGetNumberOfEntries() before retrying, rather
*                      than retrying the same index on an assumption about why the
*                      call failed.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `Index` is below the count
*      reported by mta_hal_LineTableGetNumberOfEntries().
* @post On success every member of `*pEntry` has been written. On failure the
*       contents are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other
*          MTA HAL call, including calls made from another process.
* @see mta_hal_LineTableGetNumberOfEntries, mta_hal_GetCalls
*/
INT   mta_hal_LineTableGetEntry(ULONG Index, PMTAMGMT_MTA_LINETABLE_INFO pEntry);

/**
* @brief Starts the GR909 loop-condition tests on one MTA line.
*
* GR909 is a set of electrical tests on the physical subscriber loop - hazardous
* potential, foreign electromotive force, resistive faults and receiver off-hook.
* This call starts them; it does not report their outcome. The results appear in the
* `HazardousPotential`, `ForeignEMF`, `ResistiveFaults` and `ReceiverOffHook` members
* of the line's `MTAMGMT_MTA_LINETABLE_INFO`, which a caller reads afterwards with
* mta_hal_LineTableGetEntry().
*
* @param[in] Index - it is an unsigned long integer that provides a line number to perform the GR909 diagnostics on.
*                    \n The valid range is bounded by the line count
*                    mta_hal_LineTableGetNumberOfEntries() reports, not by the width
*                    of `ULONG`. This interface states no other constraint.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The diagnostics were accepted for the identified line.
* @retval RETURN_ERR - The call failed. This interface does not state whether any part
*                      of the test sequence was started before it failed, so the state
*                      of the line and of the four GR909 members is unspecified: a
*                      caller must not read failure as the tests having been left
*                      alone, nor as the members still holding what they held before.
*                      It re-reads the entry with mta_hal_LineTableGetEntry() and
*                      treats the GR909 members as being of unknown freshness until a
*                      call has returned RETURN_OK.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `Index` identifies an existing
*      line-table entry.
* @post On success the tests have been started. This interface does not state that
*       they have completed when the call returns, so a caller polls the line-table
*       entry for results rather than assuming they are ready.
*
* @note Blocking: synchronous. This drives a physical line test rather than a
*       register read, so it may block for as long as the vendor's diagnostic takes,
*       and it may block while the MTA hardware is not ready. This interface requires
*       a procedure that risks failure on an unresponsive device to observe a
*       timeout, so a caller should not add an indefinite wait of its own.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @warning GR909 tests interrupt normal use of the line under test.
* @see mta_hal_LineTableGetEntry
*/
INT   mta_hal_TriggerDiagnostics(ULONG Index);

/**
* @brief Reports every DOCSIS service flow the MTA is using, with its QoS
*        parameters.
*
* Returns the upstream and downstream service flows carrying the MTA's voice
* traffic, each with its scheduling type, grant intervals, rate and burst limits and
* packet count. A caller reads the count first and then walks that many elements of
* the returned array.
*
* @param[out] Count - it is a pointer to an unsigned long integer through which the number of service flow entries is returned.
*                     \n The caller supplies the `ULONG` and the implementation
*                     writes the element count into it. Must not be NULL. A count of
*                     0 means no service flows exist, in which case `*ppCfg` must not
*                     be dereferenced.
* @param[out] ppCfg - Pointer to structure containing service flow info, to be returned.
*                     \n The caller supplies the address of a
*                     `PMTAMGMT_MTA_SERVICE_FLOW` and the implementation writes into
*                     it a pointer to an array of `*Count` elements. Must not be
*                     NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Count` and `*ppCfg` have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. The single failure code identifies
*                      no cause, so a caller should treat the whole result as absent
*                      rather than probing for a partial one or inferring why the call
*                      failed.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*Count` holds the element count and `*ppCfg` addresses that many
*       initialised elements.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning This interface does not specify whether the caller or the implementation
*          allocates the array returned through `ppCfg`, nor which of the two
*          releases it. A caller must not assume either: freeing memory the
*          implementation owns and leaking memory it does not are both possible
*          consequences of guessing. Establish the ownership rule with the
*          implementation before relying on it, and do not assume the array outlives
*          the next MTA HAL call.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
*/
INT   mta_hal_GetServiceFlow(ULONG* Count, PMTAMGMT_MTA_SERVICE_FLOW *ppCfg);

/**
* @brief Reports whether the DECT cordless subsystem is currently enabled.
*
* Reads the enabled state of the MTA's DECT base station - the subsystem cordless
* handsets attach to. This is the state mta_hal_DectSetEnable() writes.
*
* @param[out] pBool - It is a boolean pointer of 1 byte size, that stores the value for DECT enable, to be returned.
*                     \n The caller supplies the `BOOLEAN` and the implementation
*                     writes `TRUE` or `FALSE` into it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pBool` holds the current DECT enabled state.
* @retval RETURN_ERR - `*pBool` must not be read, and the DECT enabled state is
*                      unknown to the caller. This interface defines no "not
*                      supported" code and its single failure code identifies no
*                      cause, so failure must not be read either as a fault or as the
*                      absence of a DECT subsystem.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*pBool` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_DectSetEnable
*/
INT   mta_hal_DectGetEnable(BOOLEAN *pBool);

/**
* @brief Enables or disables the DECT cordless subsystem.
*
* Turns the MTA's DECT base station on or off. Disabling it takes away the service
* registered handsets depend on, so a caller should expect handset reachability to
* change as a result.
*
* @param[in] bBool - It is a boolean variable of 1-byte size, passed by value, that carries the state to set.
*                    \n `TRUE` enables the DECT subsystem, `FALSE` disables it. This
*                    is a value, not a pointer: there is nothing for the caller to
*                    allocate and nothing is written back.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The requested state has been applied.
* @retval RETURN_ERR - The call failed. This interface does not state whether the
*                      requested state was applied before it failed, so the DECT
*                      subsystem's state afterwards is unspecified: a caller must not
*                      read failure as the previous state still standing. It reads the
*                      state back with mta_hal_DectGetEnable() before relying on it
*                      either way.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success the DECT subsystem is in the requested state. This interface does
*       not persist the setting, so a caller that needs it across a restart stores it
*       itself and re-applies it.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_DectGetEnable
*/
INT mta_hal_DectSetEnable(BOOLEAN bBool);

/**
* @brief Reports whether DECT registration mode is currently enabled.
*
* Registration mode is the window during which a new handset may pair with the base
* station; it is a distinct setting from whether DECT itself is enabled. This reads
* the state mta_hal_DectSetRegistrationMode() writes.
*
* @param[out] pBool - It is a boolean pointer of 1 byte size, that stores whether registration mode is enabled, to be returned.
*                     \n The caller supplies the `BOOLEAN` and the implementation
*                     writes `TRUE` or `FALSE` into it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pBool` holds the current registration-mode state.
* @retval RETURN_ERR - `*pBool` must not be read. As with the other DECT getters, the
*                      return value identifies no cause, so failure says nothing about
*                      whether a DECT subsystem is present.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*pBool` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This interface does not state whether registration mode lapses on its own
*       after a period or persists until it is cleared, so a caller should not rely
*       on either behaviour.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_DectSetRegistrationMode
*/
INT mta_hal_DectGetRegistrationMode(BOOLEAN* pBool);

/**
* @brief Enables or disables DECT registration mode, the window in which a new
*        handset may pair.
*
* Opening registration mode lets a handset that is not yet known to the base station
* pair with it. It is a security-relevant state, because it is the interval during
* which an unpaired handset can join.
*
* @param[in] bBool - It is a boolean variable of 1-byte size, passed by value, carrying the registration mode to set.
*                    \n `TRUE` enables registration mode, `FALSE` disables it. This
*                    is a value, not a pointer: there is nothing for the caller to
*                    allocate and nothing is written back.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The requested registration mode has been applied.
* @retval RETURN_ERR - The call failed. This interface does not state whether the
*                      requested mode was applied, so the state of registration mode
*                      afterwards is unspecified. A failed disable in particular must
*                      not be read as registration mode having been left open or
*                      having been closed: a caller reads the state back with
*                      mta_hal_DectGetRegistrationMode() before treating the
*                      registration window as either.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success registration mode is in the requested state. This interface does
*       not persist the setting.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_DectGetRegistrationMode, mta_hal_GetDectPIN
*/
INT mta_hal_DectSetRegistrationMode(BOOLEAN bBool);

/**
* @brief Removes one registered DECT handset from the base station.
*
* Unpairs a handset, after which it can no longer place or receive calls through the
* MTA until it is registered again. The handset is identified by a vendor-assigned
* value rather than by a value this interface defines.
*
* @param[in] uValue - an unsigned long value, passed by value, identifying the handset to deregister.
*                     \n It is a vendor-specific identifier: this interface neither
*                     defines its meaning nor constrains it beyond the range of
*                     `ULONG`, and it does not state that the value is the
*                     `InstanceNumber` of the corresponding
*                     `MTAMGMT_MTA_HANDSETS_INFO`. A caller must therefore obtain the
*                     value it passes from the implementation rather than construct
*                     one. This is a value, not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The identified handset has been deregistered.
* @retval RETURN_ERR - The call failed, and the return value does not identify a
*                      cause. This interface does not state whether the identified
*                      handset was deregistered, so a caller must not read failure as
*                      the handset still being registered, nor as no handset having
*                      been removed. It re-reads the handset list with
*                      mta_hal_GetHandsets() to establish which handsets remain,
*                      rather than retrying blindly on the assumption that nothing
*                      happened.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and the DECT subsystem is enabled.
* @post On success the handset is no longer registered and is absent from the list
*       mta_hal_GetHandsets() reports.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_GetHandsets
*/
INT mta_hal_DectDeregisterDectHandset(ULONG uValue);

/**
* @brief Reports the DECT base station's identity, versions and authentication PIN.
*
* Fills one caller-supplied `MTAMGMT_MTA_DECT` with the base station's hardware and
* software versions, its RFPI as held in EEPROM, its authentication PIN and the
* handset registration and deregistration status counters.
*
* @param[out] pDect - Info of DECT. pDect is a pointer to structure PMTAMGMT_MTA_DECT.
*                     \n The caller allocates the structure and retains ownership of
*                     it; the implementation writes through the pointer. This
*                     interface does not state whether the implementation retains the
*                     pointer after the call returns, so a caller keeps the structure
*                     valid rather than reading the return as permission to release
*                     it. Must not be NULL. Every text member is a 64-byte
*                     field, `PIN` included.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pDect` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pDect`. The return value identifies
*                      no cause, so failure must not be read as the product having no
*                      DECT subsystem.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success every member of `*pDect` has been written; on failure the contents
*       are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The `RegisterDectHandset` and `DeregisterDectHandset` members are documented
*       as being set to 0 by the RDK-B code, so a caller should not read them as
*       counters of past activity.
* @warning `*pDect` includes the DECT authentication PIN in clear. A caller should
*          treat the whole structure as sensitive: do not log it and do not pass it
*          on unredacted.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_GetDectPIN
*/
INT mta_hal_GetDect(PMTAMGMT_MTA_DECT pDect);

/**
* @brief Reads the DECT base station's current authentication PIN.
*
* Returns the PIN a handset must present to pair with the base station. The same
* value appears as the `PIN` member of `MTAMGMT_MTA_DECT`.
*
* @param[out] pPINString - A caller-allocated character buffer of at least 128 writable bytes that receives the PIN as a zero-terminated string.
*                          \n The caller allocates it and retains ownership; the
*                          implementation writes into it. This interface does not
*                          state whether the implementation retains the pointer after
*                          the call returns, so a caller keeps the buffer valid rather
*                          than reading the return as permission to release or reuse
*                          it. Must not be NULL. 128 bytes is the capacity this
*                          interface states for the parameter, and the only figure it
*                          states for it. The related field `MTAMGMT_MTA_DECT::PIN` is
*                          declared 64 bytes, a narrower store for the same value, and
*                          this interface does not state how the two relate, nor what
*                          happens to that field when a longer value is set - so the
*                          field's width is not a bound on this buffer and a caller
*                          sizes the buffer by the 128 bytes. There is no argument
*                          through which a caller can declare a smaller capacity, and
*                          this interface names no constant for this buffer, so a
*                          caller must not supply less.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `pPINString` holds a zero-terminated PIN.
* @retval RETURN_ERR - The buffer content is undefined and must not be read as a PIN.
*                      The return value identifies no cause, so failure must not be
*                      read as the product having no DECT subsystem.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `pPINString` addresses at least 128
*      writable bytes.
* @post On success the buffer holds a zero-terminated string; on failure its contents
*       are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning The value returned is a credential. A caller should not log it and should
*          overwrite its buffer once it has finished with it.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_SetDectPIN, mta_hal_GetDect
*/
INT mta_hal_GetDectPIN(char* pPINString);

/**
* @brief Sets the DECT base station's authentication PIN.
*
* Replaces the PIN a handset must present to pair. Changing it does not unpair
* handsets that are already registered; it governs subsequent registrations.
*
* @param[in] pPINString - A caller-supplied zero-terminated string carrying the new DECT PIN.
*                         \n The caller owns the buffer. This interface does not state
*                         whether the implementation retains the pointer beyond the
*                         call, so a caller must not read the return as permission to
*                         release, reuse or overwrite the buffer: it either keeps the
*                         buffer valid and unchanged for as long as it goes on using
*                         this interface, or establishes the retention behaviour with
*                         its vendor first. That is a real constraint here rather than a
*                         formality, because the value is a credential a caller will
*                         want to overwrite, and overwriting storage an implementation
*                         may still be reading is unsafe. Retention is unspecified for
*                         every caller-supplied pointer in this interface, not only
*                         this one; no declaration here and no statement in the
*                         repository specification settles it.
*                         \n Must not be NULL. This interface states a capacity of 128
*                         bytes for this parameter, while the related field
*                         `MTAMGMT_MTA_DECT::PIN` is declared 64 bytes. It does not state
*                         how the two relate, nor what happens to that field when a
*                         longer value is set, so neither figure can be presented as the
*                         bound on the value a caller may set. It names no
*                         constant for a maximum length and does not state whether the
*                         implementation validates the length or the character set, so a
*                         caller should agree the acceptable length with the vendor
*                         rather than infer it from either width, and should check the
*                         value itself rather than rely on a rejection.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The new PIN has been applied.
* @retval RETURN_ERR - The call failed, and the return value does not identify a
*                      cause. This interface does not state whether the new PIN was
*                      stored, so which PIN the base station will require afterwards
*                      is unspecified: a caller must not read failure as the previous
*                      PIN still standing. It validates the value before the call and
*                      reads the PIN back with mta_hal_GetDectPIN() to establish which
*                      one is in force.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success the base station requires the new PIN for subsequent registrations.
*       This interface does not persist the setting on a caller's behalf.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning The value passed is a credential; a caller should not log it.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_GetDectPIN
*/
INT mta_hal_SetDectPIN(char* pPINString);

/**
* @brief Reports the DECT handsets registered against the MTA.
*
* Returns one `MTAMGMT_MTA_HANDSETS_INFO` per handset, carrying its instance number,
* active or inactive status, last-active time, name, firmware version and the
* telephone numbers it operates on and supports. `DECT_MAX_HANDSETS` (5) bounds how
* many can exist.
*
* @param[in] pulCount  - Unsigned long integer pointer that provides handset number. The value ranges from 0 to (2^32)-1.
*                    \n It is an input: the caller writes the value into its own
*                    `ULONG` before the call and the implementation reads it. Must
*                    not be NULL, and the value it addresses must be initialised for
*                    the same reason. This interface constrains that value no further
*                    than the range given above; `DECT_MAX_HANDSETS` (5) is the
*                    largest number of handsets it admits, so a value above five
*                    corresponds to no handset population this interface can report.
*                    \n What the direction rules out matters as much as what it
*                    establishes. Because the argument is an input, the number of
*                    records the implementation wrote is not reported back through it,
*                    and a caller must not read a count out of it after the call.
*                    That is where this entry point differs from the other
*                    array-returning ones - mta_hal_GetServiceFlow(),
*                    mta_hal_GetCalls(), mta_hal_GetDSXLogs() and
*                    mta_hal_GetMtaLog() - whose count arguments are defined as
*                    outputs carrying the number of entries returned. This interface
*                    states no relation between the value passed here and the extent
*                    of the array reached through `ppHandsets`, and declares no other
*                    output reporting that extent, so how many records a caller may
*                    read is not established by this interface: `DECT_MAX_HANDSETS`
*                    (5) is the only bound it publishes, and a caller establishes the
*                    exact convention with the implementation before indexing beyond
*                    the first record.
* @param[out] ppHandsets - Pointer to ppHandsets structure that contains Info of MTA handset.
*                          \n The caller supplies the address of a
*                          `PMTAMGMT_MTA_HANDSETS_INFO` and the implementation writes
*                          into it a pointer to an array of handset records. Must not
*                          be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The outputs have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. The return value identifies no
*                      cause, so a caller learns nothing from it about the argument it
*                      passed or about the presence of a DECT subsystem.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `*pulCount` has been initialised,
*      since the implementation reads it.
* @post On success `*ppHandsets` addresses the returned records. `pulCount` is an
*       input, so this interface establishes nothing about its value after the call
*       and a caller must not read one back from it.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning This interface does not specify whether the caller or the implementation
*          allocates the array returned through `ppHandsets`, nor which of the two
*          releases it. A caller must not assume either, and must not assume the array
*          outlives the next MTA HAL call.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_DectDeregisterDectHandset
*/
INT mta_hal_GetHandsets(ULONG* pulCount, PMTAMGMT_MTA_HANDSETS_INFO* ppHandsets);

/**
* @brief Reports the per-call voice-quality record for one line-table entry.
*
* Returns the calls associated with a single line, each as an `MTAMGMT_MTA_CALLS`
* record. Each record is a detailed quality snapshot - codecs, start and end times,
* MOS scores, jitter-buffer behaviour, packet counts and loss, and the same metrics
* as reported by the far end - which makes this the interface's principal diagnostic
* for call quality complaints.
*
* @param[in] InstanceNumber - an unsigned long value, passed by value, identifying the line-table entry whose calls are wanted.
*                             \n It is the `InstanceNumber` of an
*                             `MTAMGMT_MTA_LINETABLE_INFO`, so a caller obtains it
*                             from mta_hal_LineTableGetEntry() rather than assuming a
*                             base or a stride. This interface states no constraint
*                             beyond the range of `ULONG`.
* @param[out] Count - Pointer to an unsigned long through which the number of call records is returned.
*                     \n The caller supplies the `ULONG` and the implementation writes
*                     the element count into it. Must not be NULL. A count of 0 means
*                     the line has no call records, in which case `*ppCfg` must not be
*                     dereferenced.
* @param[out] ppCfg - Array with call info, to be returned.
*                     \n The caller supplies the address of a `PMTAMGMT_MTA_CALLS` and
*                     the implementation writes into it a pointer to an array of
*                     `*Count` records. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Count` and `*ppCfg` have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. The return value identifies no
*                      cause, so failure must not be read as the `InstanceNumber`
*                      being unknown; a caller re-reads the line table before retrying
*                      rather than acting on that assumption.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `InstanceNumber` identifies an
*      existing line-table entry.
* @post On success `*Count` holds the record count and `*ppCfg` addresses that many
*       initialised records.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The records are cleared by mta_hal_ClearCalls() for the same instance, so a
*       caller that collects statistics should read before clearing.
* @warning This interface does not specify whether the caller or the implementation
*          allocates the array returned through `ppCfg`, nor which of the two releases
*          it. A caller must not assume either, and must not assume the array outlives
*          the next MTA HAL call. The same open question applies to the `pCalls`
*          member of `MTAMGMT_MTA_LINETABLE_INFO`, which addresses the same kind of
*          record.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_ClearCalls, mta_hal_LineTableGetEntry
*/
INT   mta_hal_GetCalls(ULONG InstanceNumber, ULONG *Count, PMTAMGMT_MTA_CALLS *ppCfg);

/**
* @brief Reports the call-processing and line-card state of one voice line.
*
* Returns the line card state, the call processing state and the loop current status
* for a single line, as the three text members of `MTAMGMT_MTA_CALLP`. Where
* mta_hal_GetCalls() describes the quality of calls that happened, this describes
* whether the line is in a condition to carry one at all.
*
* @param[in]  LineNumber - an unsigned long value, passed by value, identifying the line whose call-processing status is wanted.
*                          \n It is the `LineNumber` of an
*                          `MTAMGMT_MTA_LINETABLE_INFO`, obtained from
*                          mta_hal_LineTableGetEntry(). Note that this is the line
*                          number rather than the instance number
*                          mta_hal_GetCalls() takes, and this interface does not state
*                          that the two are interchangeable. No constraint beyond the
*                          range of `ULONG` is stated.
* @param[out]  pCallp - Call processing information, to be returned.
*                       \n The caller allocates the `MTAMGMT_MTA_CALLP` and retains
*                       ownership of it; the implementation writes through the pointer.
*                       This interface does not state whether the implementation
*                       retains that pointer after the call returns, so a caller keeps
*                       the structure valid rather than reading the return as
*                       permission to release it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pCallp` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pCallp`. A caller should not
*                      interpret failure as the line being out of service; "Out of
*                      Service" is a value the structure carries on success.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `LineNumber` identifies an existing
*      line.
* @post On success all three members of `*pCallp` have been written; on failure the
*       contents are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The three members are text, and this interface defines no numeric equivalents,
*       so a caller compares the strings listed against each member.
* @warning Not thread safe. The caller serialises this call against every other MTA
*          HAL call, including calls made from another process.
* @see mta_hal_LineTableGetEntry
*/
INT   mta_hal_GetCALLP(ULONG LineNumber, PMTAMGMT_MTA_CALLP pCallp);

/**
* @brief Reports the accumulated DSX log entries.
*
* Returns the MTA's Digital Signal Cross-connect log as an array of
* `MTAMGMT_MTA_DSXLOG` records, each carrying a timestamp, a description and a
* vendor-defined identifier and level. Logging has to be enabled for entries to
* accumulate; see mta_hal_SetDSXLogEnable().
*
* @param[out] Count - Pointer to an unsigned long through which the number of log entries is returned.
*                     \n The caller supplies the `ULONG` and the implementation writes
*                     the entry count into it. Must not be NULL. A count of 0 means the
*                     log is empty - which is the expected result while logging is
*                     disabled - and `*ppDSXLog` must not be dereferenced.
* @param[out] ppDSXLog - array of DSX log entries, to be returned.
*                        \n The caller supplies the address of a `PMTAMGMT_MTA_DSXLOG`
*                        and the implementation writes into it a pointer to an array of
*                        `*Count` entries. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Count` and `*ppDSXLog` have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. A count of 0 lies within the range
*                      this interface documents for `Count`, so an empty log is
*                      reported through a successful call rather than through this
*                      code; failure therefore does not mean the log is empty, and it
*                      identifies no other cause either.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*Count` holds the entry count and `*ppDSXLog` addresses that many
*       initialised entries.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note `ID` and `Level` are vendor-defined; this interface assigns them no meaning, so
*       a caller must not map `Level` onto a syslog severity without knowing the
*       implementation.
* @warning This interface does not specify whether the caller or the implementation
*          allocates the array returned through `ppDSXLog`, nor which of the two
*          releases it. A caller must not assume either, and must not assume the array
*          outlives the next MTA HAL call.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetDSXLogEnable, mta_hal_ClearDSXLog
*/
INT   mta_hal_GetDSXLogs(ULONG *Count, PMTAMGMT_MTA_DSXLOG *ppDSXLog);

/**
* @brief Reports whether DSX logging is currently enabled.
*
* Reads the flag that governs whether the MTA accumulates DSX log entries at all. It
* is the state mta_hal_SetDSXLogEnable() writes.
*
* @param[out] pBool - It is a boolean pointer of 1 byte size, pointing to the value of enable, to be returned.
*                     \n The caller supplies the `BOOLEAN` and the implementation writes
*                     `TRUE` or `FALSE` into it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pBool` holds the current DSX logging state.
* @retval RETURN_ERR - `*pBool` must not be read; a caller must not treat failure as
*                      meaning logging is disabled.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*pBool` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_SetDSXLogEnable
*/
INT   mta_hal_GetDSXLogEnable(BOOLEAN *pBool);

/**
* @brief Enables or disables DSX logging.
*
* Turns accumulation of DSX log entries on or off. Enabling it is the precondition for
* mta_hal_GetDSXLogs() returning anything.
*
* @param[in] Bool - It is a boolean variable with 1 byte size, passed by value, carrying the state to set.
*                   \n `TRUE` enables DSX logging, `FALSE` disables it. This is a value,
*                   not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The requested logging state has been applied.
* @retval RETURN_ERR - The call failed. This interface does not state whether the
*                      requested state was applied, so whether DSX logging is on
*                      afterwards is unspecified: a caller must not read failure as
*                      the previous state still standing. A caller that depends on
*                      logging being on reads the state back with
*                      mta_hal_GetDSXLogEnable() rather than assuming either outcome.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success DSX logging is in the requested state. This interface does not
*       persist the setting, and does not state whether disabling logging discards
*       entries already accumulated.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetDSXLogEnable, mta_hal_ClearDSXLog
*/
INT   mta_hal_SetDSXLogEnable(BOOLEAN Bool);


/**
* @brief Clears the accumulated DSX log entries.
*
* Discards the entries mta_hal_GetDSXLogs() would otherwise return. The argument
* carries whether the clear is requested, so a caller drives the operation with the
* value rather than by the fact of calling.
*
* @param[in] Bool - It is a boolean variable with 1 byte size, passed by value, indicating whether the DSX log should be cleared.
*                   \n `TRUE` requests the clear. This interface does not state what
*                   passing `FALSE` does, so a caller should not use `FALSE` to mean
*                   "leave the log alone" and rely on that; it simply should not make
*                   the call. This is a value, not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The request was accepted.
* @retval RETURN_ERR - The call failed. This interface does not state whether any
*                      entry was discarded before it failed, so the contents of the
*                      log afterwards are unspecified: a caller must not read failure
*                      as the previous entries all still being present, and must not
*                      read it as the log having been left untouched. It re-reads the
*                      log with mta_hal_GetDSXLogs() to establish what remains.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success the DSX log holds no entries a subsequent mta_hal_GetDSXLogs() will
*       report.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning The operation is destructive and this interface offers no way to recover a
*          cleared entry, so a caller that needs the entries reads them with
*          mta_hal_GetDSXLogs() first.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetDSXLogs
*/
INT mta_hal_ClearDSXLog(BOOLEAN Bool) ;

/**
* @brief Reports whether call-signalling logging is currently enabled.
*
* Call-signalling logging is a separate facility from DSX logging: it records the
* signalling exchanges behind call setup and teardown rather than cross-connect
* events, and it has its own enable, clear and read controls.
*
* @param[out] pBool - It is a boolean pointer with 1 byte size, pointing to the value of enable, to be returned.
*                     \n The caller supplies the `BOOLEAN` and the implementation writes
*                     `TRUE` or `FALSE` into it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pBool` holds the current call-signalling logging state.
* @retval RETURN_ERR - `*pBool` must not be read; a caller must not treat failure as
*                      meaning logging is disabled.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*pBool` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_SetCallSignallingLogEnable
*/

INT mta_hal_GetCallSignallingLogEnable(BOOLEAN *pBool) ;

/**
* @brief Enables or disables call-signalling logging.
*
* Turns recording of call signalling on or off. Signalling logs carry dialled digits
* and endpoint identifiers, so enabling them has a privacy consequence a caller should
* weigh rather than leaving them on by default.
*
* @param[in] Bool - It is a boolean variable with 1 byte size, passed by value, carrying the state to set.
*                   \n `TRUE` enables call-signalling logging, `FALSE` disables it. This
*                   is a value, not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The requested logging state has been applied.
* @retval RETURN_ERR - The call failed. This interface does not state whether the
*                      requested state was applied, so whether call-signalling
*                      logging is on afterwards is unspecified. That matters most on a
*                      failed DISABLE: a caller must not read failure as the previous
*                      state standing in either direction, and must treat logging as
*                      possibly still recording until
*                      mta_hal_GetCallSignallingLogEnable() has reported otherwise.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success call-signalling logging is in the requested state. This interface
*       does not persist the setting.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetCallSignallingLogEnable, mta_hal_ClearCallSignallingLog
*/

INT mta_hal_SetCallSignallingLogEnable(BOOLEAN Bool) ;


/**
* @brief Clears the accumulated call-signalling log.
*
* Discards the recorded signalling exchanges. Because those records can contain dialled
* numbers, clearing them is also the interface's means of discarding that data.
*
* @param[in] Bool - It is a boolean variable with 1 byte size, passed by value, indicating whether the call-signalling log should be cleared.
*                   \n `TRUE` requests the clear. This interface does not state what
*                   passing `FALSE` does, so a caller that does not want the log cleared
*                   should not make the call rather than rely on `FALSE`. This is a
*                   value, not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The request was accepted.
* @retval RETURN_ERR - The call failed. This interface does not state whether any
*                      record was discarded before it failed, so the contents of the
*                      log afterwards are unspecified, and a caller must not read
*                      failure either as the records all still being present or as the
*                      log having been left untouched. Where the clear was made to
*                      discard call data, that data must be treated as possibly still
*                      held: this interface declares no read entry point for this log,
*                      so it offers a caller no way to establish what remains, and the
*                      outcome has to be established outside this interface.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success the call-signalling log holds no records.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning The operation is destructive and this interface offers no way to recover a
*          cleared record. It also provides no read entry point for this log, so a
*          caller cannot take a copy through this interface first.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_SetCallSignallingLogEnable
*/

INT mta_hal_ClearCallSignallingLog(BOOLEAN Bool) ;

/**
* @brief Reports the MTA event log in full.
*
* Returns the MTA's own event log as an array of `MTAMGMT_MTA_MTALOG_FULL` records,
* each carrying an index, an event identifier, a level, a timestamp and a pointer to
* the event description. This is the general MTA log, distinct from the DSX log
* mta_hal_GetDSXLogs() returns and from the call-signalling log.
*
* @param[out] Count - Pointer to an unsigned long through which the number of log entries is returned.
*                     \n The caller supplies the `ULONG` and the implementation writes
*                     the entry count into it. Must not be NULL. A count of 0 means the
*                     log is empty and `*ppCfg` must not be dereferenced.
* @param[out] ppCfg - array of log entries, to be returned.
*                     \n The caller supplies the address of a
*                     `PMTAMGMT_MTA_MTALOG_FULL` and the implementation writes into it a
*                     pointer to an array of `*Count` records. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Count` and `*ppCfg` have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. An empty log is reported through a
*                      successful call with a count of 0, so failure does not mean the
*                      log is empty; beyond that the return value identifies no
*                      cause.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*Count` holds the record count and `*ppCfg` addresses that many
*       initialised records.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Each record's `pDescription` is itself a pointer the implementation supplies,
*          so the array carries a second level of indirection. This interface does not
*          specify whether the caller or the implementation allocates either the array
*          returned through `ppCfg` or the description text, nor which of the two
*          releases them. A caller must not assume either, must not free them, and must
*          not assume they outlive the next MTA HAL call - copying any description it
*          needs to keep is the safe course.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetDSXLogs
*/
INT mta_hal_GetMtaLog(ULONG *Count, PMTAMGMT_MTA_MTALOG_FULL *ppCfg);

/**
* @brief Reports whether a backup battery is fitted.
*
* This is the presence test for the battery: it is the one call whose success reports
* absence as a value, `FALSE`, rather than leaving a caller to infer it. No other battery
* entry point publishes a value meaning "no battery" except
* mta_hal_BatteryGetStatus(), whose "Missing" serves the same purpose, and a failed call
* elsewhere in the group identifies no cause - so a caller establishes presence here
* first and interprets the rest of the group in that light.
*
* @param[out] Val - It is a boolean pointer with 1 byte size, which receives whether a battery is installed.
*                   \n The caller supplies the `BOOLEAN` and the implementation writes
*                   `TRUE` or `FALSE` into it. Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds `TRUE` if a battery is fitted, `FALSE` if not.
* @retval RETURN_ERR - `*Val` must not be read. Note the distinction a caller has to
*                      keep: `RETURN_OK` with `FALSE` means there is definitively no
*                      battery, whereas `RETURN_ERR` means presence is unknown.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note On a product with no battery support at all, this interface does not state
*       whether the call reports `RETURN_OK` with `FALSE` or `RETURN_ERR`, so a caller
*       must be prepared for either.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetInfo, mta_hal_BatteryGetStatus
*/
INT mta_hal_BatteryGetInstalled(BOOLEAN* Val);

/**
* @brief Reports the battery's design capacity - what it holds when new.
*
* This is the nameplate figure, the denominator against which
* mta_hal_BatteryGetActualCapacity() and mta_hal_BatteryGetRemainingCharge() are read.
* It does not change as the battery ages.
*
* @param[out] Val - Pointer to an unsigned long that receives the design capacity in mAVHour, the unit this interface states.
*                   \n The caller supplies the `ULONG` and the implementation writes the
*                   value into it. Must not be NULL. This interface does not constrain the
*                   value beyond the range of `ULONG`, and does not reserve a value to
*                   mean "unknown".
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds the design capacity.
* @retval RETURN_ERR - `*Val` must not be read, and the design capacity is unknown to
*                      the caller. The return value identifies no cause, so failure
*                      must not be read as an absent battery; a caller establishes
*                      presence with mta_hal_BatteryGetInstalled() instead of inferring
*                      it here.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and, for a meaningful answer, a battery
*      is fitted.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetInstalled, mta_hal_BatteryGetActualCapacity
*/
INT mta_hal_BatteryGetTotalCapacity(ULONG* Val);

/**
* @brief Reports the battery's present full-charge capacity, which falls as it ages.
*
* Where mta_hal_BatteryGetTotalCapacity() gives the design figure, this gives what the
* battery can actually hold now. The ratio between the two is the interface's measure of
* battery wear, and mta_hal_BatteryGetLife() is the vendor's own verdict on it.
*
* @param[out] Val - Pointer to an unsigned long that receives the present capacity in mAVHour, the unit this interface states.
*                   \n The caller supplies the `ULONG` and the implementation writes the
*                   value into it. Must not be NULL. Not constrained beyond the range of
*                   `ULONG`; in particular this interface does not state that it is
*                   always less than or equal to the design capacity.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds the present full-charge capacity.
* @retval RETURN_ERR - `*Val` must not be read. The return value identifies no cause,
*                      so failure must not be read as an absent battery.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and, for a meaningful answer, a battery
*      is fitted.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetTotalCapacity, mta_hal_BatteryGetLife
*/
INT mta_hal_BatteryGetActualCapacity(ULONG* Val);

/**
* @brief Reports the charge presently left in the battery.
*
* The instantaneous state of charge, in the same unit as the two capacity figures, so a
* caller can express it as a proportion of either.
*
* @param[out] Val - Pointer to an unsigned long that receives the remaining charge in mAVHour, the unit this interface states.
*                   \n The caller supplies the `ULONG` and the implementation writes the
*                   value into it. Must not be NULL. Not constrained beyond the range of
*                   `ULONG`.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds the remaining charge.
* @retval RETURN_ERR - `*Val` must not be read. The return value identifies no cause,
*                      so failure must not be read as an absent battery, and a caller
*                      must not substitute 0, which would read as a flat battery.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and, for a meaningful answer, a battery
*      is fitted.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The value changes as the battery charges or discharges, so it is a sample rather
*       than a stable property; mta_hal_BatteryGetStatus() reports which of those is
*       happening.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetRemainingTime, mta_hal_BatteryGetStatus
*/
INT mta_hal_BatteryGetRemainingCharge(ULONG* Val);

/**
* @brief Reports how long the battery is expected to last at the present rate of use.
*
* An estimate in minutes, derived by the implementation from the remaining charge and the
* current draw. It is the figure a caller surfaces as "time left on battery".
*
* @param[out] Val -  Pointer to an unsigned long that receives the estimated remaining time in minutes.
*                    \n The caller supplies the `ULONG` and the implementation writes the
*                    value into it. Must not be NULL. Not constrained beyond the range of
*                    `ULONG`, and this interface does not reserve a value to mean
*                    "indefinite" or "unknown", so a caller should not read 0 as either.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds the estimate in minutes.
* @retval RETURN_ERR - `*Val` must not be read. The return value identifies no cause,
*                      so failure must not be read as an absent battery.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and, for a meaningful answer, a battery
*      is fitted.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The estimate assumes the present rate of use continues; this interface does not
*       state how it is calculated, over what window, or how it behaves while the battery
*       is charging rather than discharging.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetRemainingCharge, mta_hal_BatteryGetPowerStatus
*/
INT mta_hal_BatteryGetRemainingTime(ULONG* Val);

/**
* @brief Reports the number of charge cycles the battery is rated for.
*
* The manufacturer's rated cycle count - a property of the battery type, not a count of
* cycles already used. This interface provides no way to read cycles consumed to date, so
* a caller cannot compute remaining cycles from it.
*
* @param[out] Val -  Pointer to an unsigned long that receives the maximum number of charge cycles.
*                    \n The caller supplies the `ULONG` and the implementation writes the
*                    value into it. Must not be NULL. Not constrained beyond the range of
*                    `ULONG`.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*Val` holds the rated cycle count.
* @retval RETURN_ERR - `*Val` must not be read, and the rated cycle count is unknown to
*                      the caller. The return value identifies no cause, so failure must
*                      not be read as an absent battery or as a rating the
*                      implementation does not hold.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and, for a meaningful answer, a battery
*      is fitted.
* @post On success `*Val` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetLife
*/
INT mta_hal_BatteryGetNumberofCycles(ULONG* Val);

/**
* @brief Reports whether the MTA is running from mains power or from its battery.
*
* This is the call that tells a caller a power cut is in progress: a value of "Battery"
* means mains has gone and the MTA is on backup, which is usually the trigger for
* conserving power and raising an alarm.
*
* @param[out] Val - It is a character pointer which stores the battery power status, to be returned. Possible values are of "AC", "Battery", or "Unknown"
*                   \n It is a zero-terminated string. The caller allocates the buffer
*                   and retains ownership; the implementation writes into it. This
*                   interface does not state whether the implementation retains the
*                   pointer after the call returns, so a caller keeps the buffer valid
*                   rather than reading the return as permission to release or reuse it.
*                   Must not be NULL. This interface does not state the
*                   minimum size the caller must provide, and provides no argument through
*                   which the caller can declare its capacity, so a caller must size the
*                   buffer to hold the longest value listed above with its terminator and
*                   must not assume the implementation checks the capacity before writing.
* @param[out] len - Pointer to an unsigned long conveying the length of the string.
*                   \n The caller supplies the `ULONG` and the implementation writes the
*                   length of the value it wrote into it - the direction this interface's
*                   own definition establishes by describing the length as being returned.
*                   Must not be NULL. This interface does NOT state whether the
*                   implementation also reads this argument on entry as a capacity, so a
*                   caller should initialise it to the size of `Val` before the call: that
*                   is correct if it is read and harmless if it is not. It is not stated
*                   whether the length counts the terminator.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `Val` holds a zero-terminated status string and `*len` its length.
* @retval RETURN_ERR - Neither output may be read. Note that "Unknown" is a successful
*                      answer, distinct from failure: a caller that must distinguish "the
*                      MTA does not know its power source" from "the call did not work"
*                      relies on the return value, not on the string.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `Val` addresses a writable buffer large
*      enough for the longest value listed.
* @post On success `Val` holds a zero-terminated string and `*len` has been written; on
*       failure both are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This is a poll. Nothing in this interface notifies a caller when the power source
*       changes, so a caller that must react promptly to a mains failure has to poll.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetStatus, mta_hal_BatteryGetRemainingTime
*/
INT mta_hal_BatteryGetPowerStatus(CHAR *Val, ULONG *len);

/**
* @brief Reports the vendor's verdict on whether the battery is serviceable.
*
* A two-valued health summary - "Good" or "Bad" - derived by the implementation rather
* than computed by the caller. It is coarser than mta_hal_BatteryGetLife(), which says
* whether replacement is called for.
*
* @param[out] Val - It is a character pointer which stores the battery condition, to be returned. Possible values are "Good" or "Bad".
*                   \n It is a zero-terminated string. The caller allocates the buffer
*                   and retains ownership; the implementation writes into it. This
*                   interface does not state whether the implementation retains the
*                   pointer after the call returns, so a caller keeps the buffer valid
*                   rather than reading the return as permission to release or reuse it.
*                   Must not be NULL. This interface does not state the
*                   minimum size the caller must provide, so a caller sizes the buffer for
*                   the longest value listed above with its terminator and must not assume
*                   the implementation checks the capacity.
* @param[out] len - Pointer to an unsigned long conveying the length of the string.
*                   \n Documented and used exactly as in mta_hal_BatteryGetPowerStatus():
*                   the implementation writes the length of the value it wrote, this
*                   interface does not state whether it also reads the argument on entry
*                   as a capacity, and a caller should therefore initialise it to the size
*                   of `Val`. Must not be NULL. Its direction is the one this interface's
*                   own definition establishes by describing the length as being returned,
*                   which is how the other three calls in this group treat their
*                   equivalent argument.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `Val` holds a zero-terminated condition string and `*len` its
*                      length.
* @retval RETURN_ERR - Neither output may be read. The two condition values this
*                      interface publishes are "Good" and "Bad", neither of which
*                      denotes an absent battery, and the return value identifies no
*                      cause - so failure must not be read as absence either.
*
* @pre mta_hal_InitDB() has returned RETURN_OK, `Val` addresses a writable buffer large
*      enough for the longest value listed, and a battery is fitted.
* @post On success `Val` holds a zero-terminated string and `*len` has been written; on
*       failure both are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This interface does not define the criteria behind "Good" and "Bad"; they are the
*       implementation's judgement, so a caller should not infer a threshold from them.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetLife, mta_hal_BatteryGetInstalled
*/
INT mta_hal_BatteryGetCondition(CHAR *Val, ULONG *len);

/**
* @brief Reports what the battery is doing right now - idle, charging, discharging,
*        missing or unknown.
*
* The battery's operational state, which is how a caller tells a charging battery from a
* discharging one when reading mta_hal_BatteryGetRemainingCharge().
*
* @param[out] Val - It is a character pointer that stores the battery status, to be returned. The values are: "Missing", "Idle", "Charging",
*                   \n "Discharging", or "Unknown".
*                   \n It is a zero-terminated string. The caller allocates the buffer and
*                   retains ownership; the implementation writes into it. This interface
*                   does not state whether the implementation retains the pointer after
*                   the call returns, so a caller keeps the buffer valid rather than
*                   reading the return as permission to release or reuse it.
*                   Must not be NULL. This interface does not state the
*                   minimum size the caller must provide, so a caller sizes the buffer for
*                   the longest value listed above - "Discharging" - with its terminator,
*                   and must not assume the implementation checks the capacity.
* @param[out] len - Pointer to an unsigned long conveying the length of the string.
*                   \n Documented and used exactly as in mta_hal_BatteryGetPowerStatus().
*                   Must not be NULL.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `Val` holds a zero-terminated status string and `*len` its length.
* @retval RETURN_ERR - Neither output may be read. Note that this call reports an absent
*                      battery successfully, as the value "Missing" - no other battery
*                      call in this interface publishes a value domain containing an
*                      equivalent marker - so a caller must not read failure here as
*                      absence.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `Val` addresses a writable buffer large
*      enough for the longest value listed.
* @post On success `Val` holds a zero-terminated string and `*len` has been written; on
*       failure both are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This interface does not specify which transitions between these states are
*       possible or in what order they occur; the value is a status to be read, not a
*       state machine a caller may predict.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetInstalled, mta_hal_BatteryGetPowerStatus
*/
INT mta_hal_BatteryGetStatus(CHAR* Val, ULONG *len);

/**
* @brief Reports whether the battery needs replacing.
*
* The end-of-life verdict - "Need Replacement" or "Good" - and therefore the value a caller
* surfaces as a service prompt. It is the actionable counterpart to
* mta_hal_BatteryGetCondition().
*
* @param[out] Val - It is a character pointer which stores the battery life status, to be returned. The values are: "Need Replacement", or "Good".
*                   \n It is a zero-terminated string. The caller allocates the buffer and
*                   retains ownership; the implementation writes into it. This interface
*                   does not state whether the implementation retains the pointer after
*                   the call returns, so a caller keeps the buffer valid rather than
*                   reading the return as permission to release or reuse it.
*                   Must not be NULL. This interface does not state the
*                   minimum size the caller must provide, so a caller sizes the buffer for
*                   the longest value listed above - "Need Replacement" - with its
*                   terminator, and must not assume the implementation checks the capacity.
* @param[out] len - Pointer to an unsigned long conveying the length of the string.
*                   \n Documented and used exactly as in mta_hal_BatteryGetPowerStatus().
*                   Must not be NULL. As with mta_hal_BatteryGetCondition(), its direction
*                   is the one this interface's own definition establishes by describing
*                   the length as being returned.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `Val` holds a zero-terminated life-status string and `*len` its
*                      length.
* @retval RETURN_ERR - Neither output may be read. A caller must not fall back to "Good"
*                      on failure, since that would suppress a replacement prompt the
*                      battery may in fact need.
*
* @pre mta_hal_InitDB() has returned RETURN_OK, `Val` addresses a writable buffer large
*      enough for the longest value listed, and a battery is fitted.
* @post On success `Val` holds a zero-terminated string and `*len` has been written; on
*       failure both are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetCondition, mta_hal_BatteryGetNumberofCycles
*/
INT mta_hal_BatteryGetLife(CHAR* Val, ULONG *len);


/**
* @brief Reports the battery's identity: model, serial and part numbers and charger
*        firmware revision.
*
* Identification rather than measurement - the values a caller quotes when ordering a
* replacement or matching a battery against a recall. None of it changes while the same
* battery is fitted.
*
* @param[out] pInfo - Structure variable of type PMTAMGMT_MTA_BATTERY_INFO containing the battery info, to be returned.
*                     \n The caller allocates the structure and retains ownership of it;
*                     the implementation writes through the pointer. This interface does
*                     not state whether the implementation retains that pointer after the
*                     call returns, so a caller keeps the structure valid rather than
*                     reading the return as permission to release it.
*                     Must not be NULL. All four members are 32-byte text fields.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pInfo` has been populated and may be read.
* @retval RETURN_ERR - Nothing may be read from `*pInfo`. The return value identifies no
*                      cause, so a caller checks mta_hal_BatteryGetInstalled() before
*                      treating failure as a fault, and must not read failure itself as
*                      absence.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and a battery is fitted.
* @post On success all four members of `*pInfo` have been written; on failure the contents
*       are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note The values are vendor-assigned identifiers; a caller treats them as opaque strings
*       rather than parsing them for meaning.
* @warning The serial number identifies a specific unit, so a caller should handle it with
*          the care due to device-identifying data.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetInstalled
*/
INT mta_hal_BatteryGetInfo(PMTAMGMT_MTA_BATTERY_INFO pInfo);

/**
* @brief Reports whether battery power-saving mode is enabled.
*
* Power-saving mode is the behaviour the MTA adopts to extend battery endurance during a
* mains failure. This call reports whether it is in force.
*
* @param[out] pValue - Pointer to an unsigned long that receives the power-saving mode status.
*                      \n The caller supplies the `ULONG` and the implementation writes
*                      the value into it. Must not be NULL. This interface defines two
*                      values, 1 for Enabled and 2 for Disabled, and defines no others -
*                      note in particular that 0 is not defined, so a caller must not read
*                      0 as "disabled" and should treat any value outside the two as
*                      unspecified by this interface. No enumeration is declared for these
*                      values.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*pValue` holds 1 (Enabled) or 2 (Disabled).
* @retval RETURN_ERR - `*pValue` must not be read, and the power-saving mode status is
*                      unknown to the caller. The return value identifies no cause, so
*                      failure must not be read as an absent battery or as a product
*                      without power-saving support.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*pValue` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This interface exposes no setter for power-saving mode, so a caller can observe it
*       but not change it here.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_BatteryGetPowerStatus
*/
INT mta_hal_BatteryGetPowerSavingModeStatus(ULONG *pValue);

/**
* @brief Reports how many times the MTA has been reset.
*
* A cumulative count over the device's life, useful as a stability indicator: a count that
* keeps climbing points at an MTA that is restarting repeatedly. It counts resets of the
* MTA as a whole, which mta_hal_devResetNow() is one way of causing.
*
* @param[out] resetcnt - Pointer to an unsigned long that receives the MTA reset count.
*                        \n The caller supplies the `ULONG` and the implementation writes
*                        the count into it. Must not be NULL. This interface does not
*                        constrain the value beyond the range of `ULONG` and does not state
*                        whether the count survives a factory reset or wraps.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*resetcnt` holds the reset count.
* @retval RETURN_ERR - `*resetcnt` must not be read; a caller must not substitute 0, which
*                      would read as a device that has never reset.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*resetcnt` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_Get_LineResetCount, mta_hal_devResetNow
*/

INT mta_hal_Get_MTAResetCount(ULONG *resetcnt);

/**
* @brief Reports how many times the MTA's voice lines have been reset.
*
* The line-level counterpart of mta_hal_Get_MTAResetCount(). A line reset count rising while
* the MTA reset count stays still points at a problem confined to the voice lines rather than
* to the device.
*
* @param[out] resetcnt - Pointer to an unsigned long that receives the line reset count.
*                        \n The caller supplies the `ULONG` and the implementation writes
*                        the count into it. Must not be NULL. This interface does not state
*                        whether the count is aggregated across all lines or reported per
*                        line, and offers no argument by which a caller could select a line,
*                        so a caller should read it as a single device-wide figure. Not
*                        constrained beyond the range of `ULONG`.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*resetcnt` holds the line reset count.
* @retval RETURN_ERR - `*resetcnt` must not be read; a caller must not substitute 0.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*resetcnt` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_Get_MTAResetCount
*/

INT mta_hal_Get_LineResetCount(ULONG *resetcnt);

/**
* @brief Discards the voice-quality call records held for one line.
*
* Clears the per-call statistics that mta_hal_GetCalls() reports for the identified line,
* which is how a caller establishes a fresh baseline before a test call or after acting on a
* quality complaint.
*
* @param[in] InstanceNumber - an unsigned long value, passed by value, identifying the line whose call records are to be cleared.
*                             \n It is the `InstanceNumber` of an
*                             `MTAMGMT_MTA_LINETABLE_INFO`, the same identifier
*                             mta_hal_GetCalls() takes, so a caller obtains it from
*                             mta_hal_LineTableGetEntry(). This is a value, not a pointer.
*                             This interface states no constraint beyond the range of
*                             `ULONG`, and does not define a value meaning "all lines", so
*                             a caller clears each line it wants cleared.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The call records for the identified line have been cleared.
* @retval RETURN_ERR - The call failed, and the return value does not identify a cause.
*                      This interface does not state whether any record was discarded
*                      before it failed, so the records held for the line afterwards are
*                      unspecified: a caller must not read failure as nothing having been
*                      cleared. It re-reads the line with mta_hal_GetCalls() to establish
*                      what remains before treating any figure as a baseline.
*
* @pre mta_hal_InitDB() has returned RETURN_OK and `InstanceNumber` identifies an existing
*      line-table entry.
* @post On success mta_hal_GetCalls() reports no records for that line until new calls occur.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning The operation is destructive and this interface offers no way to recover cleared
*          records, so a caller that needs them reads them with mta_hal_GetCalls() first.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetCalls
*/
INT mta_hal_ClearCalls(ULONG InstanceNumber);

/**
* @brief Reports the MTA's DHCP progress for IPv4 and IPv6 in one call.
*
* Gives the state of address acquisition on each family separately, which is what lets a
* caller diagnose a dual-stack MTA that has one family working and the other not. Both
* outputs are written on success even where the MTA is provisioned for only one family.
*
* @param[out] output_pIpv4status - ipv4 dhcp status from MTAMGMT_MTA_STATUS enumeration.
*                                  \n The caller supplies the `MTAMGMT_MTA_STATUS` and the
*                                  implementation writes one of its five values into it.
*                                  Must not be NULL. Read against this call: here
*                                  `MTA_COMPLETE` means IPv4 addressing is in place and
*                                  `MTA_START` that it is still in progress.
* @param[out] output_pIpv6status - ipv6 dhcp status from MTAMGMT_MTA_STATUS enumeration.
*                                  \n As above, for IPv6. Must not be NULL. In `MTA_IPV4`
*                                  provisioning mode a caller should not expect this to
*                                  reach `MTA_COMPLETE`.
*
* @returns The status of the operation.
* @retval RETURN_OK  - Both outputs have been written and may be read.
* @retval RETURN_ERR - Neither output may be read. This is a failure to REPORT the status,
*                      not a DHCP failure: a DHCP failure is reported successfully, as
*                      `MTA_ERROR` in the relevant output.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success both outputs have been written; on failure both are undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @note This interface does not specify which transitions between the status values are
*       legal or in what order they occur, so a caller polls for the value it needs rather
*       than predicting a sequence.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_GetDHCPInfo, mta_hal_GetDHCPV6Info
*/
INT mta_hal_getDhcpStatus(MTAMGMT_MTA_STATUS *output_pIpv4status, MTAMGMT_MTA_STATUS *output_pIpv6status);

/**
* @brief Reports how far the MTA has got with its configuration file.
*
* Provisioning fetches and applies a configuration file; this reports the state of that
* step. It is the call that separates "the MTA has no address" from "the MTA has an address
* but its configuration file was rejected".
*
* @param[out] poutput_status - MTA config file status from MTAMGMT_MTA_STATUS enumeration.
*                              \n The caller supplies the `MTAMGMT_MTA_STATUS` and the
*                              implementation writes one of its five values into it. Must
*                              not be NULL. Read against this call: `MTA_COMPLETE` means the
*                              configuration file has been applied, `MTA_REJECTED` that it
*                              was refused.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*poutput_status` holds the configuration file status.
* @retval RETURN_ERR - `*poutput_status` must not be read. A rejected configuration file is
*                      reported successfully as `MTA_REJECTED`, so failure here means the
*                      status could not be obtained at all.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*poutput_status` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_getMtaProvisioningStatus
*/
INT mta_hal_getConfigFileStatus(MTAMGMT_MTA_STATUS *poutput_status);

/**
* @brief Reports the registration status of every voice line in one call.
*
* Fills a caller-supplied array with one status per line, so a caller learns in a single call
* which lines have registered with the call server and which have not. This is the polled
* form of the same information the `mta_hal_getLineRegisterStatus_callback` delivers.
*
* @param[out] output_status_array - return array buffer for all line register status from MTAMGMT_MTA_STATUS enumeration.
*                                    \n The caller allocates an array of at least
*                                    `array_size` `MTAMGMT_MTA_STATUS` elements and retains
*                                    ownership of it; the implementation writes into it.
*                                    This interface does not state whether the
*                                    implementation retains the pointer after the call
*                                    returns, so a caller keeps the array valid rather than
*                                    reading the return as permission to release it. Must
*                                    not be NULL. This
*                                    interface does not state how many elements the
*                                    implementation writes, nor whether it writes fewer than
*                                    `array_size` when there are fewer lines, so a caller
*                                    should initialise the array before the call rather than
*                                    read an element it cannot confirm was written.
* @param[in] array_size - an int value, passed by value, giving the number of elements in the array, described by this interface as the total line number.
*                         \n The caller declares its own array's element count here, so the
*                         two arguments must agree: an `array_size` larger than the array
*                         invites a write beyond it. `MTA_LINENUMBER` (8) is the line count
*                         this interface publishes, and is the value a caller sizing for all
*                         lines uses. `int` is 4 bytes wide on the targets this interface is
*                         built for, and no negative value is meaningful.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The array has been populated for the lines the implementation
*                      reported.
* @retval RETURN_ERR - The array contents must not be relied on; they may have been partly
*                      written. The return value identifies no cause, so failure must not
*                      be read as `array_size` having been too small for the line count,
*                      nor as the array having been left untouched. A caller that sized
*                      the array itself re-checks that sizing against
*                      mta_hal_LineTableGetNumberOfEntries() rather than inferring it
*                      from this code.
*
* @pre mta_hal_InitDB() has returned RETURN_OK, and `output_status_array` addresses at least
*      `array_size` writable elements.
* @post On success the array holds line register statuses. This interface does not report how
*       many elements were written, so a caller cannot distinguish an untouched element from
*       one deliberately left at a status.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_LineRegisterStatus_callback_register, mta_hal_LineTableGetNumberOfEntries
*/
INT mta_hal_getLineRegisterStatus(MTAMGMT_MTA_STATUS *output_status_array, int array_size);

/**
* @brief Resets the MTA device immediately.
*
* Restarts the MTA, dropping any call in progress. It is a service-affecting operation, and
* the count it increments is the one mta_hal_Get_MTAResetCount() reports.
*
* @param[in] bResetValue - a boolean value of 1 byte, passed by value, requesting the reset.
*                          \n `TRUE` requests the reset. This interface does not state what
*                          passing `FALSE` does, so a caller that does not want a reset should
*                          not make the call rather than rely on `FALSE` being inert. This is
*                          a value, not a pointer.
*
* @returns The status of the operation.
* @retval RETURN_OK  - The reset was accepted. Note that acceptance is all this reports: the
*                      call returning does not establish that the MTA has come back.
* @retval RETURN_ERR - The call failed. This interface does not state whether the reset was
*                      initiated before it failed, so a caller must not read failure as the
*                      MTA still running as before: the device may reset regardless. It
*                      establishes what happened by polling mta_hal_getMtaOperationalStatus()
*                      once the MTA answers again, and by comparing the count
*                      mta_hal_Get_MTAResetCount() reports against the one it read before
*                      the call.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success the MTA resets. This interface does not state whether it returns before or
*       after the reset takes effect, nor how long the MTA is unavailable, so a caller should
*       expect subsequent calls to fail until the MTA is ready again and should re-establish
*       state by polling rather than assume continuity.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Service affecting: any call in progress is lost. A caller should confirm intent
*          before invoking this, since the interface offers no way to cancel it.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL call,
*          including calls made from another process.
* @see mta_hal_Get_MTAResetCount
*/
INT mta_hal_devResetNow(BOOLEAN bResetValue);

/**
* @brief Reports the MTA's overall operational status.
*
* The single summary answer to "is the MTA working": where mta_hal_getDhcpStatus() and
* mta_hal_getConfigFileStatus() report individual provisioning steps, this reports the MTA as
* a whole.
*
* @param[out] operationalStatus - Return buffer for operational status from MTAMGMT_MTA_STATUS enumeration.
*                                 \n The caller supplies the `MTAMGMT_MTA_STATUS` and the
*                                 implementation writes one of its five values into it. Must
*                                 not be NULL. Read against this call: `MTA_COMPLETE` means
*                                 the MTA is operational and `MTA_ERROR` that provisioning
*                                 failed.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*operationalStatus` holds the MTA's operational status.
* @retval RETURN_ERR - `*operationalStatus` must not be read. A non-operational MTA is
*                      reported successfully as `MTA_ERROR` or `MTA_INIT`, so failure here
*                      means the status itself could not be obtained.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*operationalStatus` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL call,
*          including calls made from another process.
* @see mta_hal_getMtaProvisioningStatus, mta_hal_getConfigFileStatus
*/
INT mta_hal_getMtaOperationalStatus(MTAMGMT_MTA_STATUS *operationalStatus);

/**
* @brief Reports whether the MTA has been provisioned.
*
* A two-valued answer, and the one that tells a caller what to expect of the MTA IP address:
* provisioned implies a valid address is obtainable, non-provisioned implies 0.0.0.0.
*
* @param[out] provisionStatus - return buffer for provision status from MTAMGMT_MTA_PROVISION_STATUS enumeration.
*                                \n The caller supplies the `MTAMGMT_MTA_PROVISION_STATUS`
*                                and the implementation writes `MTA_PROVISIONED` or
*                                `MTA_NON_PROVISIONED` into it. Must not be NULL. Note this
*                                is a different enumeration from the five-valued
*                                `MTAMGMT_MTA_STATUS` the neighbouring status calls use.
*
* @returns The status of the operation.
* @retval RETURN_OK  - `*provisionStatus` holds the provisioning status.
* @retval RETURN_ERR - `*provisionStatus` must not be read. A caller must not fall back to
*                      `MTA_NON_PROVISIONED`, because that is a definite answer this call did
*                      not give.
*
* @pre mta_hal_InitDB() has returned RETURN_OK.
* @post On success `*provisionStatus` has been written; on failure its value is undefined.
*
* @note Blocking: synchronous. It may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL call,
*          including calls made from another process.
* @see mta_hal_start_provisioning, mta_hal_getMtaOperationalStatus
*/
INT mta_hal_getMtaProvisioningStatus(MTAMGMT_MTA_PROVISION_STATUS *provisionStatus);

/**
 * @}
 */

/**
 * @addtogroup MTA_HAL_TYPES
 * @{
 */

/** Length in bytes of the DHCP option 122 sub-option 1 value, 4. It bounds
 *  `MTAMGMT_PROVISIONING_PARAMS::DhcpOption122Suboption1`, which is declared one
 *  byte longer, leaving room for a terminator after a value of the full bounded
 *  length; this interface does not state whether the implementation requires
 *  one. */
#define MTA_DHCPOPTION122SUBOPTION1_MAX          4
/** Length in bytes of the DHCP option 122 sub-option 2 value, 4. It bounds
 *  `MTAMGMT_PROVISIONING_PARAMS::DhcpOption122Suboption2` on the same terms. */
#define MTA_DHCPOPTION122SUBOPTION2_MAX          4
/** Length in bytes of the first DHCPv6 option 2171 CCC DSS identifier, 32. It
 *  bounds `MTAMGMT_PROVISIONING_PARAMS::DhcpOption2171CccV6DssID1`, whose actual
 *  length is carried separately in `DhcpOption2171CccV6DssID1Len`. */
#define MTA_DHCPOPTION122CCCV6DSSID1_MAX         32
/** Length in bytes of the second DHCPv6 option 2171 CCC DSS identifier, 32,
 *  bounding `DhcpOption2171CccV6DssID2` with its length in
 *  `DhcpOption2171CccV6DssID2Len`. */
#define MTA_DHCPOPTION122CCCV6DSSID2_MAX         32

/**
 * @brief Represents the MTA Provisioning IP Modes.
 *
 * Selects the address family or families the MTA lines are provisioned in. A
 * caller sets `MTAMGMT_PROVISIONING_PARAMS::MtaIPMode` to one of these values
 * before calling `mta_hal_start_provisioning()`; that field is declared `INT`
 * rather than as this enumeration, so the caller supplies the ordinal.
 */

typedef  enum {
   MTA_IPV4=0,          /**<  IPv4 mode */
   MTA_IPV6=1,          /**<  IPv6 mode */
   MTA_DUAL_STACK=2,    /**<  Dual stack mode */
} MTAMGMT_MTA_PROV_IP_MODE; // MTAMGMT_PROVISIONING_PARAMS.MtaIPMode

/**
 * @brief Structure to hold all the provisioning parameters for MTA.
 *
 * This is the single argument of `mta_hal_start_provisioning()`. It selects the
 * IP mode the lines are provisioned in and carries the DHCP option 122 and
 * option 2171 sub-option values the implementation is to use.
 *
 * @note Each of the four text members is declared one byte longer than its
 *       `MTA_DHCPOPTION122*_MAX` bound, which leaves room for a terminator after
 *       a value of the full bounded length; this interface does not state whether
 *       one is required. The two `*Len` members give the lengths of the
 *       corresponding `DhcpOption2171CccV6DssID*` values; the two option 122
 *       sub-option members have no length member and are bounded only by their
 *       declared size.
 */

typedef struct _MTAMGMT_PROVISIONING_PARAMS
{

INT  MtaIPMode;                                                         /*!<  Address family or families to provision the lines in. Takes one ordinal of MTAMGMT_MTA_PROV_IP_MODE: MTA_IPV4 (0), MTA_IPV6 (1) or MTA_DUAL_STACK (2). Declared INT rather than as the enumeration, so no value outside that set is rejected by the type and the caller is responsible for supplying one of the three. */
INT  DhcpOption2171CccV6DssID1Len;                                      /*!<  Number of bytes of DhcpOption2171CccV6DssID1 the caller has filled in, which the implementation reads instead of scanning the field. Declared INT, so the type rejects neither a negative value nor one above the MTA_DHCPOPTION122CCCV6DSSID1_MAX bound of 32; this interface states no default, does not say whether the count includes a terminating byte, and does not say what an out-of-range value does - so a caller sets it to the exact value length it wrote and establishes the terminator convention with its implementation. */
INT  DhcpOption2171CccV6DssID2Len;                                      /*!<  Number of bytes of DhcpOption2171CccV6DssID2 the caller has filled in, on the same terms as DhcpOption2171CccV6DssID1Len and against the MTA_DHCPOPTION122CCCV6DSSID2_MAX bound of 32. */
CHAR DhcpOption122Suboption1[MTA_DHCPOPTION122SUBOPTION1_MAX+1];        /**<  4 byte hex value ie. FFFFFFFF = "255.255.255.255". IPv4 addresses MUST be encoded as 4 binary octets in network  byte-order (high order byte first). */
CHAR DhcpOption122Suboption2[MTA_DHCPOPTION122SUBOPTION2_MAX+1];        /**<  4 byte hex value ie. FFFFFFFF = "255.255.255.255" */
CHAR DhcpOption2171CccV6DssID1[MTA_DHCPOPTION122CCCV6DSSID1_MAX+1];     /*!<  32 byte hex value. It is the first DHCPv6 option 2171 CCC DSS identifier the implementation is to provision with, written by the caller, whose length it declares in DhcpOption2171CccV6DssID1Len rather than by terminating the field. This interface states no default and does not say what the implementation does when the member is left empty. */
CHAR DhcpOption2171CccV6DssID2[MTA_DHCPOPTION122CCCV6DSSID2_MAX+1];     /*!<  32 byte hex value. It is the second such identifier, paired with DhcpOption2171CccV6DssID2Len on the same terms as the member above. */
}
MTAMGMT_PROVISIONING_PARAMS, *PMTAMGMT_MTA_PROVISIONING_PARAMS;

/**
 * @}
 */

/**
 * @addtogroup MTA_HAL_APIS
 * @{
 */

/**
* @brief Starts IP provisioning for all voice lines in the requested address mode.
*
* The second of the two boot-time calls, after mta_hal_InitDB(). It sets the MTA acquiring
* addresses and its configuration file for every line, in IPv4, IPv6 or dual-stack mode as
* the parameters select, carrying with it the DHCP option 122 and option 2171 values the
* implementation is to use. It starts the process; progress is then read through
* mta_hal_getDhcpStatus(), mta_hal_getConfigFileStatus() and
* mta_hal_getMtaProvisioningStatus().
*
* @param[in] pParameters - Pointer to a caller-populated MTAMGMT_PROVISIONING_PARAMS carrying the IP mode and DHCP option values to provision with.
*                          \n The caller allocates and fills the structure and retains
*                          ownership of it. This interface does not state whether the
*                          implementation retains the pointer beyond the call, so a caller
*                          should keep the structure valid until it has confirmed
*                          provisioning progressed rather than freeing it immediately. Must
*                          not be NULL. Set `MtaIPMode` to one ordinal of
*                          `MTAMGMT_MTA_PROV_IP_MODE` - `MTA_IPV4` (0), `MTA_IPV6` (1) or
*                          `MTA_DUAL_STACK` (2); the member is declared `INT`, so the type
*                          will not reject anything else. Each text member must fit its
*                          declared size, and the two `*Len` members must give the lengths
*                          of the corresponding `DhcpOption2171CccV6DssID*` values.
*
* @returns The status of the operation.
* @retval RETURN_OK  - Provisioning has been started. This does NOT mean the lines are
*                      provisioned: it means the process is under way, and a caller polls
*                      mta_hal_getMtaProvisioningStatus() for the outcome.
* @retval RETURN_ERR - The call failed, and the return value identifies no cause: it must
*                      not be read as the `MtaIPMode` having been rejected, as an
*                      option value having been too long, or as any other particular
*                      condition. This interface does not state whether provisioning was
*                      started before the call failed, so a caller must not assume it was
*                      not: it validates the structure it passed, then establishes what
*                      actually happened with mta_hal_getMtaProvisioningStatus() and
*                      mta_hal_getDhcpStatus() before issuing the call again.
*
* @pre mta_hal_InitDB() has returned RETURN_OK, and every member of `*pParameters` the
*      caller intends to be used has been set - the structure is not partially optional in
*      any way this interface describes.
* @post On success provisioning is under way. This interface declares no call to stop or
*       reverse it, and does not state what a second call while the first is still in
*       progress does, so a caller should not issue one speculatively.
*
* @note Blocking: synchronous. Provisioning itself is not: the call returns once the process
*       has been started, and it may block while the MTA hardware is not ready.
* @warning Not thread safe. The caller serialises this call against every other MTA HAL
*          call, including calls made from another process.
* @see mta_hal_InitDB, mta_hal_getMtaProvisioningStatus, mta_hal_getDhcpStatus
*/
INT mta_hal_start_provisioning(PMTAMGMT_MTA_PROVISIONING_PARAMS pParameters);

/**
 * @}
 */

/**
 * @addtogroup MTA_HAL_TYPES
 * @{
 */

/**
* @brief Type of the caller-supplied function the implementation invokes when line
*        registration status changes.
*
* This is the signature a caller implements and hands to
* mta_hal_LineRegisterStatus_callback_register(). The implementation calls it to deliver the
* current registration status of every line, which is the push counterpart to polling
* mta_hal_getLineRegisterStatus(). A caller writes this function; it never calls it.
*
* @param[in] output_status_array - array of per-line register status values from the MTAMGMT_MTA_STATUS enumeration, supplied by the implementation.
*                                  \n The array and its contents are supplied by the caller
*                                  of this function - that is, by the HAL implementation -
*                                  and are valid for `array_size` elements. This interface
*                                  does not state how long the array remains valid after the
*                                  function returns, nor whether the implementation frees
*                                  it, so an implementation of this callback must copy any
*                                  value it needs to keep and must not release the array.
*                                  It is marked as an input because it is data flowing INTO
*                                  the callback.
* @param[in] array_size - an int value, passed by value, giving the number of elements in output_status_array, described by this interface as the total line number.
*                         \n An implementation of this callback must not read beyond
*                         `array_size` elements, and must not assume the value equals
*                         `MTA_LINENUMBER` (8) even though that is the line count this
*                         interface publishes.
*
* @returns The status of the operation, as reported by the callback back to the implementation.
* @retval RETURN_OK  - The callback accepted and handled the delivered status.
* @retval RETURN_ERR - The callback did not handle it. This interface does not state what the
*                      implementation does in response - whether it retries, stops
*                      delivering, or ignores the result - so a callback should not rely on
*                      returning `RETURN_ERR` to trigger a redelivery.
*
* @pre The callback has been installed by mta_hal_LineRegisterStatus_callback_register().
* @post Whatever the callback body does. It must return promptly, because this interface does
*       not state whether the implementation is holding a lock or blocking a thread of its
*       own while the callback runs.
*
* @note Blocking: an implementation of this callback must be non-blocking and must not suspend
*       the context that invoked it. Nothing here states which context or thread that is, or
*       whether the implementation serialises invocations, so a callback that waits risks
*       stalling the implementation that called it.
* @warning Thread safety: this interface does not specify the thread or context on which the
*          callback is invoked, so an implementation of it must assume the worst - that it may
*          run concurrently with the caller's own code - and must protect any state it
*          touches. Note also that the MTA HAL is not thread safe, so a callback must NOT
*          invoke MTA HAL entry points; doing so would issue an unserialised call from an
*          unspecified context.
* @see mta_hal_LineRegisterStatus_callback_register, mta_hal_getLineRegisterStatus
*/
typedef INT ( * mta_hal_getLineRegisterStatus_callback)(MTAMGMT_MTA_STATUS *output_status_array, int array_size);

/**
 * @}
 */

/**
 * @addtogroup MTA_HAL_APIS
 * @{
 */

/**
* @brief Installs the caller's callback for line register status updates.
*
* Hands the implementation a function to invoke whenever line registration status changes,
* so a caller is notified rather than having to poll mta_hal_getLineRegisterStatus(). This is
* the only asynchronous notification this interface offers.
*
* @execution callback
*
* @param[in] callback_proc - The function to install, of type mta_hal_getLineRegisterStatus_callback.
*                            \n The caller owns the function. What is established about the
*                            registration is only that this interface declares no way to
*                            remove or replace one and no deinitialisation call, so a caller
*                            has nothing with which to withdraw it; how long a registration
*                            remains active, and for how long the implementation may go on
*                            invoking the function, are not stated by this interface. A
*                            caller therefore cannot rely on a registration lapsing, and
*                            keeps the function - and every piece of state it touches -
*                            callable for as long as it cannot rule out an invocation, which
*                            this interface gives it no means of doing. This interface also
*                            does not state what passing NULL does, so a caller should not
*                            pass NULL in an attempt to unregister, and does not state
*                            whether a second call replaces the first callback or adds to it,
*                            so a caller should register exactly once.
*
* @pre mta_hal_InitDB() has returned RETURN_OK. Register before the events of interest can
*      occur, since this interface does not deliver status changes that happened earlier.
* @post Nothing further is established by this call: it returns nothing, so it reports no
*       outcome, and this interface does not state when the first invocation of
*       `callback_proc` may occur - whether once this function has returned, or while it is
*       still running. A caller must therefore treat an invocation as possible from the
*       moment it registers, have everything the callback touches ready beforehand, and not
*       use the return of this function as a barrier before which no invocation can happen.
*
* @note This function returns nothing, so a caller cannot tell from it whether registration
*       succeeded; the first invocation of the callback is the only confirmation available.
* @note Blocking: it may block while the MTA hardware is not ready, in common with the rest
*       of this interface. The callback itself is invoked asynchronously and must not block -
*       see mta_hal_getLineRegisterStatus_callback.
* @warning Not thread safe. Register from one thread only, with no other MTA HAL call in
*          flight, and note that the callback may then be invoked on a context this interface
*          does not specify.
* @see mta_hal_getLineRegisterStatus_callback, mta_hal_getLineRegisterStatus
*/

void mta_hal_LineRegisterStatus_callback_register(mta_hal_getLineRegisterStatus_callback callback_proc); //Callback registration function.

#endif /* __MTA_HAL_H__ */
/**
 * @}
 */
