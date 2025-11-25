//!!! put into resource - kentse.
static PSZ apszHeader[] =
{
    "/bd {bind def} bind def /ld {load def} bd /ed {exch def} bd /a /currentpoint ld",
    "/c {_snap curveto} bd /d /dup ld /e /eofill ld /f /fill ld /p /put ld",
    "/g /setgray ld /gr /grestore ld /gs /gsave ld /j /setlinejoin ld",
    "/L {_snap lineto} bd /M {_snap moveto} bd /n /newpath ld /cp /closepath ld",
    "/rlt /rlineto ld /rm /rmoveto ld /sl /setlinewidth ld /sd /setdash ld",
    "/r /setrgbcolor ld /s /stroke ld /t /show ld /aw /awidthshow ld /im /imagemask ld",
    "/SF {findfont exch scalefont setfont} bd /SM {cmtx setmatrix} bd",
    "/MF {findfont exch makefont setfont} bd /CM {/cmtx matrix currentmatrix def} bd",
    "/box {/b3 ed /b2 ed /b1 ed /b0 ed b0 b1 M b2 b1 L b2 b3 L b0 b3 L cp} bd",
    "/h {{pop pop 3 1 roll add 2 1 roll M a} exch kshow pop pop} bd",
    "/v {{pop pop 3 -1 roll add M a} exch kshow pop pop} bd",
    "/H {{pop pop M d 0 rm a} exch kshow pop pop pop} bd",
    "/V {{pop pop M d 0 2 1 roll rm a} exch kshow pop pop pop} bd",
    NULL
};

static PSZ apszEHandler[] =
{
    "/nl {currentpoint exch pop 100 exch 10 sub moveto} def",
    "errordict begin /handleerror {showpage 100 720 moveto",
    "/Courier-Bold findfont 10 scalefont setfont (ERROR: ) show",
    "errordict begin $error begin errorname =string cvs show",
    "nl (OFFENDING COMMAND: )show /command load =string cvs show",
    "nl nl (OPERAND STACK: )show $error /ostack known",
    "{ostack aload length {=string cvs nl show} repeat} if",
    "end end showpage stop} bind def end",
    NULL
};


#if 0
    "100 dict begin",
    "/B {bind def} bind def /st {exch def} B /a {currentpoint} B",
    "/_snap { transform .25 sub round .25 add exch .25 sub round .25 add exch itransform}B",
    "/c {_snap curveto} B /d {dup} B /e {eofill} B /f {fill} B /p {put} B",
    "/g {setgray} B /gr {grestore} B /gs {gsave} B /j {setlinejoin} B",
    "/l {_snap lineto} B /m {_snap moveto} B /n {newpath} B /cp {closepath} B",
    "/rl {rlineto} B /rm {rmoveto} B /w {setlinewidth} B /sd {setdash} B",
    "/r {setrgbcolor} B /s {stroke} B /t {show} B /aw {awidthshow} B /im {imagemask} B",
    "/SF {findfont exch scalefont setfont} B /SM {cmtx setmatrix} B",
    "/MF {findfont exch makefont setfont} B /CM {/cmtx matrix currentmatrix def} B",
    "/box {/b3 st /b2 st /b1 st /b0 st b0 b1 m b2 b1 l b2 b3 l b0 b3 l cp} B",
    "/h {{pop pop 3 1 roll add 2 1 roll m a} exch kshow pop pop} B",
    "/v {{pop pop 3 -1 roll add m a} exch kshow pop pop} B",
    "/H {{pop pop m d 0 rm a} exch kshow pop pop pop} B",
    "/V {{pop pop m d 0 2 1 roll rm a} exch kshow pop pop pop} B",
    NULL
#endif
