��+ :  E   Main� VB Mapi Demo Program�
  n
    �   B #�             �     (       @         �                        �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���  wwwwwwwwwwwww                 ������������  ������������  ���     ���  ������������  ������������  ���    ����  ������������  ������������  ���      ���  ������������  ���������UU�  ���������UU�  ���������UU�  ���������UU�  ���������UU�  ������������                                                     �   ���           �  �          �  �       � �  �  �       � �  �  �          ���           �  �      �   � �  �      �   � �  �          ���                     �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  ����������������������������������������������������$Form1%�*   $Compose_BTN� &Compose���w �(   #
Delete_BTN� &Delete���w �$   !Read_BTN� &Read��Gw �"    New_BTN� &Newx ��w �    SubjectList�x �B	 �   IdList�U< g� 	   �A   Heading_LBL� Messages:                      x x ��   �    6Options_MENU�Options��   7
Logon_MENU�Logon�   8Logoff_MENU�Logoff�#   9Status_MENU�Status Window�'   :PopupErrors_MENU�Popup Errors�.%  �  PP	 
�Q�Q�
   ��w ��� lZ � $� F�J��l � v$`� ��� 9"3                  �
   Command1_Clickk retcode�  	MAPILogon�  Form1� hWnd�  MAPISessoin�MapiSession�  Logon_Clickx Text] 
SessionWin Logoff_Click� 
MAPILogoff� SessionWin_Change�rc� 
Send_Click5�ML MAPIMessage� Mrm 	MapiRecip� MfX MapiFile� MAPISendMessage� Subject@ NoteText� 
RecipCount 	FileCount; MAPISendMail� Nameg 	RecipType/ MAPI_TOK 
RecipClass$ MAPI_CC FileName| config' sys� FileType� PathName� Position� 
Save_Click� ID� MAPISaveMail� FindNext_Click� MAPIFindNext� ReadMail_Click��Mo� MAPIReadMail� Body
�MsgId� 
Form_Click� Recips Files� SessionX UIParam� 	MessageID� Flags� Reservedq Message   Orig? Info� nFiles� nRecipsS BMAPIReadMail�SUCCESS_SUCCESS� BMAPIGetReadMail��R[�F� elseEnd� IfEnd� MAPIAddressX
 Caption� Label� nRecipients� 
Recipients� 
nNewRecips� 	NewRecips NewRecipients RecipientCount, BMAPIAddress5 nEditFields@ Labels BMAPIGetAddressN Body_Change� 	ID_Changek Label1_Click{ Label2_Click   Label3_Click� DateReceived� ConversationIDk MessageType� MAPI_NO_MESSAGES. begin Retcode_Change)
 im� msgid1$ ErrorMsg� errtext� errtext_ChangeH
 ErrMsg� tmpA MailList� Fomr1� 
List_Click� List. ListHdr�@	Form_Load� From1� Logoff� Enable� FALSE� Logon|
 Enabledt TRUE� Delete_ClickU 
MAPIDelete� MAPIDeleteMail   	ListIndex/ Address_Click	
 reciph asLong@ Class   Address� ListHdr_Click7	 Resolve_BTN_Click� MapiResolveName� hSession� 	Recipient Rcpnt MapiRecipient Details_BTN_ClickO MAPIDetails� RR� FF;
 	ListCount= i% Logon_BTN_Click 	Logon_BTN   SUCCESS* count��flag��MAPI_LOGON_UI� MAPI_USER_ABORTk MAPI_E_FAILURE� MAPI_E_TOO_MANY_SESSIONSY MAPI_E_LOGON_FAILURE� Form2q 
Logoff_BTN[	 MAPI_E_USER_ABORT�	�Status� ErrCode_EDIT   ErrString_EDIT� ErrCode� Op Operation_EDITQ Session_EDIT  �ignore	 	ToolForm1e	�Tools   LoggedInAs_TXT�	 DisplayStatus�	�Main�	 MAPI_E_LOGIN_FAILUREm	@Form_Unload��Cancel� Msg� List_DblClick&	 Mail	 selected   StoreInList  �li   index   done   EndLoopI	 IdList   MapiRecipReDimm
 SubjectList�
@Delete_BTN_Click  @New_BTN_Click  @Read_BTN_Click   Save_BTN_Click   xxxxxx�	�View�	@SubjectList_Click  @SubjectList_DblClick   Unhide  �Compose4
 MAPI_E_TOO_MANY_SESSION
 Options_MENU_Click�	@Logoff_MENU_Click  @Logon_MENU_Click  @Compose_BTN_Click�
 display   results   set  �	OldCursor   MousePointer�
@Status_MENU_Click  �Status_MENU   Checked�
 
MsgId_EDIT�
 PopupError_MENU  �PopupErrors  �PopupErrors_MENU  @PopupErrors_MENU_Click
      	  ��������    Compose_BTN_Click0�      X  	
      v   &*  v &   Load and display the compose window v &   when the user clicks on the compose v 	   buttonu v   &*     �	�   �	e    x     9 	  ��������     Delete_BTN_Click0�     X  	      v   "*  v "   If there's no mail listed, then v    just exit this subroutine v   "*     Q 	 �� � � I �   = 6 8    v   %*  v %   Get the list index of the selectede v "   item, then retrive it's id from v    an associated msgid list.   v   %*     Q 	 � �  � Q �  X�   v   ,*  v ,   Call MAPIDeleteMail with the msgid of the v    mail to be deleted. v   ,*    � �     ��     �       � �    Delete Mail �   @    v   -*  v -   if the call was successful, then we deletee v -   the msgid from IdList and the subject from  v    the SubjectList.L v   -*     @� � I 2   Q 	e  �  x    Q �e  �  x  8     9 	  ��������(    	 Form_Load0@     X  k      � "  $
   � 2  9   � D  9    g � V  L    l   � �  9   g  � �  L   l    v   %*  v %   Display the hourglass cursor until* v    the ReadMail call completes v   %*     Q H
 ;
 �   Q H
   v   *  v    Log the user on.r v   *        Q � �  b    �  r    �     �   w    v   +*  v +   Display information in the status windowi v   +*    � 
 � Logon �       @   v !  Tools.Logoff_BTN.Enabled = TRUE* v !  Tools.Logon_BTN.Enabled = FALSE*   v   **  v *   Find all the user's mail and display it v )   in the SubjectList.  The correspondingn v )   message id's are stored in the IdList.L v   **    � �     �  <    �  J    �     �     �  ]  @� � 1    @� � I �   0v   (*  0v (   Read the mail and display the subject 0v    in the SubjectListe 0v   (*    0� �     ��     �          g   l 	 � 0 @� � I � @ Q 	e   k  x  @ Q �e �  x  08     8   � �     �  �    ��     �     �  ] J x   v   '*  v '   Highlight the first item in the list  v   '*     Q 	 �� � � I �  �  Q 	 � 8    v   '*  v &   Set status display in status window v   '*    � 
 � Find  �   @    v   '*  v '   Display the tools and status windowsn v   '*    v   Tools.Show  �e    x  , m
 |
 , �
 |
 , �
   v   "*  v !   Restore original mouse pointeri v   "*    ;
 Q H
    9 	  ��������[     Form_Unload0�      X  q  � �      v   %*  v %   Mapi Demo is shutting down, unloadn v #   any displayed windows and Logoffo v   %*     �   � � � �   �     9 	  ��������     Logoff_MENU_Click0�      X  �	      v   &*  v #   Call MAPILogoff, display resultss v &   and set the MapiSession global var. v   &*    � � � �   �  �  �  Logoff  �   @  �     �     9 	  ��������     Logon_MENU_Click0�     X  �	      � "  $
   � 2  9   � D  9    g � V  L    l   � �  9   g  � �  L   l    v   *  v    Call MAPILogonL v   *     Q H
 ;
 �   Q H
    Q � �  �     �  �     �    �     �   w  � �  � � �  �  N
 Logging On  �   @    v   *  v    Clear out the msg id listbox* v   *     Q 	 �� � � 1 $   Q 	e �   x    Q �e �   x  J � �  ,    �  v   /*  v *   Retrive the user's mail and display the v /   subject line from each message.  The messages v *   mail id is store in the IdList, and the v '   Subject is stored in the SubjectListt v   /*    � �     �  (    �  6    �     �     �  ]  @� � 1 �   @� � I J   0� �     ��     �          g   l 	 � 0 @� � I D @ Q 	e   k  x  @ Q �e �  x  08     8   � �     �  ^    ��     �     �  ] J d   v   /*  v /   Highlight the first item in the subject list  v   /*     Q 	 �� � � I   �  Q 	 � 8    � 
 " Find  �   @     ;
 Q H
    9 	  ��������A     New_BTN_Click0�     X  &	      � "  $
   � 2  9   � D  9    g � V  L    l   � �  9   g  � �  L   l    v   %*  v %   Display the hourglass cursor until* v    the ReadMail call completes v   %*     Q H
 ;
 �   Q H
   � 
  Find  �    � �   @    v   *  v    Clear out the msg id listbox* v   *    Q 	 �� � � 1 �   Q 	e �   x    Q �e �   x  J � �  �    �   v   **  v *   Retrieve messages until we get an error v    or no more message to get v   **   � �     �  �    �  �    �     �     �  ]  @� � 1 d   @� � I    0v   )*  0v )   Display the subject in the SubjectListe 0v '   and add the message id to the IDListt 0v   )*    0� �     ��     �          g   l 	 � 0 @� � I  @ Q 	e   k  x  @ Q �e �  x  08     8   � �     �  .    ��     �     �  ] J �     �  r Find new mail �   @    v   /*  v /   Highlight the first item in the subject list  v   /*     Q 	 �� � � I "  �  Q 	 � 8     ;
 Q H
    9 	  ��������D     PopupErrors_MENU_Click0^      X  �
       �
,� � I >   + �
  + �
 |
 2 X   , �
  , �
 |
 8     9 	  ��������     Read_BTN_Click0     X  7	      v   %*  v %   Ignore the read request if there'se v    no mail available to read v   %*     Q 	 �� � � I �   = * 8    v   #*  v "   Load the View window to display v    the mail messages v   #*     e	�   e	e    x     9 	  ��������     Status_MENU_Click0l      X  X
       m
 |
I R    �e    x   + m
 |
 2 �    �e    x   , m
 |
 8     9 	  ��������     SubjectList_Click0�      X  m	      v   *  v    Set the MsgId Global Var* v   *     Q 	 �� � � I �    Q 	 � Q �  X�  � � �
 �  8     9 	  ��������     SubjectList_DblClick0     X  �	      v   +*  v (   Set the MsgId Global var and load the v *   View window to display the mail message v   +*     Q 	 �� � � I �    Q 	 � Q �  X�  � � �
 �  2 �   =  8     e	�   e	e    x     9 	  ��������   �