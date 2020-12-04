#include <EEPROM.h>
#include <avr/sleep.h>
#define RAM     1     // 1: use RAM data, 0: use EEPROM data
#define ILLUMI  1     // 1: Sound illumination ON
#define ESIZE   512   // EEPROM size in byte
#define G2      3     // GND for V2 on 3=pin2 HiZ w/input, GND w/OUTPUT_LOW
#define G1      4     // GND for V1 on 4=pin3 HiZ w/input, GND w/OUTPUT_LOW
#define SP      0     // Speaker on 0=pin5, need AMP for volume control
#define LED     1     // LED on 1=pin6
#define KILL    2     // Kill SW on 2=pin7
#define INTN    0     // Kill SW intruppt No.0=2=pin7

unsigned int L=8, O=4, T=100, aT=100;   // playEX default value from docs
unsigned int V=3, K=0, M=6, Ms=2, TR=0; // default unknown. TR:Transposition
byte Af=0, Bf=0, Cf=0, Df=0, Ef=0, Ff=0, Gf=0;  // key signal ♭,♭♭
byte As=0, Bs=0, Cs=0, Ds=0, Es=0, Fs=0, Gs=0;  // key signal #,x
boolean Q=false;  // set true for terminate
unsigned int Start, Next;  // EEPROM data start address, Next address
int decMML(unsigned int);  // MML decoder
void quit(void);  // terminate to play
char sr(unsigned int);     // snd data reader with touuper
void setint(boolean); // interrupt switch
void powerdown(void); // enter power down mode
void play(int, unsigned long, unsigned int);  // beep player

// put score here
char *s[]={
  "::[playEX]", // ルソーむすんでひらいて by パドラッパ
  "k1@t120l8m6o5",
  "v3",
  "e4edc4c4d4d4edcp",
  "v1",
  "g4gfe4e4dcdec4p4",
  "v2",
  "e4efg4g4a4a4gfep",
  "v3",
  "e4efg4g4a4a4g2p",
  ":://"

/*
  "::[playEX]", // volume test
  "k1@t60l1o5",
  "v3a",
  "v2a",
  "v1a",
  "v2a",
  "v3a",
  ":://"

  "::[playEX]", // test data for noise
  "k0@t70l16m'2o5",
  "c2_c4_c8_c_c2_c4_c8_c_g2_g4_g8_g_g2_g4_g8g_",
  ":://"

  "::[playEX]", // ルソーむすんでひらいて by パドラッパ
  "k1@t120l8m6o5",
  "v3",
  "e4edc4c4d4d4edcp",
  "v1",
  "g4gfe4e4dcdec4p4",
  "v2",
  "e4efg4g4a4a4gfep",
  "v3",
  "e4efg4g4a4a4g2p",
  ":://"

  "::[playEX]", // Mozart, Tulkish March, ATtiny85メモリぎりぎり
  "k1@t110l16m'5m8o4",
  "v3",
  "bag+a >c8p8dc<b>c e8p8fed+e bag+abag+a >c4<a8'>c8'",
  "<g32a32b32p32a8'g8'a8' g32a32b32p32a8'g8'a8' g32a32b32p32a8'g8'f+8'",
  "e4",
  "v2",
  "<bag+a >c8p8dc<b>c e8p8fed+e bag+abag+a >c4<a8'>c8'",
  "<g32a32b32p32a8'g8'a8' g32a32b32p32a8'g8'a8' g32a32b32p32a8'g8'f+8'",
  "e4",
  "v3e8'f8' g8'g8'agfe d8p8v1e8'f8' g8'g8'agfe d8p8v2c8'd8'",
  "e8'e8'fedc <b8p8v1>c8'd8' e8'e8'fedc <b8p8v3bag+a >c8p8dc<b>c",
  "e8p8fed+e bag+abag+a >c4v2<a8'b8'",
  ">c8'<b8'a8'g+8' a8'e8'f8'd8' c4<b>c<ba32b32",
  "a4",
  ":://",

  "::[playEX]",//エルガー愛の挨拶byパドラッパ
  "k1@t90l8o5~c+~d+~f+~g+",
  "g4<b>gfede a4a4a4<b>p",
  "g4<b+>gfede f4f4f3f+",
  "g4<b>gfede >c4c4c4<",
  "bag4fe c4d4e2p2",
  ":://"
  "::[playEX]", // チャルメラ by パドラッパ
  "k1@t100l8m5mo4",
  "cde2_dcpcdedcd2_p1",
  ":://"
  "::[playEX]",//pipo
  "@t150N94N82:://"
*/
};
// score end
int r=0, R=sizeof(s)/sizeof(*s);  // row counter and max row

/*
char *s[]={
  "::[playEX]", // むすんでひらいて by パドラッパ
  "k1@t120l8m6o5",
  ":w むすんで ひらいて てをうって むすんで",
  "e4edc4c4d4d4edcpg4gfe4e4dcdec4p4",
  ":w またひらいて てをうって そのてを○○に",
  "e4efg4g4a4a4gfepe4efg4g4a4a4g2p",
  ":://"
};

char *s[]={
  "::[playEX]", // チャルメラ by パドラッパ
  "k1@t100l8m5mo4",
  "cde2_dcpcdedcd2_p1",
  ":://"
};  // はらへ

char *s[]={
  "::[playEX]",//エルガー愛の挨拶byパドラッパ
  "k1@t90l8o5~c+~d+~f+~g+",
  "g4<b>gfede a4a4a4<b>p",
  "g4+<b>gfede f4f4f3f+",
  "g4<b>gfede >c4c4c4<",
  "bag4fe c4d4e2p2",
  ":://"
};

char *s[]={
  "::[playEX]",//pipo
  "@t150N94N82:://"
};
*/

void setup()
{
  // serial setup and initial message
  // Serial.begin(115200);
  delay(200);
  // Serial.println();
  // Serial.println(F("START " __FILE__ " from " __DATE__ " " __TIME__));
  // set pinMode
  pinMode(SP,   OUTPUT);
  pinMode(G1,   INPUT); // Hi-Z
  pinMode(G2,   INPUT); // Hi-Z
  pinMode(LED,  OUTPUT);
  pinMode(KILL, INPUT_PULLUP);
  Start = 0;  // Start address, EEPROMに余裕があれば選択できるようにするかも
  Next = Start;
  // show all data
  if (RAM == 1) {
    // Serial.println(F("use RAM data"));
    // Serial.print(F("total number of rows:"));
    // Serial.println(R);
  } else {
    // Serial.println(F("use EEPROM data"));
    unsigned int i=Start;
    char c;
    while (i < ESIZE) {
      c = sr(i);
      // Serial.print(c);
      i++;
      if (c == 0x00) {
        // Serial.print("[NULL]");
        i = ESIZE;
      }
      if (i%100 == 0) {
        // Serial.println();
      }
    }
  }
  // Serial.println();
  // MML decoder
  while (Q == false and r < R and sr(Next) != 0x00) {
    Next = decMML(Next);
    // Serial.print(F("next number of char:"));
    // Serial.println(Next);
  }
  // Serial.println(F(" ===== EOF ===== "));
  // パワーダウンモード移行、ATtiny85はbootloader書き込み(ヒューズビット)でBOD Disableにしておく
  powerdown();
}

void loop()
{
  // put your main code here, to run repeatedly:
  // MML decoder for loop play
  // Next = 0; r = 0;
  // Next = decMML(0);
}

/* local fucctions */
int decMML(unsigned int i) {  // MML(Music Macro Language) decoder
  char cc, nc;        // current char, next char
  int li;    // local counter
  int nnum, nlen;     // note number and length
  int MUTE=0;         // nnum for muting
  int SF;             // Sharp / Flat for debug
  byte point;         // 符点, 0-2
  boolean Sl=false, St=false; // flag for slur and staccato
  unsigned long slen; // sound length msec
  unsigned long mlen; // mute length msec
  // read data and decode MML
  while( i < ESIZE and r < R and Q == false) {
    cc = sr(i); // read No.i char from memory and set current char
    // Serial.print(cc);
    switch (cc) {
      /* 音符の高さ長さと休符の長さを指定してplay()へ渡す */
      case 'C': case 'D': case 'E': case 'F': case 'G': case 'A': case 'B':
      case 'N': case 'P':
        SF=TR;
        i++;  // ccを使ったらカウンタを進める。以下、考え方は同じ
        nc = sr(i); // read and set next char
        // note number setting
        if (cc == 'C') {  nnum =  0 + (O + 1)*12 + TR;
          if (nc == '/') { i++; } // Naturalは個別に設定
                                             else if (Cs!=0) {nnum+=Cs;SF+=Cs;} }
        if (cc == 'D') {  nnum =  2 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
          else if (Df!=0) {nnum-=Df;SF-=Df;} else if (Ds!=0) {nnum+=Ds;SF+=Ds;} }
        if (cc == 'E') {  nnum =  4 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
          else if (Ef!=0) {nnum-=Ef;SF-=Ef;} }
        if (cc == 'F') {  nnum =  5 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
                                             else if (Fs!=0) {nnum+=Fs;SF+=Fs;} }
        if (cc == 'G') {  nnum =  7 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
          else if (Gf!=0) {nnum-=Gf;SF-=Gf;} else if (Gs!=0) {nnum+=Gs;SF+=Gs;} }
        if (cc == 'A') {  nnum =  9 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
          else if (Af!=0) {nnum-=Af;SF-=Af;} else if (As!=0) {nnum+=As;SF+=As;} }
        if (cc == 'B') {  nnum = 11 + (O + 1)*12 + TR;
          if (nc == '/') { i++; }
          else if (Bf!=0) {nnum-=Bf;SF-=Bf;} }
        // A-G sharp, flatは一括で設定
        if ( 0x41 <= cc and cc <= 0x47) { // A to G
          if ( nc == '+' or nc == '#') {
            nnum++; SF++; i++; nc = sr(i);
            if (nc == '+' or nc == '#') {  // double sharp
              nnum++; SF++; i++;
            }
          }
          else if (nc == '-') {
            nnum--; SF--; i++; nc = sr(i);
            if (nc == '-') { // double flat
              nnum--; SF--; i++;
            }
          }
        } else if (cc == 'N') {  // note number direct
          if (0x30 <= nc and nc <= 0x39) { // 0 to 9
            nnum = (unsigned int)(nc - 0x30);
            i++;
            if (0x30 <= (nc = sr(i)) and nc <= 0x39) {
              nnum = nnum*10 + (unsigned int)(nc - 0x30);
              i++;
              if (0x30 <= (nc = sr(i)) and nc <= 0x39) {
                nnum = nnum*10 + (unsigned int)(nc - 0x30);
                i++;
              }
            }
            if (127 < nnum) nnum = MUTE; // 定義越え異常、消音
            nnum += TR; // 移調の影響は受ける
          }
        } else if (cc == 'P') {  nnum = MUTE; }  // Pauseパウゼ
        
        // note length setting
        if (cc == 'N') {  // N指定時は仕様不明のため長さ調整しない
          nlen = L;
        } else {  // 音符または休符時の長さ
          nc = sr(i);
          if ( 0x30 <= nc and nc <= 0x39 ) {    // nlen指定あり 0 to 9
            nlen = (unsigned int)(nc - 0x30);
            i++;
            nc = sr(i);
            if ( 0x30 <= nc and nc <= 0x39 ) {  // nlenが2ケタのとき
              nlen = nlen*10 + (unsigned int)(nc - 0x30);
              if (64 < nlen) nlen = 64;         // max 64
              i++;
            }
          } else {  // nlen指定の無いとき
            nlen = L;
          }
          // 符点、スラー、スタッカート
          if ( sr(i) == '.') {  // 符点point、intで演算せずmsecにしてから反映
            point = 1;
            i++;
            if ( sr(i) == '.')  {
              point++;  // double point
              i++;
            }
          }
          if ( sr(i) == '_') {   // slur or tie, "._"もある
            Sl = true;
            i++;
            while( sr(i) == ' ')  { // _後のspは飛ばす（結構そういう作例あり）
              i++;
            }
          } else if (nc == 0x27) {   // ' Staccatoとslur, tieは排他
            St = true;
            i++;
          }
        }
/*
*/
        if (SF<0) { // ♭, ♭♭
          for (li=0; SF<li; li--) {////
            // Serial.print(F("-"));
          }
        } else if (SF>0) {  // #, ##
          for (li=0; li<SF; li++) {
            // Serial.print(F("+"));
          }
        }
        // Serial.print(F(" nlen "));
        // Serial.print(nlen);
        // Serial.print(F(" pt."));
        // Serial.print(point);
        // Serial.print(F(" sl."));
        // Serial.print(Sl);
        // Serial.print(F(" st."));
        // Serial.print(St);
        //
        // 基本の長さ
        slen = 240000 / ((unsigned int) (aT * nlen));  // 1分msec/((BPM/4)*nlen)
        // タイより先に符点を処理（みゅうさんご自身の譜もそうなっている）
        if (point == 1)  {
          slen = slen *15/10;
        } else if (point == 2) {
          slen = slen *175/100;
        }
        point = 0;
        // Slurの掛かっていないとき, M,M'に支配される。音長を8とした場合の割合
        if (not Sl) {
          if (St) { // Staccato
            slen = slen * Ms/8;
            mlen = slen * (8-Ms)/8; // mute length
            St = false;
          } else {  // Standard
            slen = slen * M/8;
            mlen = slen * (8-M)/8;
          }
        } else {  // Slue
          mlen = 0; // no mute
          Sl = false;
        }
        // いよいよ鳴らす
        play(nnum, slen, V);
        if (mlen > 0) play(MUTE, mlen, V);
        break;
      /* 調号, シャープやフラットの位置を指定 */
      case '~':
        i++;
        cc = sr(i);
        if (cc == 'C') {
          i++;
          if ((nc = sr(i)) == '/')  {Cf=0; Cs=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Cf=1; Cs=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Cf=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Cf=0; Cs=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Cs=2;       i++;} }// ダブルシャープ
        } else if (cc == 'D') {
          i++;
          if ((nc = sr(i)) == '/')  {Df=0; Ds=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Df=1; Ds=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Df=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Df=0; Ds=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Ds=2;       i++;} }// ダブルシャープ
        } else if (cc == 'E') {
          i++;
          if ((nc = sr(i)) == '/')  {Ef=0; Es=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Ef=1; Es=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Ef=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Ef=0; Es=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Es=2;       i++;} }// ダブルシャープ
        } else if (cc == 'F') {
          i++;
          if ((nc = sr(i)) == '/')  {Ff=0; Fs=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Ff=1; Fs=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Ff=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Ff=0; Fs=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Fs=2;       i++;} }// ダブルシャープ
        } else if (cc == 'G') {
          i++;
          if ((nc = sr(i)) == '/')  {Gf=0; Gs=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Gf=1; Gs=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Gf=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Gf=0; Gs=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Gs=2;       i++;} }// ダブルシャープ
        } else if (cc == 'A') {
          i++;
          if ((nc = sr(i)) == '/')  {Af=0; As=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Af=1; As=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Af=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Af=0; As=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){As=2;       i++;} }// ダブルシャープ
        } else if (cc == 'B') {
          i++;
          if ((nc = sr(i)) == '/')  {Bf=0; Bs=0; i++;}  // ナチュラル
          else if ( nc == '-' )     {Bf=1; Bs=0; i++;   // フラット
            if ((nc = sr(i)) == '-'){Bf=2;       i++;} }// ダブルフラット
          else if ( nc == '+' )     {Bf=0; Bs=1; i++;   // シャープ
            if ((nc = sr(i)) == '+'){Bs=2;       i++;} }// ダブルシャープ
        }
        // Serial.print(F(" set +/-"));
        break;
      /* 移調、以降のキーを指定されただけずらす。非指定時は戻す */
      case '!':
        i++;
        cc = sr(i);
        if ( cc == '+' ) {
          i++;
          if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) {  // 0 to 9
            TR = (int)(nc - 0x30);
            i++;
          }
        } else if (cc == '-' ) {
          i++;
          if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) {  // 0 to 9
            TR = -(int)(nc - 0x30);
            i++;
          }
        } else {
          TR = 0;
        }
        // Serial.print(F(" set transportation"));
        // Serial.print(cc);
        // Serial.print(TR);
        break;
      /* 音符・休符の長さ。非指定時はデフォルトの8に戻す */
      case 'L':
        i++;
        if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) {  // 0 to 9
          L = (unsigned int)(nc - 0x30);
          i++;
          if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) {
            L = L*10 + (unsigned int)(nc - 0x30);
            if (64 < L) L = 64; // max 64
            i++;
          }
        } else {
          L = 8;
        }
        // Serial.print(F(" Set L to "));
        // Serial.print(L);
        break;
      /* オクターブ指定、直接と相対 */
      case 'O':
        i++;
        if ( (0x30 <= (nc = sr(i)) and nc <= 0x37) ) { // 0 to 7
          O = (unsigned int)(nc - 0x30);
          i++;
        } // 0-7でなければ何もしない。tone()は1以上しか鳴らないがplay()でチェック
        // Serial.print(F(" Set Octave to "));
        // Serial.print(O);
        break;
      case '>':
        i++;
        O++;
        if (7 < O) O = 7; // max 7
        // Serial.print(F(" Octave shift up to "));
        // Serial.print(O);
        break;
      case '<':
        i++;
        if (O < 1) O = 1; // uintのOをデクリメントするのでmin 1
        O--;
        // Serial.print(F(" Octave shift down to "));
        // Serial.print(O);
        break;
      /* テンポ指定 */
      case 'T':
        i++;
        if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
          T = (unsigned int)(nc - 0x30);
          i++;
          if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
            T = T*10 + (unsigned int)(nc - 0x30);
            i++;
            if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
              T = T*10 + (unsigned int)(nc - 0x30);
              i++;
            }
          }
        } else {  // 指定が無ければデフォに戻す
          T = 100;
        }
        if (T   < 32) T =  32;
        if (255 < T ) T = 255;
        aT = 100 + (T-100)*0.5; // Tの意味がよく分からないので適当に調整
        // Serial.print(F(" Set T to "));
        // Serial.print(T);
        break;
      case '@':
        i++;
        if (sr(i) == 'T') {  // BPM
          i++;
          if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
            aT = (unsigned int)(nc - 0x30);
            i++;
            if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
              aT = aT*10 + (unsigned int)(nc - 0x30);
              i++;
              if ( (0x30 <= (nc = sr(i)) and nc <= 0x39) ) { // 0 to 9
                aT = aT*10 + (unsigned int)(nc - 0x30);
                i++;
              }
            }
          } else { // 指定が無ければデフォに戻す
            aT = 100;
          }
        }
        if (255 < aT) aT = 255;
        // Serial.print(F(" Set @T to "));
        // Serial.print(aT);
        break;
      /* 音量指定、Arduinoのtoneでは中間値無効=>テスト中 */
      case 'V':
        i++;
        if ( (0x30 <= (nc = sr(i)) and nc <= 0x33) ) { // 0 to 3
          V = (unsigned int)(nc - 0x30);
          if (V == 1) {
            pinMode(G1, OUTPUT);    // Low-Z
            digitalWrite(G1, LOW);  // GND
            pinMode(G2, INPUT);     // High-Z
          } else if (V == 2) {
            pinMode(G2, OUTPUT);    // Low-Z
            digitalWrite(G2, LOW);  // GND
            pinMode(G1, INPUT);     // High-Z
          } else {  // V==0 or 3, set default
            pinMode(G1, INPUT);     // High-Z
            pinMode(G2, INPUT);     // High-Z
          }
          i++;
        } else {
          V = 3;
        }
        // Serial.print(F(" Set V to "));
        // Serial.print(V);
        break;
      /* キー割り込み、有効化するとATmegaでは8kB over, ATtinyでは7kB程度 */
      case 'K':
        i++;
        if ( (0x30 <= (nc = sr(i)) and nc <= 0x31) ) { // 0 to 1
          K = (unsigned int)(nc - 0x30);
          i++;
        } else {
          K = 1;
        }
        if ( K == 1 ) {
          setint(true);
        } else {
          setint(false);
        }
        // Serial.print(F(" Set K to "));
        // Serial.print(K);
        break;
      /* 音符の発音の長さ */
      case 'M':
        i++;
        if ( (0x31 <= (nc = sr(i)) and nc <= 0x38) ) { // 1 to 8
          M = (unsigned int)(nc - 0x30);
          i++;
        } else if (nc == 'S') {
          M = 4;
          i++;
        } else if (nc == 'M') {
          M = 6;
          i++;
        } else if (nc == 'L') {
          M = 8;
          i++;
        } else if (nc == 0x27) {  // シングルコーテーション、スタッカートの長さ指定
          i++;
          if ( (0x31 <= (nc = sr(i)) and nc <= 0x38) ) { // 1 to 8
            Ms = (unsigned int)(nc - 0x30);
            i++;
          } else if (nc == 'S') {
            Ms = 2;
            i++;
          } else if (nc == 'M') {
            Ms = 4;
            i++;
          } else if (nc == 'L') {
            Ms = 6;
            i++;
          }
        } else {
          M = 6;
          Ms = 2;
        }
        // Serial.print(F(" Set M', M to "));
        // Serial.print(Ms);
        // Serial.print(F(", "));
        // Serial.print(M);
        break;
      case ':': // header or footer or comments
        i++;
        nc = sr(i);
        if (nc == ':') {  // ::
          i++;
          nc = sr(i);
          if(nc == '[') { // ::[ check header
            li = 0;
            char header[] = "[PLAYEX]";
            while(nc == header[li]) {
              i++; li++;
              nc = sr(i);
              if (nc == 0x00) break;
            }
            if (li == 8) {
              // Serial.println();
              // Serial.println(F("playEX header found"));
              // カウンタを進めただけ;これしないとAとEが鳴る
            }
          } else if(nc == '/') { // ::/ check footer
            i++;
            nc = sr(i);
            if(nc == '/') { // ::// 終端条件
              i++;
              // Serial.println();
              // Serial.println(F("playEX footer found, reset play parameters"));
              L=8, O=4, T=100, aT=100, V=3, K=0, M=6, Ms=2; TR=0;
              Af=0, Bf=0, Cf=0, Df=0, Ef=0, Ff=0, Gf=0;
              As=0, Bs=0, Cs=0, Ds=0, Es=0, Fs=0, Gs=0;
              return i++; // set Next
            }
          }
        } else {  // EEPROMにはコメントを入れない。あるとしたらRAM
          // Serial.print(F(" skip comment"));
          r++;  // 次行に飛ばす
          int tmps = 0; // 次行までの文字数
          for (li=0; li<r; li++) {
            tmps = tmps + strlen(s[li]);
          }
          i = tmps; // iを次行までの文字数に設定、これで次行の頭から続く
        }
        break;
      case ' ':
        i++;
        // Serial.print(F("sp "));
        while (sr(i) == ' ') { // discard series sp
          i++;
          // Serial.print(F("sp "));
        }
        break;
      case 0x00:  // char delimiter
        // Serial.println(" 0x00, delimiter of char[]");
        //i++;  // sr()でr++しているので、ここでi++すると次の行頭が欠ける
        break;
      default:
        // Serial.print(F(" gomi?"));
        i++;
        break;
    }
    // Serial.println();
  }
}

/* terminate to play */
void quit() {
  setint(false);  // Key intrrupt OFF for prevent chattering
  // Serial.println();
  // Serial.println(F("finish with key interruption"));
  Q = true; // stop decMML()
  V = 0;    // mute play()
}

/* read snd data from RAM/EEPROM. you may modify for your lib|PF */
char sr(unsigned int i) {
  char tmpc;        // ローカルchar
  if (RAM == 1) { // read from RAM (s[row][])
    int li, tmps=0; // ローカルカウンタ, 前行までの総文字数
    for (li=0; li<r; li++) {
      tmps = tmps + strlen(s[li]);
    }
    tmpc = toupper(s[r][i-tmps]);
/*
    // Serial.println();
    // Serial.print(" i:");
    // Serial.print(i);
    // Serial.print(" r:");
    // Serial.print(r);
    // Serial.print(" tmps:");
    // Serial.print(tmps);
    // Serial.print(" i-tmps:");
    // Serial.print(i-tmps);
    // Serial.print(" tmpc:");
    // Serial.print(tmpc);
    // Serial.println();
*/
    //
    if (tmpc == 0x00) {
      r++;
      // Serial.print(" r++");
    }
  } else {  // read from EEPROM
    if (i < ESIZE) {
      tmpc = toupper((char)EEPROM.read(i));
    } else {  // 
      // Serial.println(F(" Read address exceed EEPROM size"));
      Q = true;
      tmpc = 0x00;
    }
/*
    // Serial.println();
    // Serial.print(" i:");
    // Serial.print(i);
    // Serial.print(" Q:");
    // Serial.print(Q);
    // Serial.print(" tmpc:");
    // Serial.print(tmpc);
    // Serial.println();
*/
    //
  }
  return tmpc;
}

/* key interruption w/Arduino. You may modify for your lib|PF */
void setint(boolean sw) {
  if (sw == true) {
    attachInterrupt(INTN, quit, LOW);  // pulluped INTN is LOW, invoke quit()
  } else {
    detachInterrupt(INTN); // Key intrrupt OFF
  }
}

/* power down mode w/Arduino. You may modify for your lib|PF */
void powerdown(void) {  // ATtiny85はbootloader書き込み(ヒューズビット)でBOD Disableにしておく
  // ACSR  = 0x00; // AD比較器停止0x80（使うときは0x00）関係無かった
  // PRR = 0x00; // 電力削減レジスタまとめてゼロ（初期値通り） 関係無かった
  // GIMSK = 0x00; // 割り込み不許可0x00, 許可0x60 関係無かった
  ADCSRA = 0x00; // ADC停止、使うときは0x80（たぶんpinModeで設定） これをしないと330uA
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode(); // sleep_(enable + cpu + disable), 割り込みで戻るときに使いやすい
}

/* beep player w/Arduino tone(). you may modify for your lib|PF */
void play(int nnum, unsigned long mlen, unsigned int V) {
  unsigned int f; // frequeccy in Hz
  int c=1000;     // frequeccy compensation factor, no need by pre-experiment
/*
*/
  // Serial.print(F(", PLAY nnum "));
  // Serial.print(nnum);
  // Serial.print(F(","));
  // Serial.print(mlen);
  // Serial.print(F("ms"));
  // Serial.print(F(",vol"));
  // Serial.print(V);
  //
  if (nnum < 24) {  // minimum nnum=24 (tone min. 31Hz)
    V = 0;
    // Serial.print(F("[mute]"));
  }
  if (V == 0) {
    noTone(SP);
    delay(mlen);
  } else {
    f = (unsigned int)( 440*pow(2, (float)(nnum-69)/12) *(c/1000) );
    if (ILLUMI == 1 ) digitalWrite(LED, HIGH);
    tone(SP, f, mlen);
    delay(mlen);
    if (ILLUMI == 1 ) digitalWrite(LED, LOW);
  }
}

// end of program
