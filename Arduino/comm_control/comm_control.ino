// Motorized commutator control

#define PWMENC 2                    // AS5030 pwm readout, pin 2 on Arduino Uno (interrupt 0)
#define PWMA 3                      // Motor driver PWM
#define AIN1 7                      // Motor direction input 1
#define AIN2 4                      // Motor direction input 2
#define STBY 8                      // Motor driver standby (1 for normal operation)

const int MIN_CH = 10;              // Minimum reference angle change to move
const int MOTOR_MAX_SPEED = 90;     // Maximum motor speed
const int MOTOR_K = 1;              // Speed proportional factor
volatile int ref_angle0;            // Sensor readout
volatile int t0;                   
int ref_angle_cal = 0;
int ref_angle = 0;
bool init_done = 0;      

void setup() {
  pinMode(PWMENC, INPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  attachInterrupt(0, rising_edge, RISING); // Interrupt 0 attached to pin 2
  init_done = 0;
}

void loop() { 
  int c_speed = 0;
  int dir = 0;
  ref_angle = ref_angle0;

  // Calibration
  if (init_done == 0) {
    delay(100);
    init_done = 1;
  }

  c_speed = MOTOR_K * abs(ref_angle);   // Set motor speed 
  dir = get_dir(ref_angle);             // Set motor direction
  
  if (abs(ref_angle) > MIN_CH) {
    if (c_speed > MOTOR_MAX_SPEED) {
      c_speed = MOTOR_MAX_SPEED;
    }
    motor_go(dir, c_speed);
  } else {
    c_speed = 0;
    motor_stop();
  }

  init_done = 1;
}

void rising_edge() {
  attachInterrupt(0, falling_edge, FALLING);
  t0 = micros();
}
 
void falling_edge() {
  attachInterrupt(0, rising_edge, RISING);
  ref_angle0 = micros()-t0;
}

void motor_go(int direction, int speed){
  boolean in1;
  boolean in2;
  if(direction >= 0) { // CW
    in1 = HIGH;
    in2 = LOW;
  } else { // CCW
    in1 = LOW;
    in2 = HIGH;
  }
  digitalWrite(STBY, HIGH); 
  digitalWrite(AIN1, in1);
  digitalWrite(AIN2, in2);
  analogWrite(PWMA, speed);
}

void motor_stop(){
  digitalWrite(STBY, LOW);
  analogWrite(PWMA, 0);
}

int get_dir(int pos){
  return (pos > 0) - (pos < 0);
}
