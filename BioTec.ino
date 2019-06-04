#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>

// defino pin del sensor DHT y tipo
#define DHTPIN 2     // Pin del DHT
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// defino display lcd 16x2
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//defino luxometro
BH1750 lightMeter(0x23);

byte term[8] = //icon for termometer
{
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B10001,
    B10001,
    B01110
};

byte drop[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10011,
    B10011,
    B01110,
};

byte st[8] = //icon for st
{
    B11100,
    B10000,
    B11100,
    B00100,
    B11111,
    B00010,
    B00010,
    B00010,
};
 
void setup()
{
  Wire.begin(); 
  Serial.begin(9600);  
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);   // inicializo luxometro
  dht.begin();                                        // inicializo DHT
  lcd.init();                                         // inicializo lcd 
  lcd.backlight();
  lcd.clear();
  lcd.createChar (0,term);
  lcd.createChar (1,drop);
  lcd.createChar (2,st);
  Serial.println("\nBioTec");
}
 
 
void loop()
{  
  delay(2000);
  
  // lectura de humedad relativa %
  float h = dht.readHumidity();
  // lectura de temperatura en grados Celsius
  float t = dht.readTemperature();

  // control de datos
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }  
  // calculo de la sensacion termica en grados Celsius  
  float hic = dht.computeHeatIndex(t, h, false);

  // luxometro
  int lux = (int) lightMeter.readLightLevel(true); 
  Serial.println(lux);
  Serial.println(getDigits(lux));

  if (lux < 0) {
    Serial.println(F("Error condition detected"));
  }
  else {
    if (lux > 40000.0) {
      // reduce measurement time - needed in direct sun light
      if (lightMeter.setMTreg(32)) {
        //Serial.println(F("Setting MTReg to low value for high light environment"));
      }
      else {
        //Serial.println(F("Error setting MTReg to low value for high light environment"));
      }
    }
    else {
        if (lux > 10.0) {
          // typical light environment
          if (lightMeter.setMTreg(69)) {
            //Serial.println(F("Setting MTReg to default value for normal light environment"));
          }
          else {
            //Serial.println(F("Error setting MTReg to default value for normal light environment"));
          }
        }
        else {
          if (lux <= 10.0) {
            //very low light environment
            if (lightMeter.setMTreg(138)) {
              //Serial.println(F("Setting MTReg to high value for low light environment"));
            }
            else {
              //Serial.println(F("Error setting MTReg to high value for low light environment"));
            }
          }
       }
    }

  }
  // display data
  lcd.setCursor(1,0);
  // temperatura display
  lcd.write(byte (0));
  lcd.print(t, 1);
  lcd.write(223);
  // humedad display
  lcd.setCursor(9,0);
  lcd.write(byte (1));
  lcd.print(h, 1);
  lcd.print("%");
  lcd.setCursor(0,1);
  // sensacion termica display
  //lcd.print("ST:");
  lcd.write(byte (2));
  lcd.print(hic, 1);
  lcd.write(223);

  int digit = 13 - getDigits(lux);
  if (digit < 7) {
    digit = 6;
  }

  lcd.setCursor(6,1);
  lcd.print("        ");
  lcd.setCursor(digit,1);
  lcd.print(lux);
  lcd.setCursor(14,1);
  lcd.print("Lx");

  // serial
  Serial.print(t);
  Serial.println(" ºC");
  
}

int getDigits(int num){
  int digits = 1;
  int dividend = num / 10;
  while(dividend > 0) {
    dividend = dividend / 10;
    digits++;
  }
  return digits;
}