サンプル:  Simple Service

概要:

    Simple Service はサービスの作り方とインストールの方法のデモを行います。

    この場合、サービスは \\.\pipe\simple という名の名前付きパイプをオープン
    し、トラフィックを待ちます。何かを受信したときは、

        Hello! [<入力値はここに入ります>]

    というように入力値を囲み、クライアントへパイプを通じて返送します。

    このサービスは、コントロール パネルの [サービス] アプレットまたは
    net start/stop コマンドや下記に示すサービス コントローラ ユーティリティを
    使用して、開始、終了ができます。

    このサービスは、インストール、削除、コンソール アプリケーションとしてサ
    ービスを起動(debug)といったコマンド ライン パラメータも提供しています。

    サービスの出力/デバッグの為に、SDK には、サービスの状態の獲得、構成、制御
    を行うユーティリティ(MSTOOLS\BIN\SC.EXE)が用意されています。
    SC は、サービス データベース内の如何なるサービス/ドライバに対しても、状態
    を表示し、コマンド ラインにて容易に構成要素を変更することができます。
    SC.EXE についての詳細は、コマンド ラインにて SC とタイプしてください。


使用方法:

    このサービスをインストールするには、まずすべてをコンパイルし、次のように
    実行します。

        simple -install

    次に、以下のようにタイプして SC のコマンド ライン パラメータを確認してく
    ださい。

        sc

    あとは“net start”を使うかコントロール パネルの [サービス] アプレットを
    使うか、または次のように実行してこれを開始するかだけです。

        sc start simpleservice

    以下のようにタイプして、サービスが RUNNING の状態になっていることを確認し
    てください。

        sc query simpleservice

    いったんサービスが開始されれば、CLIENT プログラムを使ってそれが本当に稼働
    中であるか確認することができます。次の構文を使ってください。

            client

    次の応答が返ってくるはずです。

            Hello! [World]

    このサンプルを使った後で除去したいときは、次の構文を使います。

            Simple -remove

    その他: CLIENT と SIMPLE の両方でスタート アップ パラメータとして、
    "-pipe <pipename>" と指定することでパイプの名前を変更することもできます。
    CLIENT に渡す文字列を、"-string <string>" と記述することにより変更するこ
    ともできます。

    

詳細情報:

    SERVICE.H と SERVICE.C を使用することにより、サービス処理を記述することが
    簡単になります。開発者は単にヘッダー ファイル中の TODO の概略に従い、サポ
    ートしたいサービスの為の ServiceStart() と ServiceStop() 関数を記述してく
    ださい。

    SERVICE.C のコードを修正する必要はありません。単に、あなたのプロジェクト
    に SERVICE.C を追加し、下記のライブラリといっしょにリンクしてください。

    libcmt.lib kernel32.lib advapi.lib shell32.lib

    本コードは UNICODE もサポートしています。SERVICE.C とコード
    #include "sevice.h" を Unicode 設定でコンパイルして確認してください。

    完成したコードは、下記のコマンド ライン インターフェースを持つことになり
    ます。

    <service exe> -?                to display this list
    <service exe> -install          to install the service
    <service exe> -remove           to remove the service
    <service exe> -debug <params>   to run as a console app for debugging

    注意: 本コードは debug オプションを使うときの Ctrl+C と Ctrl+Break ハンド
          ラも記述されています。これらのコンソール イベントはあなたの 
          ServiceStop ルーチン呼び出しを実行します。



参照:

CloseServiceHandle, InitializeSecurityDescriptor,
SetSecurityDescriptorDacl, SetServiceStatus, OpenSCManager,
StartServiceCtrlDispatcher, RegisterEventSource,
DeregisterEventSource, RegisterServiceCtrlHandler,
SetConsoleCtrlHandler
