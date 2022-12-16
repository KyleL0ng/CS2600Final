#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// author: Kyle Long
// CS 2600 Final Project
// 16 December 2022
// Tic Tac Toe game in between 2 players (ESP32 and Laptop/Script). This code is for the ESP32, and communicates with the laptop through mqtt.

#define SDA 13  //Define SDA pins
#define SCL 14  //Define SCL pins

const char* ssid_Router = "Nighthawk";                           //input your wifi name
const char* password_Router = "AfghanistanBananaStand";          //input your wifi passwords
IPAddress server(192, 168, 254, 137);                            //IP of server
int runPin = 4;                                                  //run button pin
int xyzPins[] = { 32, 33, 2 };                                   //x,y,z pins
LiquidCrystal_I2C lcd(0x3F, 16, 2);                              //create lcd
char state[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };  //board
bool playing = false;                                            //sees whether game is in progress
WiFiClient wifi;                                                 //creates a client to be used for wifi
PubSubClient client(wifi);                                       //creates a client to be used for mqtt

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  //prints the message's topic
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  //prints each char of the message recieved
  } //end for
  Serial.println();
  int move = payload[1] & 0x0f;                                              //converts the message's second char to an int
  if (move >= 0 && move <= 8 && (payload[0] == 'X' || payload[0] == 'O')) {  //checks if the move is valid
    if (state[move] != 'X' && state[move] != 'O') {                          //checks if the move is not already made
      state[move] = payload[0];                                              //sets the correct space in the array to the correct letter
    } //end if
  } //end if
  lcd.setCursor(0, 1);                                                       //move the cursor to row 0, column 0
  //prints the board
  lcd.print(state[0]);
  lcd.print(state[1]);
  lcd.print(state[2]);
  lcd.print("...");
  lcd.print(state[3]);
  lcd.print(state[4]);
  lcd.print(state[5]);
  lcd.print("...");
  lcd.print(state[6]);
  lcd.print(state[7]);
  lcd.print(state[8]);
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("")) {  //if connected
      Serial.println("connected");
      client.subscribe("game/move");  //subscribe to the topic everything uses to send their moves
    } else {                          //if not connected
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    } //end if
  } //end while
} //end reconnect

void setup() {
  Serial.begin(115200);

  //MQTT setup
  client.setServer(server, 1883);  //sets the mqtt server
  client.setCallback(callback);    //sets which method should be called when something is recieved

  //Wifi setup
  Serial.setDebugOutput(true);
  Serial.println();
  WiFi.begin(ssid_Router, password_Router);  //connects to the network, with the correct password
  while (WiFi.status() != WL_CONNECTED) {    //keeps trying until connected
    delay(500);
    Serial.print(".");
  } //end while
  Serial.println("");
  Serial.println("WiFi connected");

  //Game setup
  pinMode(xyzPins[2], INPUT_PULLUP);  //setup for pickButton
  pinMode(runPin, INPUT_PULLUP);      //setup for runButton

  Wire.begin(SDA, SCL);           //attach the IIC pin
  lcd.init();                     //LCD driver initialization
  lcd.backlight();                //open the backlight
  lcd.setCursor(0, 0);            //move the cursor to row 0, column 0
  lcd.print("  Tic-Tac-Toe!  ");  //the print content is displayed on the LCD
  lcd.setCursor(0, 1);            //move the cursor to row 0, column 1
  lcd.print("Hit Run to begin");  //prompt the player to begin the game
} //end setup

void loop() {
  int sel = 0;                         //stores the joystick selection square
  int xVal = analogRead(xyzPins[0]);   //stores the x value of the joystick
  int yVal = analogRead(xyzPins[1]);   //stores the y value of the joystick
  int pick = digitalRead(xyzPins[2]);  //stores the value of the pick button
  int run = digitalRead(runPin);       //stores the value of the run button

  //gets the value (0-8) depending on where the joystick is being pushed (corresponds to game array)
  if (xVal < 1000) {
    if (yVal < 1000) {  //bottom left
      sel = 6;
    } else if (yVal > 3000) {  //bottom right
      sel = 8;
    } else {  //bottom middle
      sel = 7;
    } //end if
  } else if (xVal > 3000) {
    if (yVal < 1000) {  //top left
      sel = 0;
    } else if (yVal > 3000) {  //top right
      sel = 2;
    } else {  //top middle
      sel = 1;
    } //end if
  } else {
    if (yVal < 1000) {  //middle left
      sel = 3;
    } else if (yVal > 3000) {  //middle right
      sel = 5;
    } else {  //center (joystick at rest)
      sel = 4;
    } //end if
  } //end if

  //prints out the board if the game is being played
  if (playing) {
    lcd.setCursor(0, 0);  //move the cursor to row 0, column 0
    lcd.print("Selection: ");
    lcd.print(sel + 1);  //prints their selection (1-9 instead of 0-8)
    lcd.print("    ");

    lcd.setCursor(0, 1);  //move the cursor to row 1, column 0
    //prints the entire board
    lcd.print(state[0]);
    lcd.print(state[1]);
    lcd.print(state[2]);
    lcd.print("...");
    lcd.print(state[3]);
    lcd.print(state[4]);
    lcd.print(state[5]);
    lcd.print("...");
    lcd.print(state[6]);
    lcd.print(state[7]);
    lcd.print(state[8]);
  } //end if

  if (pick == 0) {                     //if the pick button is pressed
    char buf[3];                       //string to hold mqtt message
    buf[0] = 'X';                      //player 1 is always X
    itoa(sel, &buf[1], 10);            //converts the integer square selection to char
    client.publish("game/move", buf);  //publishes X and their selection
    delay(1000);
  } //end if

  if (run == 0) {                        //if the run button was pressed
    if (playing) {                       //and if the game is being played
      playing = false;                   //set playing to be false since the game is stopping
      client.publish("game/move", "Q");  //publish a Q to tell the laptop/script that we are done playing
      lcd.clear();                       //clear the lcd screen
      lcd.setCursor(0, 0);               //move the cursor to row 0, column 0
      lcd.print("  Tic-Tac-Toe!  ");
      lcd.setCursor(0, 1);  //move the cursor to row 1, column 0
      lcd.print("Hit Run to begin");
    } else {                             //if the game is not being played
      playing = true;                    //set playing to true since the game is starting
      lcd.clear();                       //clear the lcd screen
      client.publish("game/move", "S");  //publish an S to let the laptop/daemon know the game is beginning
    } //end if
    delay(1000);
  } //end if


  //mqtt
  if (!client.connected()) {  //if not connected to mqtt
    reconnect();
  } //end if
  client.loop();  //make sure mqtt has time to check if messages come in
} //end loop
