Microsoft(r) Agent supports the presentation of software agents as interactive personalities within the Microsoft Windows(r) interface. Microsoft(r) Agent comes with a cast of characters including Peedy, Merlin, Genie, and Robby. You can use these characters as interactive assistants to introduce, guide, entertain, or otherwise enhance Web pages or applications. Microsoft(r) Agent also includes tools for defining custom characters. 

IMPORTANT NOTE: Installing these files includes a license for you to use Microsoft Agent or any of its components. However, to distribute Agent components with your application, you must complete a Microsoft Agent distribution license agreement, which is available at the Microsoft Agent web site (www.microsoft.com/msagent).

Msagent.exe
This is a self-installing file that automatically installs Microsoft Agent to your Windows directory. This file will NOT install on Windows 2000 as the operating system installs Agent core components by default.

\Chars
This directory includes self-installing files that install the named Microsoft Agent character to the \chars subdirectory of \%windir%\msagent.

\Chars\Peedy.exe
Installs the Peedy parrot character file.

\Chars\Robby.exe
Installs the Robby robot character file.

\Chars\Genie.exe
Installs the Genie genie character file.

\Chars\Robby.exe
Installs the Merlin wizard character file.

\Intl
This subdirectory includes the installation files for Microsoft Agent Language Components. The language components allow you to set the language ID property of the character, which determines the language for word balloon text, the commands in the character's pop-up menu and the default language for speech input and output.  

         Language                    Filename
         --------                    -------- 
         Arabic                      Agtx0401.exe
         French                      Agtx040C.exe 
         German                      Agtx0407.exe 
         Hebrew                      Agtx040D.exe 
         Italian                     Agtx0410.exe 
         Japanese                    Agtx0411.exe 
         Korean                      Agtx0412.exe 
         Simplified Chinese          Agtx0804.exe 
         Spanish                     Agtx0C0A.exe 
         Traditional Chinese         Agtx0404.exe 

\Links
This subdirectory includes links to pages on the Microsoft Agent Web site where you can find more information.

\Links\MSAgent.url
Linkx to Microsoft Agent Web site homepage. You can also get information about licensing and logo programs.

\Links\Doc.url
Links to Microsoft Agent Documentation.

\Links\Download.url
Links to Microsoft Agent Download page. When more language components and speech engines of other languages are available they will be posted to this page.

\Links\WhatsNew.url
News about Microsoft Agent.

\Speech
This subdirectory includes the installation files for Microsoft Agent Speech components.

\Speech\ActCnC.exe
This is a self-installing file that automatically installs the Microsoft Speech Recognition Engine.

\Speech\SpchCpl.exe
This is a self-installing file that automatically installs the Speech Control Panel. It enables you to list the compatible speech recognition and text-to-speech engines installed on your system and to view and customize their settings.

\Speech\tv_enua.exe
This is a self-installing file that automatically installs the Lernout & Hauspie Text-To-Speech Engine.

The following Agent components can be installed through the Platform SDK setup:

Microsoft Agent Documentation
You can install Agent programming documentation under Documentation -> User Interface Servies -> Agent.  

Microsoft Agent Include Files
You can install Agent Include Files, which are required for compiling the Agent Visual C++ and MFC samples and if you plan to call the Agent Server COM interfaces from your C++ program, under Build Environment -> Web Services -> Microsoft Agent Headers and Libraries.

Microsoft Agent Tools
You can install the Microsoft Agent Character Editor, the Microsoft Linguistic Information Sound Editing Tool and the Office Assistant Palette if you plan to design characters for Microsoft Office under Tools -> Microsoft Agent Tools.

Microsoft Agent Samples
You can install Agent Visual Basic, Visual C++, Java, MFC and HTML samples under Sample Code  -> Graphics and Multimedia Samples -> Microsoft Agent Samples.

