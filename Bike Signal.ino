#define LEFT_SGN  0x0C
#define RIGHT_SGN 0x0B
#define STOP_SGN  0x0A
#define LR_SGN    0x09

#define LEFT_TRN  0x08
#define RIGHT_TRN 0x07
#define STOP_TRN  0x06

void setup()
{
  pinMode(LEFT_SGN, INPUT);
  pinMode(RIGHT_SGN, INPUT);
  pinMode(STOP_SGN, INPUT);
  pinMode(LR_SGN, INPUT);

  pinMode(LEFT_TRN, OUTPUT);
  pinMode(RIGHT_TRN, OUTPUT);
  pinMode(STOP_TRN, OUTPUT);
}

bool turning_state_left = LOW;
bool turning_state_right = LOW;
bool stopping_state = LOW;
bool stopping_state_flkr = LOW;
bool pre_lr_state = LOW;

uint32_t counter1 = 0;
uint32_t counter2 = 0;
uint32_t counter3 = 0;
uint32_t counter4 = 0;

constexpr uint32_t counter_up = 16000000 / 4000; // assuming that the frequincy is fixed at 16MHz

void loop()
{

  bool lr_state = digitalRead(LR_SGN);

  if(lr_state != pre_lr_state) // reset both counters for left and right signal beforehand
  {
    pre_lr_state = lr_state;
    counter1 = 0;
    counter2 = 0;
    turning_state_left = HIGH;
    turning_state_right = HIGH;
  }

  if(digitalRead(LEFT_SGN) || lr_state)
  {
    if(counter1 % counter_up == 0)
      digitalWrite(LEFT_TRN, turning_state_left = !turning_state_left);
    counter1++;
  }
  else
  {
    digitalWrite(LEFT_TRN, turning_state_left = LOW);
    counter1 = 0;
  }

  if(digitalRead(RIGHT_SGN) || lr_state)
  {
    if(counter2 % counter_up == 0)
      digitalWrite(RIGHT_TRN, turning_state_right = !turning_state_right);
    counter2++;
  }
  else 
  {
    digitalWrite(RIGHT_TRN, turning_state_right = LOW);
    counter2 = 0;
  }
  
  if(digitalRead(STOP_SGN))
  {
    if(counter3 % 6000 == 0)
    {
      stopping_state = !stopping_state;
    }

    if(stopping_state)
    {
      if(counter4 % 500 == 0)
        digitalWrite(STOP_TRN, stopping_state_flkr = !stopping_state_flkr);
      counter4++;
    }
    else 
      counter4 = 0;
      
    counter3++;
  }
  else
  {
    digitalWrite(STOP_TRN, stopping_state_flkr = LOW);
    stopping_state = LOW;
    counter3 = 0;
    counter4 = 0;
  }
}
