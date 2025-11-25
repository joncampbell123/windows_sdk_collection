'$IFDEF  GERMAN
    '$INCLUDE 'GERMAN.INC'

'$ELSEIFDEF FRENCH
    '$INCLUDE 'FRENCH.INC'

'$ELSEIFDEF SPANISH
    '$INCLUDE 'SPANISH.INC'

'$ELSE                                          'ENGLISH
    '$INCLUDE 'ENGLISH.INC'

'$ENDIF

CONST   MENU_OPERATIONS         = 1
CONST   MENU_RESTORE            = 2
CONST   MENU_CATALOG            = 3
CONST   MENU_ERASE              = 4
CONST   MENU_EXIT               = 9
CONST   MENU_WINDOW             = 5
CONST   MENU_2                  = 7




CONST   MAX_RESTORE_WAIT        = 1400
CONST   MAX_ERASE_WAIT        = 1400
CONST   MAX_TAPE_READY_WAIT     = 1400
CONST   MIN_SLEEP_TIME      = 5
CONST   MAX_TAPE_FOREIGN_TAPE   = 20

