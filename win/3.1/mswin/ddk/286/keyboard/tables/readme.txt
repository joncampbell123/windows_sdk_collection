===== What all the tables in a DLL tables file mean ======

A tables file KBDxx.ASM contains data in two segments: the CODE
segment, which is loadoncall and disposable, and the DATA segment,
which is fixed.  Data from the CODE segment is used to overlay
the tables in the DATA segment, or is copied to the driver itself.

The initial values in the tables in the DATA segment are for
type 4 (Enhanced) keyboards.  For other keyboard types, the
tables in the DATA segment must be patched or overlaid.
For any keyboard type, the 'header' must be copied from the
DLL to the keyboard driver.

The copying or overlaying is done by the GetKbdTable() function,
which is called from the driver after the driver loads the library.
This is the only code in this library; once GetKbdTable() is called,
the driver accesses the DLL's fixed DATA segment directly.

Entries in the CODE segment are listed first in the following.


1) Conditional assembly flags for dead key table.

    The dead key translation table (see the label DeadKeyCode below)
    has several sections, which are assembled conditionally, depending
    on whether the flags DGRAVE, DACUTE, etc. are defined here.

2) Accent definitions for dead key table.

    The values of the various accent characters are defined here,
    for the dead key tables.  The acute accent and umlaut deadkeys
    use different ANSI characters in different countries.


3) Patches for KeyTrTab[].

    KeyTrTab[] (always located in the driver itself) is the table
    the interrupt routine (in TRAP.ASM) uses to translate scan codes
    to Windows virtual keycodes.  This group of tables contains
    patches for KeyTrTab[].
   
    The array PatchIndices[] lists the entries in KeyTrTab[] which must
    be overlaid.  The entries X1 and X2 are interchanged, if the keyboard
    is an AT-type keyboard (84 keys) on a non-Olivetti system.  The
    entry Patch102 is modified for a Nokia type 6 table.

    TransPatches contains a list of offsets to the patches for the various
    keyboard types.


4) The header.

    This is a structure containing the offsets and sizes of various
    tables in the DLL's DATA segment.  It is patched to reflect any
    changes in the sizes of certain tables in the DATA segment, and
    then copied to the driver.

    Note that the header is overlaid, even though it is in the CODE
    segment.  It is necessary to get an alias for the CODE segment
    to do this in protect mode.


5) Flags list

    The fFlags array contains values to be copied into the fKeyType
    entry in the header, depending on the keyboard type.


6) AsciiTran overlay list.

    The entries in AsciiTran[] (see below) for VK_0..VK_9,
    VK_OEM_*, and VK_DECIMAL change among various keyboard
    types.  This group of patch tables contains a complete
    overlay of this part of AsciiTran, for each keyboard
    type.   Note that the patch for type 3 keyboards is
    usually the same as for type 1, which explains the
    'ifdef ATSAME' directive.

    These patch lists are fixed in length, and only the translations
    are patched (not the VK codes).


7) Overlays for Control, Control-Alt, Shift-Control-Alt

    These overlays contain patches for both the virtual keycode
    values and their translations.

    They are variable in length, so the destination table in the
    DATA segment may have to have padding at the end, if the table
    for type 4 is shorter than the same table for some other keyboard
    type.


8) Overlays to the 'Morto' dead key table

    These tables contain patches for the search value (a combination
    of virtual keycode and shift state) and the translation (some
    dead key accent value).

    These are also variable in length, so that the table which is
    overlaid (type 4) may need to be padded at the end to allow
    for the overlay.


9) Overlays to the Capital table.

    Each overlay to the Capital table just contains a list of
    virtual keycodes for which Caps Lock (Shift Lock) takes effect,
    for the particular keyboard type.



Following tables are in the DATA segment.  They are initialized
for the type 4 keyboard.


10) Ascii translation table AscTran

    This is an associative translation table, matching a byte vector
    of virtual keycode values to a word vector, each entry of which
    contains the translations of the VK code for the unshifted and
    shifted states.

    Entries for VK codes VK_0 through VK_DECIMAL will be overlaid
    for keyboard types other than 4.


11) Dead key table.

    The DeadKeyCode/DeadChar table maps a combination of a dead
    key (accent) character and a letter or space into some
    accented character.  It is used when the next key is struck
    AFTER a dead key accent key -- ordinarily, the second key
    is some letter, or a space.

    It is conditionally assembled; if a character is typed with
    a dead key for some keyboard for this country, the appropriate
    section of this table is included.


12) 'Morto' table

    This table does the initial translation for dead keys, when
    the dead key is typed.  A combination of virtual key code
    and shift state is mapped into the ANSI code for some accent.

    Padding may be needed at the end of the table to allow for
    overlaying for a keyboard type different from 4.

13) Control, Control-Alt, Shift-Control-Alt tables

    These tables map VK codes into ANSI characters, for one of the
    3 specific shift states listed above.

    Padding may be needed at the end of the table to allow for
    overlaying for a keyboard type different from 4.

14) Capital Table.

    This table lists the virtual keycodes for which Shift Lock
    is valid (aside from VK_A..VK_Z).

    Padding may be needed at the end of the table to allow for
    overlaying for a keyboard type different from 4.

