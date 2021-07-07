#include <MHZ19.h>  // library can be found at: https://github.com/strange-v/MHZ19
#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel

// Pin to the LED
#define pixelpin 8
// Number of LEDs on the strip
#define nr_of_pixels 1

// Communication w/ CO2 sensor
#define serial_rx 7
#define serial_tx 6

// Buzzer
#define spiezopin 5
#define piezopin 4


Adafruit_NeoPixel pixel(nr_of_pixels, pixelpin, NEO_GRB + NEO_KHZ800);

MHZ19 mhz(&Serial1);
void setup() {
  // Init LED
  pixel.begin();
  pixel.clear();
  pixel.setBrightness(225);

  // Set color
  pixel.fill(pixel.Color(80, 80, 80)); pixel.show();

  // Init serial
  Serial.begin(115200);
  Serial1.begin(9600);

  // Chirp once after setup
  chirp(HIGH);
}

void loop() {
  for (int t = 0; t < random(30, 300); t++) {

    // Read from the CO2 sensor
    MHZ19_RESULT response = mhz.retrieveData();

    // Keep trying until we get a valid response
    while (response != MHZ19_RESULT_OK) {
      Serial.print("Response NOK. Retrying. ");
    Serial.print("got: ");
    if (response == MHZ19_RESULT_ERR_CRC) {
      Serial.println("err crc");
    } else if (response == MHZ19_RESULT_ERR_TIMEOUT) {
      Serial.println("err timeout");
    } else {
      Serial.println(response);
    }
    
      pixel.fill(pixel.Color(0, 0, 255)); pixel.show();
      delay(100);
      response = mhz.retrieveData();
    }

    // Reading successful. Get the CO2 value
    int co2 = mhz.getCO2();
    Serial.print("CO2: ");
    Serial.print(co2);


    // Map value to RGB color
    int G = map(co2, 200, 1400, 0, 255);
    int R = map(co2, 200, 1400, 255, 0);
    G = constrain(G, 0, 255);
    R = constrain(R, 0, 255);

    // Set the LED with the color generated
    pixel.fill(pixel.Color(R, G, 0)); pixel.show();


    if (co2 > 1400) {
      // Big alarm
      Serial.println(F(" That's horrible!"));
      loud_mode();
      for (int l = 0; l < 15; l++) {
        lowChirp(random(5, 10), 1);
        if (random(0, 5) == 0) {
          tweet(random(2, 12), 2);
        }
      }
    } else if (co2 > 850) {
      // Small alarm
      Serial.println(F(" That's not so good!"));
      silent_mode();
      for (int l = 0; l < 10; l++) {
        highChirp(random(5, 10), 1);
        if (random(0, 5) == 0) {
          tweet(random(2, 12), 2);
        }
      }
    } else {
      Serial.println(F(" That is ok!"));
      // Wait a second before a new reading
      delay(1000);
    }
  }
  chirp(LOW);
}

void chirp(boolean volume) {
  if (volume) {
    loud_mode();
  } else {
    silent_mode();
  }
  Serial.println("chirp");
  for (int i = 0; i < random(1, 3); i++ ) {
    highChirp(5, random(1, 5)); //intensity, amount of chirps
    delay(100);
    lowChirp(random(40, 200), 2); //intensity, amount of chirps
    delay(100);
    tweet(random(2, 12), 2); //intensity, amount of tweets
  }
}

void highChirp(int intensity, int chirpsNumber) {
  for (int veces = 0; veces <= chirpsNumber; veces++) {
    for (int i = 100; i > 0; i--) {
      for  (int x = 0; x < intensity;  x++) {
        digitalWrite (piezopin, HIGH);
        digitalWrite (spiezopin, HIGH);
        delayMicroseconds (i);
        digitalWrite (piezopin, LOW);
        digitalWrite (spiezopin, LOW);
        delayMicroseconds (i);
      }
    }
  }
}

void lowChirp(int intensity, int chirpsNumber) {
  for (int veces = 0; veces <= chirpsNumber; veces++) {
    for (int i = 0; i < 200; i++) {
      digitalWrite (piezopin, HIGH);
      digitalWrite (spiezopin, HIGH);
      delayMicroseconds (i);
      digitalWrite (piezopin, LOW);
      digitalWrite (spiezopin, LOW);
      delayMicroseconds (i);
    }
    for (int i = 90; i > 80; i--) {
      for  (int x = 0; x < 5;  x++) {
        digitalWrite (piezopin, HIGH);
        digitalWrite (spiezopin, HIGH);
        delayMicroseconds (i);
        digitalWrite (piezopin, LOW);
        digitalWrite (spiezopin, LOW);
        delayMicroseconds (i);
      }
    }
  }
}

void tweet(int intensity, int chirpsNumber) {
  //normal chirpsNumber 3, normal intensity 5
  for (int veces = 0; veces < chirpsNumber; veces++) {
    for (int i = 80; i > 0; i--) {
      for  (int x = 0; x < intensity;  x++) {
        digitalWrite (piezopin, HIGH);
        digitalWrite (spiezopin, HIGH);
        delayMicroseconds (i);
        digitalWrite (piezopin, LOW);
        digitalWrite (spiezopin, LOW);
        delayMicroseconds (i);
      }
    }
  }
  delay(1000);
}

void loud_mode() {
  pinMode(piezopin, OUTPUT);
  pinMode(spiezopin, INPUT);
}

void silent_mode() {
  pinMode(piezopin, INPUT);
  pinMode(spiezopin, OUTPUT);
}
