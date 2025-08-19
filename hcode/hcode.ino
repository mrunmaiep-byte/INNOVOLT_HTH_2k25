#include &lt;Wire.h&gt;
#include &lt;Adafruit_MPU6050.h&gt;
#include &lt;Adafruit_Sensor.h&gt;
#include &lt;HardwareSerial.h&gt;
Adafruit_MPU6050 mpu;
HardwareSerial sim900(1); // UART1 for SIM900A
//  PIN DEFINITIONS 
#define BUTTON_PIN 4 // Cancel/SOS button
#define BUZZER_PIN 15 // Buzzer 
#define LED_R 25
#define LED_G 26
#define LED_B 27
// GLOBALS 
unsigned long fallTime = 0;
bool fallDetected = false;
bool alertSent = false;
unsigned long buttonPressStart = 0;
bool buttonHeld = false;
// Phonebook for escalation
String contacts[] = {&quot;+919834806454&quot;, &quot;+917499380316&quot;};
int contactIndex = 0;
//  FUNCTION DECLARATIONS 
void sendSMS(String number, String message);
void checkIncomingSMS();
void setLED(int r, int g, int b);
//  SETUP 
void setup() {
Serial.begin(115200);
// IO setup
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(BUZZER_PIN, OUTPUT);
pinMode(LED_R, OUTPUT);
pinMode(LED_G, OUTPUT);
pinMode(LED_B, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);
setLED(0,0,0);

// MPU6050 init
Wire.begin(21, 22);
if (!mpu.begin()) {
Serial.println(&quot;Failed to find MPU6050&quot;);
while (1);
}
mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
mpu.setGyroRange(MPU6050_RANGE_500_DEG);
mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
// SIM900A init
sim900.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
delay(1000);
sim900.println(&quot;AT&quot;);
delay(500);
sim900.println(&quot;AT+CMGF=1&quot;);
delay(500);
sim900.println(&quot;AT+CNMI=1,2,0,0,0&quot;); // New SMS → UART
delay(500);
Serial.println(&quot;System ready.&quot;);
setLED(0,1,0); // Green
}
//  LOOP 
void loop() {
sensors_event_t a, g, temp;
mpu.getEvent(&amp;a, &amp;g, &amp;temp);
float magnitude = sqrt(a.acceleration.x * a.acceleration.x +
a.acceleration.y * a.acceleration.y +
a.acceleration.z * a.acceleration.z);
// Detect fall
if (!fallDetected &amp;&amp; magnitude &gt; 25) {
fallDetected = true;
fallTime = millis();
Serial.println(&quot;Fall detected!&quot;);
setLED(1,1,0); // Yellow
digitalWrite(BUZZER_PIN, HIGH);
}
// Confirm fall
if (fallDetected &amp;&amp; !alertSent) {
if (millis() - fallTime &gt; 5000) {
sendSMS(contacts[contactIndex], &quot;ALERT: Fall detected! Reply OK to
confirm.&quot;);
alertSent = true;
digitalWrite(BUZZER_PIN, LOW);
setLED(1,0,0); // Red
}
}
// Button logic
if (digitalRead(BUTTON_PIN) == LOW) {
if (buttonPressStart == 0) buttonPressStart = millis();

if (!buttonHeld &amp;&amp; (millis() - buttonPressStart &gt; 2000)) {
buttonHeld = true;
Serial.println(&quot;SOS Triggered&quot;);
sendSMS(contacts[contactIndex], &quot;SOS: Manual alert triggered!&quot;);
setLED(1,0,0);
}
} else {
if (buttonPressStart &gt; 0 &amp;&amp; !buttonHeld &amp;&amp; (millis() - buttonPressStart &lt;
2000)) {
Serial.println(&quot;Alert cancelled.&quot;);
fallDetected = false;
alertSent = false;
digitalWrite(BUZZER_PIN, LOW);
setLED(0,0,1); // Blue
delay(2000);
setLED(0,1,0); // Green
}
buttonPressStart = 0;
buttonHeld = false;
}
// Incoming SMS
checkIncomingSMS();
}
//  FUNCTIONS 
void sendSMS(String number, String message) {
sim900.println(&quot;AT+CMGF=1&quot;);
delay(500);
sim900.print(&quot;AT+CMGS=\&quot;&quot;);
sim900.print(number);
sim900.println(&quot;\&quot;&quot;);
delay(500);
sim900.print(message);
delay(500);
sim900.write(26);
delay(2000);
}
void checkIncomingSMS() {
if (sim900.available()) {
String sms = sim900.readString();
sms.toUpperCase();
if (sms.indexOf(&quot;OK&quot;) &gt;= 0) {
Serial.println(&quot;Caregiver acknowledged. Resetting alert.&quot;);
fallDetected = false;
alertSent = false;
setLED(0,1,0); // Green
}
else if (alertSent &amp;&amp; (millis() - fallTime &gt; 60000)) {
contactIndex = (contactIndex + 1) %
(sizeof(contacts)/sizeof(contacts[0]));
sendSMS(contacts[contactIndex], &quot;ESCALATION: Fall detected. Previous
contact did not reply.&quot;);
fallTime = millis();

setLED(1,0,0);
}
}
}
void setLED(int r, int g, int b) {
digitalWrite(LED_R, r ? HIGH : LOW);
digitalWrite(LED_G, g ? HIGH : LOW);
digitalWrite(LED_B, b ? HIGH : LOW);
}