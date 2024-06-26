#include <OneWire.h>
#include <LiquidCrystal.h>

// Initialize OneWire and LCD objects
OneWire ds(10); // OneWire communication on pin 10
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD pins: rs, en, d4, d5, d6, d7

// Define button pins
const int up = 9;
const int down = 8;

// Initialize target temperature
int temp = 32;

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    
    // Initialize LCD
    lcd.begin(16, 2);
    lcd.cursor();
    lcd.print("Hello World :)");
  
    // Set pin modes
    pinMode(7, OUTPUT); // Control pin for external device
    pinMode(up, INPUT); // Up button pin
    pinMode(down, INPUT); // Down button pin
    
    // Delay for sensor stabilization
    delay(2500);
}

void loop() {
    // Variables to hold sensor data
    byte i;
    byte present = 0;
    byte data[12];
    byte addr[8];
    float celsius;
  
    // Search for devices on the OneWire bus
    if (!ds.search(addr)) {
        ds.reset_search();
        delay(50);
        return;
    }
  
    // Verify device address
    if (OneWire::crc8(addr, 7) != addr[7]) {
        return;
    }

    // Request temperature conversion
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1); // Start conversion
  
    delay(750); // Wait for conversion to complete
  
    // Read temperature data
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE); // Read Scratchpad

    for (i = 0; i < 9; i++) {
        data[i] = ds.read();
    }

    // Convert raw data to temperature
    int16_t raw = (data[1] << 8) | data[0];
    byte cfg = (data[4] & 0x60);

    if (cfg == 0x00) raw = raw & ~7; // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

    celsius = (float)raw / 16.0;

    // Print temperature to Serial monitor
    Serial.print("Temperature = ");
    Serial.println(celsius);

    // Display temperature on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature: ");
    lcd.print(celsius);
    lcd.print(" C");

    // Control external device based on temperature
    if (celsius > temp) {
        digitalWrite(7, LOW); // Turn off the device
    } else {
        digitalWrite(7, HIGH); // Turn on the device
    }

    // Adjust target temperature using buttons
    if (digitalRead(up) == HIGH) {
        temp++;
    }

    if (digitalRead(down) == HIGH) {
        temp--;
    }

    // Delay before next iteration
    delay(1000);
}