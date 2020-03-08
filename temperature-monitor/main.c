// Pins used
enum DigitalPins{
  LED_RGB_RED = 3,
  LED_RGB_BLUE = 5,
  LED_RGB_GREEN = 6
};
enum AnalogPins{
  TEMPERATURE_SENSOR = 0
};
// Sampling properties
#define ANALOG_READ_N 10
#define ANALOG_READ_MS 5
// User/global properties
#define UPDATE_DELAY_MS 200

// Helper macros
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

// Resolves temperature to RGB color
struct rgb_t{byte red, green, blue;};
static struct rgb_t celsius2color(const int celsius){
  static const struct colorstop_t{const int celsius; const struct rgb_t color;} color_stops[] = {
    {-51, {255,255,255}},
    {0, {0,0,255}},
    {20, {0,255,0}},
    {40, {255,255,0}},
    {80, {255,0,0}},
    {451, {255,127,127}}
  };
  for(byte i = 0; i < ARRAY_SIZE(color_stops)-1; ++i){
    const struct colorstop_t cs1 = color_stops[i],
    				  cs2 = color_stops[i+1];
    if(cs1.celsius <= celsius && cs2.celsius >= celsius)
      return {
      	map(celsius, cs1.celsius, cs2.celsius, cs1.color.red, cs2.color.red),
        map(celsius, cs1.celsius, cs2.celsius, cs1.color.green, cs2.color.green),
        map(celsius, cs1.celsius, cs2.celsius, cs1.color.blue, cs2.color.blue)
      };
  }
  return {0};
}

// Converts analog signal of sensor to temperature in celsius
static int analog2celsius(const unsigned analog){
  return (analog * 5.0f / 1023 - 0.5f) * 100;	// SIGNAL * MAX_VOLTAGE / MAX_SIGNAL = VOLTAGE; (VOLTAGE - 0.5) * 100 = CELSIUS
}

// Processor setup run
void setup() {
  // Set LED pins ready for output
  pinMode(LED_RGB_RED, OUTPUT);
  pinMode(LED_RGB_BLUE, OUTPUT);
  pinMode(LED_RGB_GREEN, OUTPUT);
}
// Processor idle callback
void loop() {
  // Read temperature multiple times for good approximation
  unsigned sensor_accum = 0;
  for(byte i = 0; i < ANALOG_READ_N; ++i){
  	sensor_accum += analogRead(TEMPERATURE_SENSOR);
  	delay(ANALOG_READ_MS);
  }
  // Evaluate temperature from collected data and convert to celsius unit
  const int celsius = analog2celsius(sensor_accum / ANALOG_READ_N);
  // Set LED to temperature color
  const struct rgb_t color = celsius2color(celsius);
  analogWrite(LED_RGB_RED, color.red);
  analogWrite(LED_RGB_BLUE, color.blue);
  analogWrite(LED_RGB_GREEN, color.green);
  // Send temperature into network
  
  // TODO: Serial data to ESP module (WiFi library)
  
  // Wait and let processor rest
  delay(UPDATE_DELAY_MS);
}