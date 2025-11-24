/*++

Copyright (c) 1990-1995  Microsoft Corporation

Module Name:

    sonicdet.h

Abstract:

    This file has processor-specific definitions.

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

--*/


//
// Handy macros to read out of sonic ports.
//
// Because the use of PortShift makes the code more complicated,
// we make some assumptions about when it is needed. On x86, we
// only support the EISA card; on MIPS, we only support the
// internal version unless MIPS_EISA_SONIC is defined.
//
// We define two constants, SONIC_EISA and SONIC_INTERNAL, if
// that particular adapter type is supported by this driver.
// This is to prevent unneeded code from being compiled in.
//


//
// x86, only the EISA card is supported, the registers are always 16 bits.
//

#define SONIC_WRITE_PORT(_Adapter, _Port, _Value) \
    NdisWritePortUshort((_Adapter)->NdisAdapterHandle, (_Adapter)->SonicPortAddress + (_Port * 2), (USHORT)(_Value))

#define SONIC_READ_PORT(_Adapter, _Port, _Value) \
    NdisReadPortUshort((_Adapter)->NdisAdapterHandle, (_Adapter)->SonicPortAddress + (_Port * 2), (PUSHORT)(_Value))


#define SONIC_EISA
#undef SONIC_INTERNAL
#undef MIPS_EISA_SONIC  // just in case it is defined


//
// The default adapter type is EISA
//

#define SONIC_ADAPTER_TYPE_DEFAULT  SONIC_ADAPTER_TYPE_EISA
