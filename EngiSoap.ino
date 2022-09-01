/*  EngiSoap-ohjelmistokoodi 2.periodin sovellusprojektiin 2022.  
 *  Saippua-annostelijaan kasattu ja liitetty ohjelmistokoodi, jossa havaitaan anturilla liike, aktivoidaan servomoottori ja kommunikoidaan tapahtumat käyttäjälle hyödyntäen LCD-näyttöä. 
 *  Vuorovaikutteisuutta käyttäjältä lisää saippuapullon täydennys käyttäen painonappia. 
 *  
 *  Koodatut osat:
 *  -Anturi: Sampo Ikonen
 *  -Servo/nappi: Tuomas Jokikokko
 *  -LCD-näyttö: Sampo Sarlin
 *  -Ohjelmistokoodin kasaus: Samuli Järvensivu
 */

//Ladataan tarvittavat kirjastot.
#include <LiquidCrystal.h> //LCD-näyttö
#include <Servo.h> //Servo

/*  Vaihtoehtoinen anturin kirjasto. Tässä sitä ei ole käytetty.
 *  #include <ST_HW_HC_SR04.h> 
 */

// Luodaan muuttujat ja oliot.
Servo servoOlio; // Luodaan servo-olio. 
const int rs = 30, en = 31, d4 = 37, d5 = 33, d6 = 34, d7 = 35; //Tehdään muuttujat LCD-näytölle
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Annetaan lcd-oliolle luodut muuttujat
int trigPin = 53;    //Trigger pinni anturille.
int echoPin = 52;    //Echo pinni anturille.
long etaisyys, cm;  
bool painettu = true;
int annoksia = 2;
const char nappi = 8; //Pinni 8 Arduinolla täyttönapille.
int pos = 0; //Servon sijainti asteina 0 - 180.

void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(nappi, INPUT); 
    servoOlio.write(180); //Laitetaan servo 180 asteeseen.
    lcd.begin(16,2);  
    servoOlio.attach(51); //Servo saa pinnin 51 Arduinolta.
    while(!Serial); 
}

//Jatkuva toisto luoduille funktioille.
void loop() {
  alkuNakyma(annoksia); //LCD-näytölle ilmoitus jäljellä olevasta annosmäärästä ja toimintavalmiudesta.
  nappi(); //Pullon täydennys.
  anturi(); //Anturi valmiuteen liikettä varten.
  liikutaServoa(); //Kun liike havaittu servo liikkuu.
  delay(1000);
}

//LCD-näytön toiminta loopin alussa.
int alkuNakyma(int annoksia) {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("VALMIINA");
  lcd.setCursor(0,1);
  lcd.print("Annoksia:");
  lcd.print(annoksia);
  return annoksia;
}
//Anturi lähettämään pulsseja liikettä varten.
void anturi() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  etaisyys = pulseIn(echoPin, HIGH);
  cm = (etaisyys/2) / 29.1;  //Kaava senttien laskemiselle pulssien osumisajasta.
  Serial.print(cm); //Tulostetaan näytölle etäisyys ensimmäiseen pulssien esteeseen.
  Serial.println("cm");  
}
//Servon toiminta liikkeeseen.
void liikutaServoa() {
  if (cm < 10 ) { //Jos liikettä havaitaan alle 10cm. etäisyydellä.  
    if (annoksia == 0)  { //Saippua loppu.
      lcd.clear();
      lcd.print("Tyhja!");
      lcd.setCursor(0,1);
      lcd.print("Tayta pullo.");
      delay(1000);
    } else { 
      lcd.clear();
      lcd.print("Saippuaa");
      lcd.setCursor(0,1);
      lcd.print("tulossa...");
      delay(3000); // 3s viive jotta keretään laittamaan käsi alle.
        for (pos = 180; pos >= 0; pos -= 1) { // Laitetaan servo menemään 180 -> 0 asteeseen.
          servoOlio.write(pos);               // Servo menee muuttujan pos asteeseen.
          delay(5);                           // Odotusaika servolle jokaisen asteen välillä. Servon nopeus säädetään näillä.
        }
        for (pos = 0; pos <= 180; pos += 1) { // Laitetaan servo menemään 0 -> 180 asteeseen.
          servoOlio.write(pos);               // Servo menee muuttujan pos asteeseen.
          delay(5);                           // Odotusaika servolle jokaisen asteen välillä. Servon nopeus säädetään näillä.
        }
        annoksia = annoksia - 1; //Joka annostelun jälkeen -1. 
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print("Kiitos");
        delay(3000);
      }
  }
  
}
//Pullon täyttö.
void nappi() {
  bool current = digitalRead(nappi);
  if (current == painettu) { //Jos nappia painetaan täytetään saippuapullo.
    annoksia = 2; //2 = pullo täynnä.
    lcd.clear();
    lcd.print("Pullo taytetty");
  }
}
