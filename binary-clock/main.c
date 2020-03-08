// Digital pins positions
enum DigitalPins{
  SECOND_TICK = 0,
  MINUTE_LOW_BIT0,
  MINUTE_LOW_BIT1,
  MINUTE_LOW_BIT2,
  MINUTE_LOW_BIT3,
  MINUTE_HIGH_BIT0,
  MINUTE_HIGH_BIT1,
  MINUTE_HIGH_BIT2,
  HOUR_LOW_BIT0,
  HOUR_LOW_BIT1,
  HOUR_LOW_BIT2,
  HOUR_LOW_BIT3,
  HOUR_HIGH_BIT0,
  HOUR_HIGH_BIT1,
  PINS_NUMBER
};
// Short delay for better performance
#define UPDATE_DELAY_MS 200

// Time ratio
#define SECOND_MS 1000
#define MINUTE_MS 60000
#define HOUR_MS 3600000
#define DAY_MS 86400000

// Time structure
struct time_t{
  byte hour, minute, second;
  unsigned millisecond;
  
  time_t(void){
    this->hour = this->minute = this->second = this->millisecond = 0;
  }
  bool validate(void){
    return this->hour < 24 && this->minute < 60 && this->second < 60 && this->millisecond < 1000;
  }
  struct time_t* add(unsigned long ms){
  	const div_t hour_div = div(ms % DAY_MS, HOUR_MS),
                minute_div = div(hour_div.rem, MINUTE_MS),
                second_div = div(minute_div.rem, SECOND_MS);
    this->millisecond += second_div.rem;
    if(this->millisecond >= 1000)
      this->millisecond -= 1000, this->second++;
    this->second += second_div.quot;
    if(this->second >= 60)
      this->second -= 60, this->minute++;
    this->minute += minute_div.quot;
    if(this->minute >= 60)
      this->minute -= 60, this->hour++;
    this->hour += hour_div.quot;
    if(this->hour >= 24)
      this->hour -= 24;
    return this;
  }
};

// Set LED pins to hour and minute
static void set_leds(const struct time_t time){
  const div_t minute_div = div(time.minute, 10),
  			hour_div = div(time.hour, 10);
  digitalWrite(MINUTE_LOW_BIT0, bitRead(minute_div.rem, 0) ? HIGH : LOW);
  digitalWrite(MINUTE_LOW_BIT1, bitRead(minute_div.rem, 1) ? HIGH : LOW);
  digitalWrite(MINUTE_LOW_BIT2, bitRead(minute_div.rem, 2) ? HIGH : LOW);
  digitalWrite(MINUTE_LOW_BIT3, bitRead(minute_div.rem, 3) ? HIGH : LOW);
  digitalWrite(MINUTE_HIGH_BIT0, bitRead(minute_div.quot, 0) ? HIGH : LOW);
  digitalWrite(MINUTE_HIGH_BIT1, bitRead(minute_div.quot, 1) ? HIGH : LOW);
  digitalWrite(MINUTE_HIGH_BIT2, bitRead(minute_div.quot, 2) ? HIGH : LOW);
  digitalWrite(HOUR_LOW_BIT0, bitRead(hour_div.rem, 0) ? HIGH : LOW);
  digitalWrite(HOUR_LOW_BIT1, bitRead(hour_div.rem, 1) ? HIGH : LOW);
  digitalWrite(HOUR_LOW_BIT2, bitRead(hour_div.rem, 2) ? HIGH : LOW);
  digitalWrite(HOUR_LOW_BIT3, bitRead(hour_div.rem, 3) ? HIGH : LOW);
  digitalWrite(HOUR_HIGH_BIT0, bitRead(hour_div.quot, 0) ? HIGH : LOW);
  digitalWrite(HOUR_HIGH_BIT1, bitRead(hour_div.quot, 1) ? HIGH : LOW);
}

// Initialize device
static struct time_t current_time;
void setup(){
  // Ask for start time
  Serial.begin(9600);
  while(!Serial)
    delay(UPDATE_DELAY_MS);
  do{
    Serial.println("Please insert start time in valid format 'HOUR:MINUTE':");
    while(!Serial.available())
      delay(UPDATE_DELAY_MS);
  }while(sscanf(Serial.readString().c_str(), "%u:%u", &current_time.hour, &current_time.minute) != 2 || !current_time.validate());
  Serial.println(current_time.hour + String(':') + current_time.minute);
  Serial.end();
  // Set modes of digital pins for I/O
  for(byte pin = SECOND_TICK; pin < PINS_NUMBER; ++pin)
    pinMode(pin, OUTPUT);
}
// Update clock state
void loop(){
  // Update time
  const unsigned long current_ms = millis();
  static unsigned long last_ms = current_ms;
  set_leds(*current_time.add(current_ms - last_ms));
  last_ms = current_ms;
  // Tick second
  static unsigned long last_tick = current_ms;
  digitalWrite(SECOND_TICK, current_ms - last_tick >= SECOND_MS ? (last_tick += SECOND_MS, HIGH) : LOW);
  // Some rest for the processor
  delay(UPDATE_DELAY_MS);
}