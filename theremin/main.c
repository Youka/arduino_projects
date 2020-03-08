// Digital pins used
enum DigitalPins{
  SENSOR = 2,
  BUZZER
};
// Update time
#define SAMPLE_DURATION_MS 20
// Frequency limits
#define MIN_FREQ_HZ 0
#define MAX_FREQ_HZ 1000
// Sensor signal response limits
#define SOUND_US_PER_CM 29.387f
#define MIN_SENSOR_CM 20
#define MAX_SENSOR_CM 200

// Initialize device
void setup(){
  // Prepare buzzer for output
  pinMode(BUZZER, OUTPUT);
}
// Device running after initialization
void loop(){
  // Send out signal with sensor
  pinMode(SENSOR, OUTPUT);
  digitalWrite(SENSOR, HIGH);
  delayMicroseconds(5);	// Let the sensor some time to send out a strong signal
  digitalWrite(SENSOR, LOW);
  // Read reflected signal
  pinMode(SENSOR, INPUT);
  const unsigned long response_us = pulseIn(SENSOR, HIGH) >> 1;	// Want just the incoming, not outgoing duration
  // Convert time to distance to frequency
  const unsigned int freq = map(constrain(response_us / SOUND_US_PER_CM, MIN_SENSOR_CM, MAX_SENSOR_CM), MIN_SENSOR_CM, MAX_SENSOR_CM, MAX_FREQ_HZ, MIN_FREQ_HZ);
  // Tone on with frequency for short duration
  if(freq)
  	tone(BUZZER, freq, SAMPLE_DURATION_MS);
  // Wait for next run
  delay(SAMPLE_DURATION_MS);
}