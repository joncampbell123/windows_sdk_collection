サンプル:  Windows Sockets (ソケット) API のデモ

概要: 

WSOCK は基本的なソケット プログラミングのデモを行うサンプルです。
ソケットは Windows Sockets を使います。このサンプルは、(Windows Sockets 
非同期エクステンション API や、スレッド、さらに従来の BSD スタイルの
ブロッキング呼び出し経由で) 入ってくる接続要求の受け入れ方とリモート 
ホストへの接続の方法のデモを行います。いったん接続されると、リモート 
ホストへテキスト文字列を送信することができます。また WSOCK では、ユー
ザーにより入力された名前のホストの情報を表示することもできます。


詳細:

プログラムが正しく作動するには、TCP/IP プロトコルが正しくインストール
されていなければなりません。また、ネットワーク上の 2 台のマシンを使う
ときは、両方のマシンに HOSTS テキスト ファイルが必要です。(Windows NT 
マシンでは、このファイルは %SYSTEMROOT%\SYSTEM32\DRIVERS\ETC\HOSTS 
にあります。) それぞれの HOSTS ファイルに、両方のマシンのリモート アド
レスとローカル アドレスがリストされている必要があります。

WSOCK は マシン 1 台でも (この場合 WSOCK を 2 つ実行)、ネットワーク上の 
Win32 マシン 2 台でも実行できます。以下の例ではネットワーク上の 2 台の
離れた Win32 マシンによるWSOCK のテスト方法を説明します。

1.  マシン "Bob" が WSOCK を実行します。

2.  マシン "Fred" が WSOCK を実行します。

3.  マシン "Bob" は (Winsock の下で) [Listen] メニューの 3 つのオプ
    ション、 [Listen (Blocking)]、[Listen With Threads]、[Async Listen] 
    の 1 つを選択します。

4.  マシン "Fred" は WinSock の下で [Connect] メニュー オプションを
    選択します。

5.  マシン "Bob" は TCP の ポート番号として 12 を入力します。

6.  マシン "Bob" は接続を待ちます。

7.  マシン "Fred" が接続先のホスト名として「ボブ」を入力します。

8.  マシン "Fred" が TCP の ポート番号として 12 を入力します。

これで両方のマシンが接続され、Winsock の [Send Message] メニュー オプ
ションを使って文字列の送受信ができます。

もし接続中に "Bob" が WSOCK を終了したときは、「フレッド」はメッセージ 
ボックスの通知を受け取ります。

使用する Windows Sockets 呼び出し:

   accept
   closesocket
   connect
   gethostbyname
   getservbyname
   htons
   listen
   send
   recv
   WSAAsyncSelect
   WSACleanup
   WSAStartup
