// Aiden's one year birthday gift, A blue minicooper, modified as 
// remote control car 
// specification : 
// The car come with only one wheel drive 
// try to make it forward and backward 

//=============================
// int analogInPin[] = {A0,A1,A2,A3,A4,A5};
int analogInPin[] = {A2};
int analogInPin_NUM = 1 ;  // length of analogInPin
int bk_right_1 = 7; 
int bk_right_2 = 8;
int sensorValue = 0;
//===initilize variables ========
int read_signal[]={0}; // signal from transimtter 

void setup() {
   Serial.begin(9600); 
  // put your setup code here, to run once:
 for(int i=0;i< analogInPin_NUM;i++){
  pinMode(analogInPin[i], INPUT);
 
 }
  pinMode(bk_right_1,OUTPUT);
  pinMode(bk_right_2,OUTPUT);
  digitalWrite(bk_right_1,HIGH); // disable 
  digitalWrite(bk_right_1,HIGH); // disable 
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<analogInPin_NUM;i++){
    sensorValue = pulseIn(analogInPin[i],HIGH);
    read_signal[i]=get_signal(sensorValue);  
    /*
    Serial.print(sensorValue);
    Serial.print(":");
    Serial.print(read_signal[i]);
    Serial.print("|" );
    */
  }
  // drive car 
  drive(read_signal[0],1);
}

//=====
int get_signal(int raw_value){
// convert raw value from transimtter to signal 
    int low_cut=1090;
    int high_cut=1914;   // cut raw_value in interval [low_cut,high_cut]
    int mid_value_low=1400;
    int mid_value_high=1600;
    int signal; 

    if(raw_value==0){
       return 0; 
     }

     // constraint raw value into the interval 
    int sensor_read=constrain(raw_value, low_cut, high_cut); 
    //classify the signal 
    if(sensor_read<mid_value_low){
         signal=-1;
    }else if(sensor_read>mid_value_high){
         signal=1;
    }else{
         signal=0;
    }

      return signal;
}
void drive(int direction,int delay_time){
  // car drive 
  // direction : 1 forward  or -1 backward  or stop 
  int high_pin[] = {bk_right_1}; 
  int low_pin[] = {bk_right_2};
  int NUM_pin = 1;
  if(direction==1){
    set_high_low(high_pin,NUM_pin,low_pin,NUM_pin);
  }else if(direction==-1){
    set_high_low(low_pin,NUM_pin,high_pin,NUM_pin);
  }else{
    // stop 
    set_high_low(high_pin,NUM_pin,high_pin,0);
    set_high_low(low_pin,NUM_pin,low_pin,0);
  }
  delay(delay_time);
}
void set_high_low(int pin_high[],int high_size,int pin_low[],int low_size){ 
  // set pin_high high 
  // set pin_low low 
  for(int i=0;i<high_size;i++){
     digitalWrite(pin_high[i],HIGH); 
  }
  for(int i=0;i<low_size;i++){
     digitalWrite(pin_low[i],LOW);
  }
  
}
