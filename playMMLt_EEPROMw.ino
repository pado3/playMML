#include <EEPROM.h>
#define LED     3       // LED on 3=pin2
#define ESIZE   512     // EEPROM size in byte
boolean WRITE1 = true;  // write flag
boolean ERASE1 = true;  // erase flag

// if RAM is short with large score, split score with comment out and K0
int K0 = 233;  // Start address, ボレロ:0 愛の挨拶-pipo:233 計502
char *s[]={
/*
  "::[playEX]",//ラヴェル・ボレロbyパドラッパ
  "k1@t70l16m'2o5",
  "c4_c8<b>cdc<ba>c8c<a>c4_c8<b>c<agefg2_gfedefgag4_",
  "g4_gabagfededc8_c8_cde8'f8'd4g2_g2_g8p8 ",
  ">d4_d6_c<bab>cdc<b4_b>c<ba>c<baf_f8f'f'f8'a8'>c<abgf8f'f'f8'a8'bgaf",
  "d8dcd4d8d'd'd8'f8'afged8dcd4_d8dcd8efg2_gfedc8p8p2",
  ":://"
*/
// split line for ATtiny85

  "::[playEX]",//エルガー・愛の挨拶byパドラッパ
  "k1@t90l8o5~c+~d+~f+~g+",
  "g4<b>gfede a4a4a4<b>p",
  "g4<b+>gfede f4f4f3f+",
  "g4<b>gfede >c4c4c4<",
  "bag4fe c4d4e2p2",
  ":://"
  "::[playEX]",//むすんでひらいてbyパドラッパ
  "k1@t120l8m6o5",
  "e4edc4c4d4d4edcpg4gfe4e4dcdec4p4",
  "e4efg4g4a4a4gfepe4efg4g4a4a4g2p",
  ":://"
  "::[playEX]", // チャルメラ by パドラッパ
  "k1@t100l8m5mo4",
  "cde2_dcpcdedcd2_p1",
  ":://"
  "::[playEX]",//pipo
  "@t150N94N82p:://"

};

void setup()
{
  int i, j, k=K0, l=0, R;
  char c;
  // serial setup and initial message
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("START " __FILE__ " from " __DATE__ " " __TIME__));
  // set pinMode
  pinMode(LED, OUTPUT);
  // data count
  R = sizeof(s)/sizeof(*s);
  Serial.print(F("score row count:"));
  Serial.println(R);
  for (i=0; i< R; i++) {
    for (j=0; j<strlen(s[i]); j++) {
      if (j==0 and s[i][j]==':' and s[i][j+1]!=':') { // comment line
        i++; j--;
      } else if(s[i][j] != ' ') { // truncate sp
        // Serial.print(s[i][j]);
        l++;
      }
    }
    // Serial.println();
  }
  Serial.print(F("desinated start address K0:"));
  Serial.print(K0);
  Serial.print(F(", charactor count:"));
  Serial.print(l);
  Serial.print(F(", estimated last address:"));
  Serial.println(k+l-1);  // start address is zero, ignore last increment
  if (k+l > ESIZE) {
    Serial.println(F("!!!!!!!!!! SIZE OVER !!!!!!!!!!"));
    WRITE1 = 0;
  } else {
  }
  if (WRITE1) {
    Serial.println(F("===== EEPROM writing, wait for blink LED ====="));
    digitalWrite(LED, HIGH); // ON
  } else {
    Serial.println(F("do not write to EEPROM"));
  }
  if(WRITE1) {
    for (i=0; i<R; i++) {
      for (j=0; j<strlen(s[i]); j++) {
        if (j==0 and s[i][j]==':' and s[i][j+1]!=':') { // comment line
          Serial.print(F("[Skip comment line]"));
          Serial.println();
          i++; j--;
        } else if(s[i][j] != ' ') { // truncate sp
          Serial.print(s[i][j]);
          EEPROM.write(k, s[i][j]);
          k++;
        }
      }
      Serial.println();
    }
  }
  Serial.print(F("written character count:"));
  Serial.print(k-K0);
  Serial.print(F(", last address:"));
  Serial.println(k-1);  // start address is zero, ignore last increment
  if (ERASE1) {
    Serial.println(F("Erase rest of EEPROM"));
    for (i=k; i<ESIZE; i++) {
      EEPROM.write(i, 0x00);
      Serial.print(F("~"));
    }
    Serial.println();
  }
  Serial.println(F("===== read from EEPROM ====="));
  for (i=0; i<ESIZE; i++) {
    c = (char)EEPROM.read(i);
    Serial.print(c);
    if (c==0x00) {
      Serial.print("~");
    }
    if (i%100 == 0) {
      Serial.println();
    }
  }
  Serial.println();
  Serial.println(F("===== finish ====="));
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, LOW);  // OFF
  delay(100);
  digitalWrite(LED, HIGH); // ON
  if (WRITE1 == 1) {  // ちゃんと書けたと思われるときは長く点灯
    delay(1000);
  } else {
    delay(100);
  }
}

// end of program
