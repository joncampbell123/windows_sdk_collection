		SimTest Datafile Information

                Date: 1-August-1988


	To create a datafile for display device:
		a. Select Datafile-New to give datafile name
		b. Create test cases
		c. Select Datafile-Save to save datafile away

	To create a datafile for printing device:
		a. Select Datafile-New to give datafile name
		b. Select Printer-CreateDC
		c. Select Destination-Printer
		d. Create test cases
		e. Select Printer-PrintDC
		f. Select Printer-DeleteDC
		g. Select Datafile-Save to save datafile away

	To run a datafile to display or printing device:
		1. In SimTest:
		  a. Select Datafile-Run to give datafile name
		2. At DOS prompt:
		  a. win SimTest DataFile1 DataFile2 ...
		3. Pause after each Datafile run (Wait for user to hit <SPACE>)
		  a. win SimTest -w DataFile1 DataFile2 ...

	To run a datafile, which was for display, to printing device:
		1. In SimTest:
		  a. Select Printer-CreateDC
		  b. Select Destination-Printer
		  c. Select Datafile-Run to give datafile name
		  d. Select Printer-PrintDC
		  e. Select Printer-DeleteDC
		2. At DOS prompt:
		  a. win SimTest -p DataFile1 DataFile2 ...

	To run a datafile in command line:
		a. Run "Simtest [-p] [-w] DataFile1 DataFile2 ..."

	To print DeviceCaps of display:
		a. Select Printer-CreateDC
		b. Select Printer-Print DeviceCaps
		c. Select Printer-PrintDC
		d. Select Printer-DeleteDC

	To print DeviceCaps of printing device:
		a. Select Printer-CreateDC
		b. Select Destination-Printer
		c. Select Printer-Print DeviceCaps
		d. Select Printer-PrintDC
		e. Select Printer-DeleteDC

	To print some output to printing device:
		a. Select Printer-CreateDC
		b. Select Destination-Printer
		c. Do SimTest operations
		d. Select Printer-PrintDC
		e. Select Printer-DeleteDC

        To edit a previously created-datafile:
                a. Run SIMDEC.EXE [.SIM file name]
                   (This will create a text file with .DEC extension)
                b. Use editor to modify .DEC file
                c. Run DECSIM.EXE [.DEC file name]
                   (This will create a .SIM binary file from the .DEC file)

	New features in SimTest:

  		Take multiple files as command arguments
		   e.g. win simtest test1.sim test2.sim ...

  		-p, -P flag for sending jobs to both display and printer
		   e.g. win simtest -p test1.sim test2.sim ...

  		-w, -W flag for waiting key response between jobs
		   (Hitting SP to continue)
		   e.g. win simtest -w test1.sim test2.sim ...

  		Menu is alphabetized.

  		New menu item - 'Trace'
		Default is on, which will update the current cursor position in
		caption bar. While 'Trace' is set as off, then the position will
		be updated only by moving cursor with keys, not with mouse.

  		New menu item - 'Brush-Color'
		This will bring up a dialog box for brush color selection.

  		Output will be send to both display and printer, if dest is
		printer.  This will shown the current printing status.


	SimTest Command keywords:

			 Keywords		Token
			----------		-----
			bckgrndblk		300
			bckgrndblu		304
			bckgrndcyn		306
			bckgrndgrn		302
			bckgrndmgt		305
			bckgrndopaque		309
			bckgrndred		301
			bckgrndtransparent	308
			bckgrndwht		307
			bckgrndylw		303
			bitblt			600
			bltsizing		4000
			brshbackward		207
			brshblu00		230
			brshblu40		231
			brshblu80		232
			brshbluc0		233
			brshbluff		234
			brshcolorptrn		203
			brshcross		208
			brshdiagcross		209
			brshforward		206
			brshgrn00		220
			brshgrn40		221
			brshgrn80		222
			brshgrnc0		223
			brshgrnff		224
			brshhollow		201
			brshhrzt		204
			brshmonoptrn		202
			brshred00		210
			brshred40		211
			brshred80		212
			brshredc0		213
			brshredff		214
			brshsetcolorptrn	2101
			brshsetmonoptrn		2100
			brshsolid		200
			brshvert		205
			clicksmark		6200
			clrblk			1050
			clrblu			1054
			clrcyn			1056
			clrdsp			1001
			clrgrn			1052
			clrhugcmpt		1004
			clrhugmono		1002
			clrmgt			1055
			clrpointscount		1007
			clrprt			1006
			clrred			1051
			clrsmlcmpt		1005
			clrsmlmono		1003
			clrwht			1057
			clrylw			1053
			copyhugcmpttodsp	6004
			copyhugmonotodsp	6002
			copysmlcmpttodsp	6005
			copysmlmonotodsp	6003
			datafilenew		9000
			datafilerun		9020
			datafilesave		9010
			dstdsp			801
			dsthugcmpt		804
			dsthugmono		802
			dstprt			806
			dstsmlcmpt		805
			dstsmlmono		803
			fldfilbordblk		1450
			fldfilbordblu		1454
			fldfilbordcyn		1456
			fldfilbordgrn		1452
			fldfilbordmgt		1455
			fldfilbordred		1451
			fldfilbordwht		1457
			fldfilbordylw		1453
			fldfildofldfil		11
			fldfilseedblk		1400
			fldfilseedblu		1404
			fldfilseedcyn		1406
			fldfilseedgrn		1402
			fldfilseedmgt		1405
			fldfilseedred		1401
			fldfilseedwht		1407
			fldfilseedylw		1403
			forgrndblk		400
			forgrndblu		404
			forgrndcyn		406
			forgrndgrn		402
			forgrndmgt		405
			forgrndred		401
			forgrndwht		407
			forgrndylw		403
			mrk64kbasondst		6800
			mrk64kbasonsrc		6700
			offsetyby3		3003
			offsetyby1		3001
			offsetyby2		3002
			org			8000
			penblk			100
			penblu			104
			pencyn			106
			pendash			122
			pendashdot		123
			pendashdotdot		124
			pendot			121
			pengrn			102
			penmgt			105
			pennull			125
			penred			101
			pensolid		120
			penwdth0		110
			penwdth1		111
			penwdth10		114
			penwdth2		112
			penwdth5		113
			penwht			107
			penylw			103
			pixelblk		1550
			pixelblu		1554
			pixelcyn		1556
			pixeldoaltpixelline	3004
			pixeldopixelline	1500
			pixelgrn		1552
			pixelmgt		1555
			pixelred		1551
			pixelwht		1557
			pixelylw		1553
			prtcreatedc		7700
			prtdeldc		7900
			prtprndc		7800
			prtprndevcaps		7950
			rop2blk			501
			rop2copypen		513
			rop2masknotpen		503
			rop2maskpen		509
			rop2maskpennot		505
			rop2mergenotpen		512
			rop2mergepen		515
			rop2mergepennot		514
			rop2nop			511
			rop2not			506
			rop2notcopypen		504
			rop2notmaskpen		508
			rop2notmergepen		502
			rop2notxorpen		510
			rop2wht			516
			rop2xorpen		507
			rop3blk			900
			rop3dpx			903
			rop3dsa			905
			rop3dsno		906
			rop3dso			909
			rop3dson		901
			rop3dsx			904
			rop3p			910
			rop3pn			911
			rop3psa			907
			rop3s			908
			rop3sdna		902
			rop3sn			913
			rop3wht			912
			shapealterpolygon	1102
			shapearc		1112
			shapecoordinate		1114
			shapeellipse		1101
			shapefinegrid		1109
			shapegrid		1110
			shapeline		1111
			shapepie		1113
			shapepolyline		1104
			shaperectangle		1100
			shaperoundrect		1105
			shaperuler		1108
			shapesampletxt		1106
			shapetxt		1107
			shapetxtlen=1		2001
			shapetxtlen=16		2016
			shapetxtlen=2		2002
			shapetxtlen=3		2003
			shapewindingpolygon	1103
			srcdsp			701
			srchugcmpt		704
			srchugmono		702
			srcsmlcmpt		705
			srcsmlmono		703
			stretchblt		20
