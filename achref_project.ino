#include<Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
// Pin definitions
Servo outing_servo;
#define SS_PIN 10   // Slave Select pin for RFID
#define RST_PIN 9   // Reset pin for RFID
int outing_sensor = 4 ;
// RTC DS1302 Wiring: DAT, CLK, RST
ThreeWire myWire(5, 6, 7);
RtcDS1302<ThreeWire> Rtc(myWire);

// LCD setup
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// RFID setup
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Maximum number of visitors
const int num_total = 20;
const int numVisitorsInsideMax = 2;
int numVisitorsInside = 0;
int j=0;

// Visitor class to store visitor information
class Visitor {
public:
    String name;
    int age;
    String id;
    int number_of_visits = 0;
    int last_visit_day = 0;
    int last_visit_month = 0;
    int last_visit_year = 0;
    int last_visit_minute = 0;

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

// Arrays to store all visitors and those inside
Visitor allvisitor[num_total];

String visitorInside[numVisitorsInsideMax];
void setup() {
  outing_servo.attach(3);
    Serial.begin(9600);
    
    pinMode(4,INPUT);
    SPI.begin();
    mfrc522.PCD_Init();

    // Initialize LCD
    lcd.init();
    lcd.backlight();



  Visitor v1("folen1", 22, "793C097F"); // First visitor
  Visitor v2("folen2", 30, "FB17AB1B");
  Visitor v3("folen3", 17, "899BEAC9");
allvisitor[0] = v1;
allvisitor[1] = v2; 

}
bool isElementInArray(String element, String array[], int size) {
  for (int i = 0; i < size; i++) {
    if (array[i] == element) {
      return true; // Element found
    }
  }
  return false; // Element not found
}
bool test(String uid) {
    Serial.println("Checking UID against registered visitors...");

    int i = 0;
    while (i < num_total) {
        if (allvisitor[i].id == uid) {
            break; // Match found
        }
        i++;
    }

    // If no matching UID is found
    if (allvisitor[i].id != uid) {
        lcd.setCursor(0, 0);
        lcd.print("Visitor not ");
        lcd.setCursor(0, 1);
        lcd.print("registered");
        delay(1500);
        lcd.clear();
        return false;
    }
    
    // Get current date and time
    RtcDateTime now = Rtc.GetDateTime();


     
      if(isElementInArray(uid, visitorInside, numVisitorsInsideMax)){
        lcd.setCursor(0, 0);
        lcd.print("Visitor is  ");
        lcd.setCursor(0, 1);
        lcd.print("inside");
        delay(1500);
        lcd.clear();
        return false;
      }
    


    if (allvisitor[i].number_of_visits >= 3) {
        lcd.setCursor(0, 0);
        lcd.print("Visitor had ");
        lcd.setCursor(0, 1);
        lcd.print("3 visits");
        delay(1500);
        lcd.clear();
        return false;
    }
    // Check if the visitor has to wait for a certain period
    if (allvisitor[i].number_of_visits > 0) {
        if (abs(allvisitor[i].last_visit_minute - now.Minute()) < 1) {
            lcd.setCursor(0, 0);
            lcd.print("You have to ");
            lcd.setCursor(0, 1);
            lcd.print("wait 1 minute");
            delay(1500);
            lcd.clear();
            return false;
        }
    }

  if (numVisitorsInside >= numVisitorsInsideMax) {
        lcd.setCursor(0, 0);
        lcd.print("There is no ");
        lcd.setCursor(0, 1);
        lcd.print("place inside");
        delay(1500);
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

    Serial.print("Read UID: ");
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
                    if (allvisitor[i].id == uid) {
                        break;
                    }
                    i++;
                }
                lcd.setCursor(0, 1);
                lcd.print(allvisitor[i].name);

                // Update visitor's last visit info
                RtcDateTime now = Rtc.GetDateTime();
                allvisitor[i].last_visit_day = now.Day();
                allvisitor[i].last_visit_month = now.Month();
                allvisitor[i].last_visit_year = now.Year();
                allvisitor[i].last_visit_minute = now.Minute();

                // Update visitor inside info
                numVisitorsInside++;
                allvisitor[i].number_of_visits++;
                visitorInside[j]=allvisitor[i].id;
                j++;

                delay(1500);  // Display message for 2 seconds
                lcd.clear();  // Clear the display
            }
        }
    }
    if(digitalRead(outing_sensor )==1){
numVisitorsInside-- ;
outing_servo.write(90) ;
delay(100);}


    }
/*    void test_outing(bool isElementInArray,( String uid,  String visitorInside, int numVisitorsInsideMax)){
      if(isElementInArray(uid, visitorInside, numVisitorsInsideMax)){
        lcd.setCursor(0, 0);
        lcd.print("Visitor is  ");
        lcd.setCursor(0, 1);
        lcd.print("inside");
        delay(1500);
        lcd.clear();
        return false;
      }
      
    }*/

