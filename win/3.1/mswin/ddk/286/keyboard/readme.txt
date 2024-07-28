Notes on 3.00 keyboard drivers
6 Sept 1988
Peter Belew


For 3.00 Windows, the keyboard drivers have been split up into two
modules.  The basic driver is somewhat hardware-specific, and contains
translation tables only for the enhanced USA keyboard.  Tables for
other keyboards (other countries and other physical keyboards)
are contained in an optional dynamically-linked library (DLL), which
is copied by Setup or another application, such as Control Panel,
to the Windows directory.

Four drivers are produced from these sources:

	KBD.DRV			For IBM-compatible keyboards
				Almost everyone will use this
				keyboard driver.
	KEYBDO.DRV		For Olivetti M24 and M240 systems,
				and AT&T 6300 and 6300 Plus systems.

	KBDNOKIA.DRV		For Nokia (Ericsson) systems.

	KBDHP.DRV		For Hewlett-Packard Vectra systems.

The selected driver is normally renamed or copied to KEYBOARD.DRV.

Each one of these drivers contains the keyboard translation table for the
USA 'type 4' (enhanced) keyboard, in a loadoncall segment.

If the DLL "KBDDLL.MOD" exists in the Windows directory, the function
NewTable() in the driver calls the function GetKbdTable() in the DLL
to install the DLL's tables for the type of keyboard which is installed.
If NewTable() fails to load the DLL properly, it restores the USA 4
table, so the driver will keep on using the table in the loadoncall segment.
Keyboard.NewTable() may be called from an application (control panel)
after copying a new DLL onto KBDDLL.MOD.

Identification of keyboard type is done in these drivers by a combination
of reading WIN.INI settings and checking the ROM BIOS.  This check is
done when the keyboard enable() function is called.

In the case of KBD.DRV, or KBDHP.DRV, the driver first determines a default
keyboard type on the basis of (a) is the system an AT-compatible system or
not, and (b) if it is an AT-compatible, does it have an enhanced keyboard.
(This is the same algorithm used for the old USA.DRV keyboard driver).

Then, it reads the Type entry from WIN.INI -- if an entry is found, it
overrides the default:

    [keyboard]
	Type=4

Keyboard types are

	1:	PC/XT 83-key
	2:	Olivetti 'ICO' 102-key
	3:	AT-type 84 or 86-key
	4:	Enhanced 101 (USA) or 102-key (foreign)
	5:	Nokia 1050 or 1051
	6:	Nokia 9140
        7:      Japanese style keyboards

For Olivetti and AT&T systems, there is an additional parameter in WIN.INI,
which is the hardware keyboard ID byte.  This is used in some cases to
distingush between different keyboards with the same layout, for the purpose
of determining how many LED's it has, etc.  It is particularly important
to make the settings in WIN.INI for Olivetti systems, since the BIOS
may not give a proper indication of the keyboard type.

    [keyboard]
	Type=1			; PC/XT layout
	OliType=4		; AT&T 302 keyboard for 6300 Plus

    [keyboard]
	Type=1			; PC/XT layout
	OliType=2		; AT&T 301 keyboard or Olivetti 83-key

    [keyboard]
	Type=1			; PC/XT layout
	OliType=66		; 42H: XT-type keyboard on M28

    [keyboard]
	Type=2			; ICO layout
	OliType=1		; Olivetti 102-key

    [keyboard]
	Type=3			; AT layout
	OliType=16		; 10H: 86-key

    [keyboard]
	Type=4			; Enhanced layout
	OliType=64		; 40H: 101/102-key

When KEYBDO.DRV is installed, default settings for various Olivetti
or AT&T systems are

	Type = 1, OliType = 2 for M24's or AT&T 6300's (8086 systems).
	Type = 4, OliType = 64 for Olivetti M240's (8086 system).
	Type = 1, OliType = 4 for AT&T 6300 Plus sytems (Type 302
				keyboard).

Other AT&T or Olivetti systems (AT-compatible '286 or '386 systems)
should use the KBD.DRV driver.

Ericsson/Nokia systems should used the KBDNOKIA.DRV driver; the default
setting is Type = 5.

==========================================================================
A sample program for changing the keyboard translation table DLL:
SETKBD.EXE.

The sample application SETKBD.EXE demonstrates how an application can
override the keyboard DLL file with another one, and make a call to the
keyboard driver to install the new DLL.

It also provides a dialog for changing the settings of physical keyboard
types in WIN.INI (the Type and OliType settings described above).  An
assembly-code function is provided which returns the keyboard type,
the hardware 'olitype' byte, and the primary and secondary system type
bytes.  It is expected that the system should be rebooted when the keyboard
type is changed; on some systems (AT&T), a newly changed keyboard will not work
at all after it is connnected, unless the system is power-cycled; on 
other systems (most IBM-compatibles), the BIOS will not properly recognize
the changed keyboard until the system is rebooted.

It is expected that this code will be integrated (with appropriate
modifications) into SETUP and CONTROL.

Ordinarily, Setup should do the hardware keyboard type selection, using
the code provided for this in the KEYSET program.  This should be followed
by a menu which allows the user to override the keyboard physical type.
However, the routine provided is pretty reliable.  The menu should highlight
the default selection.

Setup should determine which of the four drivers is appropriate for the
particular system type.  For example,

	Type	OliType	Driver		Description

	1	--	KBD.DRV		XT 83-key keyboard
	1	(66)	KBD.DRV		Olivetti XT 83-key keyboard on M28
    	1	2	KEYBDO.DRV	Olivetti M24 or AT&T PC 6300 301 83-key
    	1	4	KEYBDO.DRV	AT&T 302 keyboard
    	2	1	KEYBDO.DRV	Olivetti ICO M24 102-key keyboard
    	3	--	KBD.DRV		AT 84-key keyboard
    	3	(16)	KBD.DRV		Olivetti 86-key keyboard
	3	--	KBDHP.DRV	Hewlett-Packard Vectra, Envoy kbd.
	4	--	KBDHP.DRV	Hewlett-Packard Vectra, Enhanced kbd.
    	4	(64)	KBD.DRV		Enhanced 101 or 102-key keyboard
    	5	--	KBDNOKIA.DRV	Nokia/EIS 1050 or 1051 keyboard
    	6	--	KBDNOKIA.DRV	Nokia/EIS 9140 keyboard

The above settings should be made in WIN.INI, and the appropriate driver
copied as 'keyboard.drv'.  The OliType setting need only be made when the
KEYBDO.DRV driver is used.

For selection of a national keyboard in Setup, the keyboard selection may
follow the country selection menu.  It may be possible to make a default
selection of keyboard on the basis of the country selection.  However, it
is probably better to just place the most likely selections at the top
of the menu in each localized version.  There is by no means a one-to-one
correspondence between the selection of a country (which mainly selects
punctuation and currency symbols) and the selection of a keyboard!

Care should be taken that the country and keyboard selections in Setup
and Control Panel are identical.

==========================================================================
SOURCE FILES:

Make file:

		makeit.bat		May be used to make 'kb'
		kb			Makes the 4 drivers above

Module definition files:

		kbd.def			for kbd.drv
		keybdo.def		for keybdo.drv
		kbdnokia.def		for kbdnokia.drv
		kbdhp.def		for kbdhp.drv

LINK4 script files:

		keyboard.lnk		for kbd.drv
		keybdo.lnk		for keybdo.drv
		kbdnokia.lnk		for kbdnokia.drv
		kbdhp.lnk		for kbdhp.drv


MASM source files in KEYBOARD:

		trap.asm		Hardware interrupt
		init.asm		Code run at load time
		setspeed.asm		Speed setting for AT keyboards
		toascii.asm		VK to Ascii translation
		xlat.asm		Oem/Ansi translation
		enable.asm		Enable(), Disable()
		oemscan.asm		OemKeyScan() for Win386
		tabs.asm		Tables and tables initialization

Include files in KEYBOARD: 

		keyboard.inc		Most definitions
		equate.inc		Hewlett-Packard definitions
		vkwin.inc		Define VK codes 0..7FH
		vkoem.inc		Define VK codes 80h..0FFH
		xlatus.inc		Oem/Ansi conversion tables, most places
		xlatca.inc		Oem/Ansi conv. tables, French Canada
		xlatno.inc		Oem/Ansi conv. tables, Norway/Denmark
		xlatpo.inc		Oem/Ansi conv. tables, Portugal
		xlates.inc		Oem/Ansi conv. tables, Olivetti Spain II
		date.inc		Current date string
		tab4.inc		USA enhanced keyboard tables

Files in KEYBOARD\TABLES:

		(This directory mainly contains sources for keyboard DLL's)

		makeit.bat		May be used to make DLL's
		tables			Makefile for DLL's: makes KBD??.MOD

		trans.inc		Macro definitions for tables

		xlatus.asm		Includes xlatus.inc
		xlatpo.asm		.. etc ...
		xlatno.asm
		xlates.asm
		xlatca.asm

		getkbd.asm		Code source for DLL. Contains
					GetKbdTable() function.

					Tables for ..
		kbdus.asm		USA
		kbduk.asm		United Kingdom
		kbdbe.asm		Belgium
		kbdca.asm		Canada (French)
		kbdda.asm		Denmark
		kbdes.asm		Olivetti "Spain II"
		kbdfr.asm		France
		kbdfs.asm		Finland/Sweden
		kbdgr.asm		Germany
		kbdit.asm		Italy
		kbdla.asm		Latin America
		kbdne.asm		Netherlands
		kbdno.asm		Norway
		kbdpo.asm		Portugal
		kbdsf.asm		Swiss-French
		kbdsg.asm		Swiss-German
		kbdsp.asm		Spanish

==========================================================================
General notes on driver internals (directed to those familiar with
the 2.03 drivers):

The tables for translating VK codes to ASCII have been drastically
reorganized.  Macros have been used to organize the search values
(virtual keycodes or VK codes + a dead key) into separate byte or
word vectors, so that string instructions can be used to make a
rapid search.

The KeyTrTab[] scan code to VK code table is always kept in the driver
itself, but may be partially overlaid when a DLL is initialized.
The DLL contains a list of the entries which need to be updated
in KeyTrTab for each keyboard type for the country it handles.

If a DLL by the name KBDDLL.MOD is not found in the Windows
directory, the driver uses the VK-to-ANSI tables in the TABS
segment, which is loadoncall.  ToAscii() and other functions
accessing these tables call a function which loads this segment
(which is a CODE segment),  if the DLL handle is invalid.

A DLL contains a copy of the ANSI/OEM translation tables for the
country it handles. This is copied into the driver, and completely
overlays the OemToAnsi and AnsiToOem tables in the driver.  Note
that these two tables are in a code segment (as they were in the
previous Windows keyboard drivers), so it is necessary to
pass a data-segment 'alias' for this segment to the DLL, in order
for the driver and DLL to function properly in protected mode.

A DLL contains a 'header' table of offsets and sizes for the tables
which are used in the DLL -- this table is copied into the driver by
the DLL function GetKbdTable(), after being updated properly for
the appropriate keyboard table type.

All VK-to-Ascii tables remain in the DLL, and are accessed via
the header table.

TABS.ASM in the driver, and GETKBD.ASM in the DLL sources, contain
the code relevant to initialization of tables and loading and
initialization of the DLL's.

==========================================================================
Notes on HP and Nokia drivers:

The 'ENVOY' code has been preserved from the 2.10 driver. It has not been
tested as yet (6 Sept 88).

Ericsson/Nokia code has been preserved from the 2.03 driver.  The
organization of the tables has been changed quite a bit.  In KeyTrTab[]
(the scan code to VK code table), the table is initialized with the
1050 translations (for high-value scan codes), and overlaid with the
9140 translations when the NewTable() routine is called (from Enable()
or elsewhere).  The Nokia driver has not been tested (88-9-6).

==========================================================================
Generation of 3.00 keyboard tables:

A utility 'MT.C' was written for translating 2.03 keyboard tables to
3.00 tables.

This utility and its makefile will be archived under the

	KEYBOARD\TABLES\MAKETAB

subdirectory.

