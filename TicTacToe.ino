#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SDA 13  //Define SDA pins
#define SCL 14  //Define SCL pins

const char* ssid_Router = "Nighthawk";                   //input your wifi name
const char* password_Router = "AfghanistanBananaStand";  //input your wifi passwords
IPAddress server(192, 168, 254, 137);                    //IP of server
int runPin = 4;                                          //run button pin
int xyzPins[] = { 32, 33, 2 };                           //x,y,z pins
LiquidCrystal_I2C lcd(0x3F, 16, 2);
char state[] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };  //board
bool updateBoard = false;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  int move = atoi((char*)payload);
  if (move >= 0 && move <= 8) {
    state[move] = 'O';
    updateBoard = true;
  }
  lcd.setCursor(0, 1);
  lcd.print(state[0]);
  lcd.print(state[1]);
  lcd.print(state[2]);
  lcd.print("   ");
  lcd.print(state[3]);
  lcd.print(state[4]);
  lcd.print(state[5]);
  lcd.print("   ");
  lcd.print(state[6]);
  lcd.print(state[7]);
  lcd.print(state[8]);
}

WiFiClient wifi;
PubSubClient client(wifi);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("game/move");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  //mqtt
  client.setServer(server, 1883);
  client.setCallback(callback);


  //wifi
  Serial.setDebugOutput(true);
  Serial.println();
  WiFi.begin(ssid_Router, password_Router);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  pinMode(xyzPins[2], INPUT_PULLUP);  //z axis is a button.
  pinMode(runPin, INPUT_PULLUP);

  Wire.begin(SDA, SCL);           // attach the IIC pin
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  lcd.setCursor(0, 0);            // Move the cursor to row 0, column 0
  lcd.print("  Tic-Tac-Toe!  ");  // The print content is displayed on the LCD
  lcd.setCursor(0, 1);
  lcd.print("Hit Run to begin");
  while (digitalRead(runPin))
    ;
  lcd.clear();
}

void loop() {
  int sel = 0;
  int xVal = analogRead(xyzPins[0]);
  int yVal = analogRead(xyzPins[1]);
  int zVal = digitalRead(xyzPins[2]);

  if (xVal < 1000) {
    if (yVal < 1000) {
      sel = 6;
    } else if (yVal > 3000) {
      sel = 8;
    } else {
      sel = 7;
    }  //end if
  } else if (xVal > 3000) {
    if (yVal < 1000) {
      sel = 0;
    } else if (yVal > 3000) {
      sel = 2;
    } else {
      sel = 1;
    }  //end if
  } else {
    if (yVal < 1000) {
      sel = 3;
    } else if (yVal > 3000) {
      sel = 5;
    } else {
      sel = 4;
    }  //end if
  }    //end if

  lcd.setCursor(0, 0);
  lcd.print("Selection: ");
  lcd.print(sel + 1);
  lcd.print("    ");

  if (zVal == 0) {
    char buf[2];
    itoa(sel, buf, 10);
    client.publish("game/move", buf);
  }

  if (updateBoard) {
    state[sel] = 'X';

    delay(500);
    updateBoard = false;
  }  //end if

  //mqtt
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
