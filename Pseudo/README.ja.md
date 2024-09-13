# Pseudo

Read this in other languages: [English](README.md), [日本語](README.ja.md)

* サンプルプログラムと接続してデータを送信するためのSiTCP疑似デバイス(Python)です。
	* 実行には[sitcpyライブラリ](https://github.com/BeeBeansTechnologies/sitcpy)が必要です。

## 動作環境
* Python 3.11.4で動作確認済です。

## 使用方法
* `pip`コマンドで`sitcpy`をインストールします。
	* 同梱されている`requirements.txt`を使用してのインストールも可能です。

```
pip install sitcpy
```


* 疑似プログラムを実行します.
```
python .\SiDaqPseudo.py
```

* オプション --port: TCPポート番号を指定できます。(既定値：24242)
* オプション --header: 送信するデータに付与するヘッダのbyte長を指定できます。(既定値：2)
	* ヘッダには、送信するデータフレームのbyte長が格納されます。
	* 指定したヘッダ長に対してデータフレーム長がおさまらない場合、ヘッダは付与されません。
* オプション --unit：一度に送信するデータの単位を指定します。(既定値：4096)
	* 一度の送信処理で、`8byte * 指定したデータ単位`の長さを持つデータフレームが作成されます。　
* 0.0.0.0で待ち受けが開始されます。
	* UDPポートは4660で固定です。　
* `SiDaqRun`で接続されると、自動的にデータ送信が開始されます。

## 起動コマンド例

```
python .\SiDaqPseudo.py --port 24 --header 0 --unit 1
```

* TCPポート番号24、データ長ヘッダなし、データフレーム長: 1 * 8 = 8byteで疑似プログラムが起動します。