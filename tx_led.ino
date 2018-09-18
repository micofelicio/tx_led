// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX
 
#include <SPI.h>
#include <RH_RF95.h>
 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 933.0
// Change to 434.0 or other frequency, must match RX's freq!
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
#define RLED A5
#define YLED A4
#define GLED A3
int rssi_last = 0;
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(9600);
//  while (!Serial) {
//    delay(1);
//  }
  delay(100);
 
  Serial.println("Feather LoRa RX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  pinMode(GLED,OUTPUT);
  pinMode(YLED,OUTPUT);
  pinMode(RLED,OUTPUT);

}
 
void loop()
{
  delay(1000);
  uint8_t data[] = "ABC";
  Serial.print("sending...");
  Serial.println((char*)data);
  rf95.send(data, sizeof(data));

  rf95.waitPacketSent();
  
  if (rf95.waitAvailableTimeout(1000))
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    rf95.waitPacketSent();
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      rssi_indicator(rf95.lastRssi());
      //rssi_indicator(-30);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }//end of big if
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
  delay(500);
  turn_off();
}

//https://www.metageek.com/training/resources/understanding-rssi-2.html
void rssi_indicator(float rssi){
  Serial.println(rssi);

  if ( rssi < -90){// <-90
    digitalWrite(GLED,LOW);
    digitalWrite(YLED,LOW);
    digitalWrite(RLED,LOW);
    Serial.println("STATE 6");
  } else if ( rssi < -80 ){//-80 to -90
    digitalWrite(GLED,HIGH);
    digitalWrite(YLED,LOW);
    digitalWrite(RLED,LOW);
    Serial.println("STATE 5");
  } else if ( rssi < -70 ){//-70 to -80
    digitalWrite(GLED,LOW);
    digitalWrite(YLED,HIGH);
    digitalWrite(RLED,LOW);
    Serial.println("STATE 4");
  } else if ( rssi < -60 ){//-60 to -70
    digitalWrite(GLED,LOW);
    digitalWrite(YLED,LOW);
    digitalWrite(RLED,HIGH);
    Serial.println("STATE 3");
  } else if ( rssi < -40 ){// -40 to -60
    digitalWrite(GLED,LOW);
    digitalWrite(YLED,HIGH);
    digitalWrite(RLED,HIGH);     
    Serial.println("STATE 2");
  } else if (rssi >= -40){// > -40
    digitalWrite(GLED,HIGH);
    digitalWrite(YLED,HIGH);
    digitalWrite(RLED,HIGH);
    Serial.println("STATE 1");
  }
  //delay(1500);
}


void turn_off(){
  digitalWrite(GLED,LOW);
  digitalWrite(YLED,LOW);
  digitalWrite(RLED,LOW);
}
