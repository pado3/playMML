# playMML
Arduino beep player for HP 100/200LX .SND file

HP 100/200LX用のSNDファイル、および みゅうさんが<a href="https://www.vector.co.jp/soft/dos/util/se013900.html" target="_blank" rel="noopener">playEX</a>で定義されたMMLフォーマットの内容を、Arduinoで演奏するスケッチです。<br />
ATmega328PおよびATtiny85で、RAMまたはEEPROMから読み込んで演奏します。
<p></p>
ATmega328P用:<br />
playMML_EEPROMw.ino EEPROMへの書き込みスケッチ<br />
playMML.ino デコードし演奏するスケッチ<br />
<p></p>
ATtiny85用:<br />
playMMLt_EEPROMw.ino EEPROMへの書き込みスケッチ<br />
playMMLt.ino デコードし演奏するスケッチ<br />
<p></p>
共通:<br />
score_sample.txt 楽譜のサンプル（ボレロ、愛の挨拶、むすんでひらいて、チャルメラ、ピポ音）
<p></p>
MMLフォーマットの内容については、上記playEXのドキュメントをご参照下さい。
<p></p>
また、当スケッチに関連した内容を次のブログに書いています。適宜ご参照下さい:<br />
http://pado.tea-nifty.com/top/2020/11/post-462c6c.html
<p></p>
ご利用は自己責任でお願いします。<br />
