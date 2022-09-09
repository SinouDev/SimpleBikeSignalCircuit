#define LEFT_SGN  0x0C /* The left pin 12 input switch */
#define RIGHT_SGN 0x0B /* The right pin 11 input switch */
#define STOP_SGN  0x0A /* The stop pin 11 input switch */
#define LR_SGN    0x09 /* The left right 10 pin input switch */

#define LEFT_TRN  0x08 /* The left pin 08 light indicator */
#define RIGHT_TRN 0x07 /* The right pin 07 light indicator */
#define STOP_TRN  0x06 /* The stop pin 06 light indicator */

#define UNKNOWN_STATE       0x00 /* Not in use for now */
#define TURNING_STATE_LEFT  0x01 /* The left indicator state flag : 1 for HIGH and 0 for LOW */
#define TURNING_STATE_RIGHT 0x02 /* The right indicator state flag : 1 for HIGH and 0 for LOW */
#define TURNING_STATE_LR    0x04 /* The both left and right indicators state flag : 1 for HIGH and 0 for LOW */
#define STOPPING_STATE      0x08 /* The stop signal state flag : 1 for HIGH and 0 for LOW */
#define STOPPING_STATE_FLKR 0x10 /* The stop flickering signal state flag : 1 for HIGH and 0 for LOW */
#define PRE_LEFT_STATE      0x20 /* The left previous indicator state flag : 1 for HIGH and 0 for LOW */
#define PRE_RIGHT_STATE     0x40 /* The right previous indicator state flag : 1 for HIGH and 0 for LOW */
#define PRE_LR_STATE        0x80 /* The both left and right previous indicator state flag : 1 for HIGH and 0 for LOW */

uint8_t output_state = 0; // state flags storage

uint32_t counter1 = 0U; // the left indicator counter
uint32_t counter2 = 0U; // the right indicator counter
uint32_t counter3 = 0U; // the stop signal counter
uint32_t counter4 = 0U; // the stop flickering counter

constexpr uint32_t module_frequency = 16000000U; // arduino nano atmega328p frequency at 16MHz

constexpr uint32_t counter_up1 = module_frequency /  6400U;
constexpr uint32_t counter_up2 = module_frequency / 64000U;

void setup()
{
  pinMode(LEFT_SGN, INPUT);
  pinMode(RIGHT_SGN, INPUT);
  pinMode(STOP_SGN, INPUT);
  pinMode(LR_SGN, INPUT);

  pinMode(LEFT_TRN, OUTPUT);
  pinMode(RIGHT_TRN, OUTPUT);
  pinMode(STOP_TRN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  cli(); // clear interrupts
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  
  OCR0A = 255;

  TCCR0A |= (1 << WGM01);
  
  TCCR0B |= (1 << CS01);   
  
  TIMSK0 |= (1 << OCIE0A);
  sei(); // enable interrupts

  //Serial.begin(2000000);
}

bool tr = false;

ISR(TIMER0_COMPA_vect)
{
  output_state = digitalRead(LR_SGN) ? TURNING_STATE_LR | output_state : ~TURNING_STATE_LR & output_state;

  if((output_state & TURNING_STATE_LR) != (PRE_LR_STATE & output_state)) // reset both counters for left and right signal beforehand
  {
    output_state = output_state & TURNING_STATE_LR ? PRE_LR_STATE | output_state : ~PRE_LR_STATE & output_state;
    counter1 = 0;
    counter2 = 0;
  }

  if(digitalRead(LEFT_SGN) || output_state & PRE_LR_STATE)
  {
    if(counter1 % counter_up1 == 0)
      output_state = output_state & TURNING_STATE_LEFT ? ~TURNING_STATE_LEFT & output_state : TURNING_STATE_LEFT | output_state;
    counter1++;
  }
  else
  {
    output_state &= ~TURNING_STATE_LEFT;
    counter1 = 0;
  }

  if(digitalRead(RIGHT_SGN) || output_state & PRE_LR_STATE)
  {
    if(counter2 % counter_up1 == 0)
      output_state = output_state & TURNING_STATE_RIGHT ? ~TURNING_STATE_RIGHT & output_state : TURNING_STATE_RIGHT | output_state;
    counter2++;
  }
  else 
  {
    output_state &= ~TURNING_STATE_RIGHT;
    counter2 = 0;
  }
  
  if(digitalRead(STOP_SGN))
  {
    if(counter3 % counter_up1 == 0)
      output_state = output_state & STOPPING_STATE ? ~STOPPING_STATE & output_state : STOPPING_STATE | output_state;

    if(output_state & STOPPING_STATE)
    {
      if(counter4 % counter_up2 == 0)
        output_state = output_state & STOPPING_STATE_FLKR ? ~STOPPING_STATE_FLKR & output_state : STOPPING_STATE_FLKR | output_state;
      counter4++;
    }
    else 
      counter4 = 0;
      
    counter3++;
  }
  else
  {
    output_state &= ~STOPPING_STATE_FLKR;    
    counter3 = 0;
    counter4 = 0;
  }
}

void loop()
{
  digitalWrite(LEFT_TRN, output_state & TURNING_STATE_LEFT);
  digitalWrite(RIGHT_TRN, output_state & TURNING_STATE_RIGHT);
  digitalWrite(STOP_TRN, output_state & STOPPING_STATE_FLKR);
  
  //Serial.print("outout_state = 0b");
  //for (int32_t i = 0; i < sizeof(output_state) * 8; i++)
  //{
  //  Serial.print((output_state >> i) & 1 ? "1" : "0");
  //}
  //Serial.println();  
}
