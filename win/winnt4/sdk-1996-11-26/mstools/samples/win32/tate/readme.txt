Sample: 縦書き デモンストレーション

要約：

このサンプル・プログラムは、縦書きフォントを指定して、
出力をおこなうプログラムです。

縦書き用のフォントを指定している関数は、GetUseFont()です。
縦書き用のフォントが選択された時には、LOGFONT構造体の、
lfEscapementとlfOrientationに2700(270度)を指定して
CreateFontIndirect APIを呼んでいることに注意してください。

また、出力をおこなう関数は、StringOut()で、この中で、
GetTextExtentPoint APIとTextOut APIを使用して、縦書きの
出力をおこなっています。