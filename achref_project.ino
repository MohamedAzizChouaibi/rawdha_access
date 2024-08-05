#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10   // Slave Select pin
#define RST_PIN 9   // Reset pin

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

class Visitor {
  public:
    String name;
    int age;
    String id;
    int number_of_visits = 0;
    String last_visit = "";

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

const int num_total = 20;
Visitor myArr[num_total];
int numVisitors = 499;

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
  }
    if (numVisitors>500){
        lcd.setCursor(0, 0);
        lcd.print("there is no ");
        lcd.setCursor(0,1);
        lcd.print("place inside");
        return false;

    }

  if (i >= num_total) {
    lcd.setCursor(0, 0);
    lcd.print("Visitor not ");
    lcd.setCursor(0,1);
    lcd.print("registered");
    return false;
  }

  if (myArr[i].number_of_visits >= 3) {
    lcd.setCursor(0, 0);
    lcd.print("Visitor had ");
    lcd.setCursor(0,1);
    lcd.print("3 visits");
    return false;
  }

  // You can uncomment and adjust this if you want to use last_visit logic
  // if (myArr[i].last_visit > 3) {
  //   lcd.setCursor(0, 0);
  //   lcd.print("Visitor end his 3 visits");
  //   return false;
  // }

  return true;
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("New card detected");

    if (mfrc522.PICC_ReadCardSerial()) {
      String uid = getCardUID();
      Serial.print("UID: ");
      Serial.println(uid);

      if (test(uid)) {
        lcd.setCursor(0, 0);
        lcd.print("You can enter");
        lcd.setCursor(0, 1);
        lcd.print(uid);
        numVisitors++;

        int i = 0;
        while (i < num_total) {
          if (myArr[i].id == uid) {
            break;
          }
          i++;
        }
        if (i < num_total) { // Ensure index is within bounds
          myArr[i].number_of_visits++;
        }
      } else {
 
      }

      delay(2000);  // Display message for 2 seconds
      lcd.clear();  // Clear the display
    } else {
      Serial.println("Failed to read card serial");
    }
  } else {
    Serial.println("No new card detected");
  }

  delay(1000);  // Delay to avoid constant reading
}

String getCardUID() {
  String uidString = "";  // Variable to hold the UID

  // Print debug information
  Serial.print("Card UID Size: ");
  Serial.println(mfrc522.uid.size);

  // Convert UID to a string
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0";  // Add leading zero for bytes less than 0x10
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);  // Append byte in HEX format
  }

  // Convert the UID to uppercase
  uidString.toUpperCase();

  // Print UID for debugging
  Serial.print("UID String: ");
  Serial.println(uidString);

  // Halt PICC communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();  // Ensure encryption is stopped

  return uidString;  // Return the UID
}
