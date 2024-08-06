#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
ThreeWire myWire(5, 6, 7);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire); 
#define SS_PIN 10   // Slave Select pin
#define RST_PIN 9   // Reset pin

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
const int num_total = 20;

int numVisitors = 490;

class Visitor {
  public:
    String name;
    int age;
    String id;
    int number_of_visits = 0;
    int last_visit_day = 0;
    int last_visit_month = 0;
    int last_visit_year = 0;
    
    Visitor() {
      name = "";
      age = 0;
      id = "000000";
    }

    Visitor(String name1, int age1, String id1) {
      name = name1;
      age = age1;
      id = id1;
    }
};
Visitor myArr[num_total];

void setup() {
  Serial.begin(9600);
  
  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522 reader
  
  lcd.init();
  lcd.backlight();

  // Register visitors
  Visitor v1("aziz", 22, "793C097F");

  myArr[0] = v1;

}

bool test(String uid) {
  int i = 0;
  while (i < num_total) {
    if (myArr[i].id == uid) {
      break;
    }
    i++;
  } RtcDateTime now = Rtc.GetDateTime();

  time_test:
      if(myArr[i].number_of_visits>0){
        
      if (!(myArr[i].last_visit_year< now.Year() )) {
        if(!(myArr[i].last_visit_month< now.Month())){
          if (!(myArr[i].last_visit_month< now.Month())) {
          lcd.setCursor(0, 0);
        lcd.print(" you have to ");
        lcd.setCursor(0, 1);
        lcd.print("24 hours ");
        delay(1500);  // Display message for 2 seconds
        lcd.clear(); 
        return false;
          
          }
          }

      }
      }
  numOfVisitor_test:
    if (numVisitors>500){
        lcd.setCursor(0, 0);
        lcd.print("there is no ");
        lcd.setCursor(0,1);
        lcd.print("place inside");
        delay(1500);  // Display message for 2 seconds
      lcd.clear();
        return false;

    }
  VisitorRegistration_test:
    if (i >= num_total) {
      lcd.setCursor(0, 0);
      lcd.print("Visitor not ");
      lcd.setCursor(0,1);
      lcd.print("registered");
      delay(1500);  // Display message for 2 seconds
        lcd.clear();
      return false;
    }
  numOfVisits_test:
    if (myArr[i].number_of_visits >= 3) {
      lcd.setCursor(0, 0);
      lcd.print("Visitor had ");
      lcd.setCursor(0,1);
      lcd.print("3 visits");
      delay(1500);  // Display message for 2 seconds
        lcd.clear();
      return false;
    }
 

  return true;
}

String getCardUID() {
  String uidString = ""; 

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0";  // Add leading zero for bytes less than 0x10
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);  // Append byte in HEX format
  }


  uidString.toUpperCase();

  Serial.println(uidString);

  // Halt PICC communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  // Ensure encryption is stopped

  return uidString;  // Return the UID
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    
    if (mfrc522.PICC_ReadCardSerial()) {
      String uid = getCardUID();
      Serial.print("UID: ");
      Serial.println(uid);

      if (test(uid)) {
        lcd.setCursor(0, 0);
        lcd.print("You can enter");
         int i = 0;
        while (i < num_total) {
          if (myArr[i].id == uid) {
            break;
          }
          i++;
        }
          lcd.setCursor(0, 1);
          lcd.print(myArr[i].name);
          RtcDateTime now = Rtc.GetDateTime();
          myArr[i].last_visit_day=now.Day();
          myArr[i].last_visit_month=now.Month();
          myArr[i].last_visit_year=now.Year();
          numVisitors++;
          
          myArr[i].number_of_visits++;
          

        

      delay(1500);  // Display message for 2 seconds
      lcd.clear();  // Clear the display
      } 
    
    } 
  }
 // Delay to avoid constant reading
}
