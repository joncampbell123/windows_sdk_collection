サンプル:  WinDbg エクステンションの作成


概要:

この WDBGEXTS サンプルプログラムは WinDbg のエクステンションの作成法を
デモするためのものです。これは、NTSD エクステンションの作成方法をデモす
るサンプルの一部です。

エクステンションは DLL へのエントリ ポイントです。エクステンションに
渡される引数は以下のものがあります。

    HANDLE hCurrentProcess   -	現在のプロセスのハンドル
				(エクステンションが呼び出されたときのもの)

    HANDLE hCurrentThread    -	現在のスレッドのハンドル
				(エクステンションが呼び出されたときのもの)

    DWORD CurrentPc 	     -	エクステンションが呼び出されたときの pc

    PWINDBG_EXTENSION_APIS lpExtensionApis - エクステンションによって
    				呼び出し可能な関数のアドレス

    LPSTR lpArgumentString   -	エクステンションのコマンドライン引数への
    				ポインタ

PWINDBG_EXTENSION_APIS の型宣言は \mstools\h\wdbgexts.h のヘッダー
ファイルで定義されています。

関数 __stdcall が確実に使用できるように makefile に-Gz オプション スイッチ
を指定することに注意してください。.


詳細:

エクスポート関数に関するの説明を以下に記します。

    igrep()
	パターン用の命令ストリームを検索します。

    str()
	ストリングのポインタから、ストリングの内容、その長さ、さらにその
	メモリ アドレスを出力します。

WDBGEXTS.DLL のコマンドを利用するためには、この DLL が PATH で指定されて
いるディレクトリにあることを確認してください。

このコマンドの使用方法は以下のようになります

    !wdbgexts.igrep [pattern [expression] ]

    !wdbgexts.str [string]
