; /*
; Microsoft Developer Support
; Copyright (C) 1992 - 1996 Microsoft Corporation
;
; This file contains the message definitions for the Win32
; cliptext.exe sample program.

;-------------------------------------------------------------------------
; HEADER SECTION
;
; The header section defines names and language identifiers for use
; by the message definitions later in this file. The MessageIdTypedef,
; SeverityNames, FacilityNames, and LanguageNames keywords are
; optional and not required.
;
;
MessageIdTypedef=DWORD

;
; The LanguageNames keyword defines the set of names that are allowed
; as the value of the Language keyword in the message definition. The
; set is delimited by left and right parentheses. Associated with each
; language name is a number and a file name that are used to name the
; generated resource file that contains the messages for that
; language. The number corresponds to the language identifier to use
; in the resource table. The number is separated from the file name
; with a colon. The initial value of LanguageNames is:
;
;LanguageNames=(English=1:MSG00001)

LanguageNames=(Japanese=0x411:MSG00002)

;
; Any new names in the source file which don't override the built-in
; names are added to the list of valid languages. This allows an
; application to support private languages with descriptive names.
;
;
;-------------------------------------------------------------------------
; MESSAGE DEFINITION SECTION
;
; Following the header section is the body of the Message Compiler
; source file. The body consists of zero or more message definitions.
; Each message definition begins with one or more of the following
; statements:
;
; MessageId = [number|+number]
; Severity = severity_name
; Facility = facility_name
; SymbolicName = name
;
; The MessageId statement marks the beginning of the message
; definition. A MessageID statement is required for each message,
; although the value is optional. If no value is specified, the value
; used is the previous value for the facility plus one. If the value
; is specified as +number then the value used is the previous value
; for the facility, plus the number after the plus sign. Otherwise, if
; a numeric value is given, that value is used. Any MessageId value
; that does not fit in 16 bits is an error.
;
; The Severity and Facility statements are optional. These statements
; specify additional bits to OR into the final 32-bit message code. If
; not specified they default to the value last specified for a message
; definition. The initial values prior to processing the first message
; definition are:
;
; Severity=Success
; Facility=Application
;
; The value associated with Severity and Facility must match one of
; the names given in the FacilityNames and SeverityNames statements in
; the header section. The SymbolicName statement allows you to
; associate a C/C++ symbolic constant with the final 32-bit message
; code.
; */

MessageId=0x1
SymbolicName=MSG_CLIENTAREATEXT
Language=English
This program demonstrates the use of the Edit menu to copy and paste text to and from the clipboard.  Try using the Copy command to move this text to the clipboard, and the Paste command to replace this text with data from another application.



You might want to try running Notepad and Clipbrd alongside this application so that you can watch the data exchanges take place.
.
Language=Japanese
このプログラムはクリップボードへのテキストをコピー、およびクリップボードからテキストの貼り付けの編集メニューのサンプルです。コピーコマンドを実行するとテキストをクリップボードにコピーします。貼り付けコマンドを実行するとこのテキストを他のアプリケーションからクリップボードにコピーされたデータと置き換えます。



メモ帳 と クリップボード をこのアプリケーションと同時に実行すれば、データがどのように各アプリケーション間で交換されるかを見ることができます。
.
