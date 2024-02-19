#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <SPI.h>
#include <MFRC522.h>

#define DHTPIN 4       // DHT11 data pin connected to Arduino digital pin 4
#define DHTTYPE DHT11  // DHT type

#define RST_PIN 9    // Define the RST_PIN (if not connected to Arduino, set to -1)
#define SS_PIN 10    // Define the SS_PIN (MFRC522's SDA pin connected to Arduino digital pin 10)

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display

SoftwareSerial bluetoothSerial(2, 3);  // TX(2), RX(3) pins for Bluetooth module

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance

volatile bool flag = false;

float temperature = 0;
float humidity = 0;

void setup() {
  Serial.begin(9600);  // Serial monitor for debugging
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the backlight
  dht.begin();         // Initialize DHT sensor
  SPI.begin();         // Init SPI bus

  rfid.PCD_Init();     // Init MFRC522

  bluetoothSerial.begin(9600);  // Start SoftwareSerial for Bluetooth communication
  Serial.println("Bluetooth Terminal - Temperature & Humidity");
  bluetoothSerial.println("Bluetooth Terminal - Temperature & Humidity");

  Timer1.initialize(2000000); // 2 seconds interval
  Timer1.attachInterrupt(timerIsr);
}

void loop() {
  if (flag) {
    humidity = dht.readHumidity();        // Read humidity
    temperature = dht.readTemperature();  // Read temperature in Celsius

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    lcd.clear();          // Clear the LCD screen
    lcd.setCursor(0, 0);  // Set cursor to the first column and first row
    lcd.print("Scan RFID to get");
    lcd.setCursor(0, 1);
    lcd.print("weather data");

    flag = false; // Reset the flag
  }

  // Check for RFID scan
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("RFID card detected!");

    // Place your logic here to identify the card and trigger weather display
    // For demonstration purposes, let's assume the card triggers weather display
    displayWeather();
    delay(2000); // Delay to avoid repeated readings
  }

  // Check if data is available from Bluetooth
  if (bluetoothSerial.available() > 0) {
    char receivedChar = bluetoothSerial.read();
    if (receivedChar == '1') {
      // Send temperature and humidity to Bluetooth
      bluetoothSerial.print("Temperature: ");
      bluetoothSerial.print(temperature);
      bluetoothSerial.print(" C, Humidity: ");
      bluetoothSerial.print(humidity);
      bluetoothSerial.println(" %");
    }
  }
}

void timerIsr() {
  flag = true; // Set the flag in the interrupt
}

void displayWeather() {
  // Your logic to display weather information on the LCD here
  // This function is called when an RFID card is detected
  // For example:
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
  lcd.print(humidity);
  lcd.print("%");
}
