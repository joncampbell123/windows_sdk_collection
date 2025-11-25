****************************************************************

			Multimedia Samples

****************************************************************

Sample		Description
---------       -------------------------------------------
ACMAPP          Displays wave file format, and plays, 
		records, and converts wave files.

AVIEDIT 	Simple AVI editing application using the
		editing APIs in AVIFile.

AVIVIEW         Simple AVI viewing application using the 
		read/write APIs in AVIFile.

CAPTEST         Sample capture application that uses the 
		AVICap capture window class.

DSEQFILE	AVIFile file handler for DIB sequences.
                Implemented in C++.  To install, execute
                dseqfile.reg and copy dll to system dir.
                Can be used with AVIEDIT or AVIVIEW.

ICMAPP		Shows how to call the ICM APIs.

ICWALK		Sample application that shows all the
                codecs installed on the system.  Uses the
		ICM APIs.

IDFEDIT         MIDI IDF (Instrument Definition File) editor.

JOYTOY		Sample joystick application.

LANGPLAY	Plays back multi-audio stream AVI files.
                MULTILNG.AVI is an example file found on
                the CD.

LOWPASS 	Low pass filter for waveform audio.

MCIAPP          MCI test application.

MCIPLAY 	Simple video playback application.
		Uses MCIWnd.

MCIPUZZL        Application lets you make a 15-square
                puzzle of a playing video sequence.  Shows
		how to use installable draw procedures.

MIDIMON 	Sample that displays textual listing of
                MIDI input messages.

MIDIPLYR        Sample MIDI player.

MIXAPP		Sample mixer application.

MMCAPS		Sample that shows the multimedia capabilities
		of the hardware.

MOVPLAY 	Simple application for playing movies using
                MCI.  Builds two versions, one using 
                mciSendCommand and another using mciSendString.

MPLAY           Simple AVI playback application.  Includes
		a subset of the features in Media Player 
		and uses the MCIWnd window class.

PALMAP		Stream handler that translates video
		streams into new streams of a different
		bit depth--for example, it can  translate 
                from 24-bit to 8-bit.  Copy dll to system
                directory and use with AVIVIEW, 'Find
                Optimal Palette' menu item.

REVERSE 	Simple application that will play a waveform
		in reverse.

SOUNDAPP	Simple application showing the SndPlaySound API.

TXTOUT		Text stream draw renderer for rendering
                text data in AVI files.  To install on Win95,
                copy dll to system dir and add the following
                line to [drivers32] section of system.ini:
                     txts.draw=txtout32.dll
                On NT, install from control panel, drivers
                applet, pointing to the sample directory.
                Example text stream is in OUTPUT.AVI, created
                by WRITEAVI sample.

VIDCAP          Full-featured video capture application.

WAVEFILE        Sample AVIFile file handler for waveform
                audio files.  To install, execute
                dseqfile.reg and copy dll to system dir.
                Can be used with AVIEDIT or AVIVIEW.

WRITEAVI        Example showing how to use the AVIFILE 
		interface to write files such as those 
		that AVIView reads.
