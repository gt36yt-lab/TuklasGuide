// This is the code for your ESP32.
// It will connect to your WiFi and create a webpage to control the built-in LED.

// =================================================================
// SECTION 1: LIBRARIES AND GLOBAL VARIABLES
// =================================================================

// The '#include' directive is like telling the ESP32 to grab a toolbox.
// This specific toolbox, 'WiFi.h', contains all the commands for using WiFi.
#include <WiFi.h>

// --- IMPORTANT: CHANGE THESE! ---
// Here, we store the WiFi network's name (SSID) and password.
// 'const char*' means these are constant text strings that won't change while the program runs.
const char* ssid = "bleep";
const char* password = "samoksamok";
// --------------------------------

// We create a 'WiFiServer' object named 'server'. This object will listen for
// incoming connections from web browsers on port 80, which is the standard port for HTTP (web traffic).
WiFiServer server(80);

// This integer variable, 'ledState', will act as our memory.
// It keeps track of whether the LED is currently ON (1) or OFF (0).
int ledState = 0;

// This constant integer, 'ledPin', stores the GPIO number of the built-in LED.
// On most ESP32 boards, this is pin 2. Using a named variable makes the code easier to read.
const int ledPin = 2; // GPIO2

// =================================================================
// SECTION 2: THE SETUP() FUNCTION
// This code runs only ONCE when the ESP32 first powers on or is reset.
// =================================================================

void setup() {
  // 'Serial.begin(115200)' starts the serial communication. This allows the ESP32
  // to send messages back to your computer through the USB cable. It's our "message window" for debugging.
  Serial.begin(115200);

  // 'pinMode(ledPin, OUTPUT)' configures the LED's GPIO pin as an OUTPUT.
  // This means the ESP32 can send an electrical signal OUT to the LED to turn it on.
  pinMode(ledPin, OUTPUT);
  // 'digitalWrite(ledPin, LOW)' sets the initial state of the LED to OFF (LOW signal).
  digitalWrite(ledPin, LOW);

  // --- Connecting to WiFi (with improvements!) ---
  Serial.println(); // Prints a blank line for neatness.
  Serial.print("Connecting to "); // Prints a message to the Serial Monitor.
  Serial.println(ssid); // Prints the WiFi name you provided.

  // 'WiFi.mode(WIFI_STA)' explicitly tells the ESP32 to act as a "Station".
  // This means it will be a client connecting to an existing WiFi router.
  WiFi.mode(WIFI_STA);
  // 'WiFi.begin(ssid, password)' tells the ESP32 to start trying to connect
  // to the network using the credentials you provided. This process happens in the background.
  WiFi.begin(ssid, password);

  // 'millis()' is a built-in clock that counts milliseconds since the ESP32 started.
  // We store the current time here to use for our connection timeout.
  unsigned long startAttemptTime = millis();

  // This 'while' loop is our waiting room. It will continue to loop as long as
  // the ESP32's WiFi status is NOT equal to 'WL_CONNECTED'.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait for half a second.
    Serial.print("."); // Print a dot to show that we are still trying.
    
    // This 'if' statement is our timeout safety net. It checks if more than
    // 15,000 milliseconds (15 seconds) have passed since we started trying.
    if (millis() - startAttemptTime > 15000) {
      Serial.println("\nConnection Failed! Check your WiFi credentials or network.");
      // If it fails, 'return' stops the setup() function. The ESP32 will do nothing more.
      return; 
    }
  }

  // If the code reaches this point, the 'while' loop has ended, which means we are connected!
  Serial.println(""); // Print a blank line.
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  // 'WiFi.localIP()' asks the ESP32 for its IP address on the network. This is crucial!
  // It's the address you will type into your phone's browser.
  Serial.println(WiFi.localIP());

  // 'server.begin()' officially starts the web server we created earlier.
  // Now, it's actively listening for incoming browser connections.
  server.begin();
  Serial.println("Web server started. Open the IP address in a browser.");
}

// =================================================================
// SECTION 3: THE LOOP() FUNCTION
// This code runs over and over again, forever, after setup() is complete.
// =================================================================

void loop() {
  // It's good practice to check if the WiFi is still connected at the start of the loop.
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi Disconnected. Please reset the ESP32.");
    // This simple 'while(true)' loop will trap the ESP32 here if it disconnects,
    // preventing it from trying to run web server code without a connection.
    while(true){ delay(1000); }
  }

  // 'server.available()' checks if a new client (a web browser) has connected.
  // If so, it returns a 'client' object that we can use to communicate.
  WiFiClient client = server.available();
  if (!client) {
    return; // If there is no client, 'return' ends this run of the loop() and starts it again.
  }

  // This loop waits for the browser to send its request data.
  while (!client.available()) {
    delay(1);
  }

  // 'client.readStringUntil('\r')' reads the first line of the browser's request.
  // This line contains the command, e.g., "GET /LED=ON HTTP/1.1".
  String request = client.readStringUntil('\r');
  client.flush(); // Clears out any remaining data from the client.

  // --- Check the Request and Control the LED ---
  // 'request.indexOf("/LED=ON")' searches the request string for the substring "/LED=ON".
  // If it finds it, it returns the position; otherwise, it returns -1.
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH); // Send a HIGH signal to turn the LED ON.
    ledState = 1; // Update our memory variable to 1 (ON).
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW); // Send a LOW signal to turn the LED OFF.
    ledState = 0; // Update our memory variable to 0 (OFF).
  }

  // --- Send the Webpage to the Browser ---
  // Now, we send back a response. This is plain text formatted in HTML.
  // The first lines are the HTTP header, telling the browser what kind of content to expect.
  client.println("HTTP/1.1 200 OK"); // Status: 200 OK means everything is good.
  client.println("Content-Type: text/html"); // We are sending an HTML page.
  client.println(""); // A mandatory blank line separates the header from the content.

  // This is the actual HTML content for our webpage.
  client.println("<!DOCTYPE html><html><head><title>ESP32 LED Control</title>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>"); // Helps with mobile screen scaling.
  // This is inline CSS for styling the page and buttons.
  client.println("<style>html {font-family: Arial; display: inline-block; text-align: center;}");
  client.println("h1 {font-size: 2rem;} .button { background-color: #4CAF50; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 24px; margin: 2px; cursor: pointer;}");
  client.println(".button-off {background-color: #f44336;}</style></head>");
  client.println("<body><h1>ESP32 Web Server</h1>");

  // Here, we check our 'ledState' memory variable to display the correct status on the page.
  if (ledState == 1) {
    client.println("<p>LED is ON</p>");
  } else {
    client.println("<p>LED is OFF</p>");
  }

  // These are the control buttons. They are hyperlinks ('<a>' tags).
  // Clicking "Turn ON" sends the ESP32 to its own IP address with "/LED=ON" appended to it.
  // Our code above looks for this in the request.
  client.println("<a href='/LED=ON' class='button'>Turn ON</a>");
  client.println("<a href='/LED=OFF' class='button button-off'>Turn OFF</a>");
  client.println("</body></html>");

  delay(1); // A small delay to ensure the client has time to receive the data.
}

