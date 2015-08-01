// steven  remote car second version 07/2015
// smart yellow car
// Board : mega 2560 

//================================
// import librarys 
#include <SoftwareSerial.h> // use this library to reset tx and rx 
#include <Servo.h> // use servo 

//============================

//=============set up pin ==========
SoftwareSerial xbee_Serial(11, 10); // RX, TX
int analogInPin[] = {A0,A1,A2,A3}; // channels for rc transmitter
// channel setting
// 1: for left or right 
// 2: for camera angle change direction  
// 3: for forward or backward
// 5: switch for camera : base or angle camera 

// set car wheels pins 
int ft_left_1=24; // front left 
int ft_left_2=25; 
int bk_left_1=26; // back left 
int bk_left_2=27; 
int ft_right_1=50; // front right 
int ft_right_2=51; 
int bk_right_1=52; // back right 
int bk_right_2=53; 

int base_camera_pin=46; // camera base servo 
int angle_camera_pin=47; // camera anlge servo 
int dis_sensor_pin = 44; // distance sensor pin 
int d_echo_Pin=22; // distance sensor pin
int d_trig_pin=23; //distance sensor pin

int car_wheel_Pin[] = {ft_left_1,ft_left_2,bk_left_1,bk_left_2,ft_right_1,ft_right_2,bk_right_1,bk_right_2};

//===initilize variables ========
int read_signal[]={0,0,0,0}; // signal from transimtter 
char auto_cmmd; // command from xbee 
// camera servo 
Servo base_servo;  
Servo angle_servo; 
Servo sensor_servo; 
// servo angle 
int base_cam_angle = 120;
int top_cam_angle = 90; 
int sensor_angle =120; 
int incre=3; // angle increment 
int min_angle = 50 ; 
int max_angle = 180;
int CENTRAL_ANGLE = 120;
const int   anglge_size =  5; // array size of different angles 
int angle_array[anglge_size];
int dis_array[anglge_size]; // distance at each angle 
int dis_limit = 35 ; 
int TURN_BACK = 0 ;  // drive car turn_around 
int TURN_LEFT = 0 ;  // turn left 
int TURN_RIGHT = 0;  // turn right
int MAX_DIS_INDEX = 0 ; // index of max distance 
int dis; // distance in front of car; 
int stop_ct = 0 ; // stop cycyle counter 
int stop_ct_limit = 2 ; 
int sweep_ct = 0; // sweep cycle counter 
int sweep_ct_limit = 300; 

//============

void setup() {
  // put your setup code here, to run once:
  // initialize serial communications :
  Serial.begin(19200); 
 // set up pin model for read transimmter 
for(int i=0;i<4;i++){
  pinMode(analogInPin[i], INPUT);
} 
 // initialize car wheel pin
for(int i=0;i<7;i++){
    pinMode(car_wheel_Pin[i], OUTPUT);
}

  xbee_Serial.begin(19200); // xbee serial

  pinMode(d_trig_pin, OUTPUT); // distance sensor 
  pinMode(d_echo_Pin, INPUT);  // distance sensor
   // camera servo 
  base_servo.attach(base_camera_pin);
  angle_servo.attach(angle_camera_pin);
  // sensor servo
  sensor_servo.attach(dis_sensor_pin);
  
  // set initial angel for servo 
   base_servo.write(base_cam_angle);
   angle_servo.write(top_cam_angle);
   sensor_servo.write(sensor_angle);
  // initialize angle array
 set_angle_array(min_angle,max_angle,anglge_size);
} 
// end of setup 
 
void loop_debug(){
   
 //long d = distance_sensor();
 //Serial.print(d);
// test camera 

 // ============
 // Serial.println();
 // char *xx ="xyz\n";  terminal_print(1,xx);
 //char *mm ;
 /*char mm ; 
 mm = get_xbee_cmd(); 
 Serial.print(mm);
 //terminal_print(1,mm);
 terminal_print(1,"===\n");
 //xbee_Serial.println(str);
 if(xbee_Serial.available()){
  xbee_Serial.write((char)mm);
 }
 */
 //sweep_around(1);
auto_car();
}
void loop() {
  Serial.print("terminal :");
  //terminal_print( 1, "terminal :");
  // put your main code here, to run repeatedly:
  auto_cmmd = get_xbee_cmd();
  //Serial.write(auto_cmmd);// print 
  
  // switch control mode 
if(auto_cmmd=='c'){
    // command mode: control car by sending command  
    Serial.print("command mode");
    xbee_Serial.print("command mode");
    //terminal_print(1,"command mode");
    cmd_car();
}else if(auto_cmmd=='a'){ 
    Serial.print("auto mode");
    xbee_Serial.print("auto mode");
    //terminal_print(1,"auto mode");
    auto_car();
}else{
    Serial.print("rc mode");
    //terminal_print(1,"rc mode");
    rc_car(); 
} 
  Serial.print("\n");
  //terminal_print(1," \n");
}
//========end of loop =====
// 
void cmd_car(){
  char cmd;
  int stoploop=0;
  int cmd_time = 200;
  int switch_ct = 1 ; // switch counter 
  int central_angle = 120; 
  int servo_switch = 1 ; // select which servo to control 
  int servo_angle = central_angle ;
  int base_angle = central_angle ; 
  int up_angle = 90 ; 
  int angle_inc = 10 ; // anlge increament 
  while(1){
     cmd = get_xbee_cmd(); 
      Serial.write(cmd);
      switch(cmd){
        case 'w':
          drive(1,cmd_time);
          drive(0,0);
          Serial.print("cmd drive forward;");
          xbee_Serial.print("cmd drive forward;");
          break; 
        case 's':
          drive(-1,cmd_time);
          drive(0,0);
          Serial.print("cmd drive back;");
          xbee_Serial.print("cmd drive back;");
          break;
        case 'a':
          turn(3,cmd_time);
          drive(0,0);
          Serial.print("cmd drive left;");
          xbee_Serial.print("cmd drive left;");
          break;
         case 'd':
          turn(1,cmd_time);
          drive(0,0);
          Serial.print("cmd drive right;");
          xbee_Serial.print("cmd drive right;");
          break;
         case 'i': // select switch 
          Serial.print("cmd select switch: ");
          xbee_Serial.print("cmd select switch:");
          if(switch_ct ==1){
            servo_switch = 1 ; //base 
            up_angle = servo_angle;
            servo_angle = base_angle; 
            central_angle = 120 ; 
          }
          if(switch_ct ==2){
            servo_switch = -1; // angle
            base_angle = servo_angle;
            servo_angle = up_angle; 
            central_angle = 90 ;
          }
          ++switch_ct; 
          if(switch_ct > 2){
            switch_ct = 1 ; //reset ct 
          }
          
          Serial.println(servo_switch);
          xbee_Serial.println(servo_switch);
          break;
          
        case 'k': 
          Serial.print("cmd reset angle");
          xbee_Serial.print("cmd reset angle;");
          servo_angle=set_servo_angle(servo_switch,central_angle,1);
          break;
          
        case 'j': 
          Serial.print("cmd minus angle");
          xbee_Serial.print("cmd minus angle;");
          servo_angle=set_servo_angle(servo_switch,servo_angle-angle_inc,0);
          break;
          
        case 'l': 
          Serial.print("cmd plus angle");
          xbee_Serial.print("cmd plus angle;");
          servo_angle=set_servo_angle(servo_switch,servo_angle+angle_inc,0);
          break; 
                  
        case 'q': 
          Serial.print("cmd quit;");
          xbee_Serial.print("cmd quit;");
          auto_cmmd='d'; // switch to rc mode
          stoploop =1;
          break;
        default:  
          drive(0,0);
      } //end of switch

      
       Serial.println("while loop;");
        //xbee_Serial.println("while loop;");
      if(stoploop==1) break;
  }
} //end of cmd car 

void auto_car(){
   // auto car mode 
   // check cmd 
   int stoploop = 0 ;
   while(1){
     char cmd = get_xbee_cmd();
     if(cmd =='q') {
        auto_cmmd='d'; // switch to rc mode
        xbee_Serial.print(" auto quit;");
        stoploop =1;
        sensor_servo.write(CENTRAL_ANGLE); // reset angle 
      }else{
        // sweep distinace sensor
        //sweep_around(1);
        //think(1); 
        // get distance sensor in front of car 
        dis = distance_sensor(); 
        if(dis < dis_limit ){
          stop_ct++; 
            if(stop_ct > stop_ct_limit ){
              //confirm that have  
              stop_ct = 0; //reset ct
              drive(0,1000);  // stop 
              // sweep and think 
              sweep_around(1);
              think(1);
              // set angle back to central 
              sensor_angle=set_servo_angle(2,CENTRAL_ANGLE,100);
              Serial.print("*****\n"); 
            }
        }else{
           drive(1,1); // drive 
           sweep_ct++;  // sweep ct 
           if(sweep_ct > sweep_ct_limit){
              // reset ct 
              sweep_ct = 0 ; 
              // sweep 
              sweep_around(1);
              // to do ??
          }
        }
        

        Serial.print("stop_ct :: "); 
        Serial.print(stop_ct);
        Serial.print("::"); 
        Serial.print(sweep_ct);
        Serial.print("dis::"); 
        Serial.print(dis);
        Serial.print("\n");
        
        //delay(500); 
    }
    if( stoploop==1) break;
     
   } // end of while loop 
}

//==========
void rc_car(){ // remote control mode 

// get signal from transimtter 
for(int i=0;i<4;i++){
   int sensorValue = pulseIn(analogInPin[i],HIGH);
    // Serial.print("sensor = " );                       
    Serial.print(sensorValue);
    Serial.print(":");
    read_signal[i]=get_signal(sensorValue);
    Serial.print(read_signal[i]);
    Serial.print("|" ); 
}
 

//====== set left or right : channel 1
    if(read_signal[0]==-1){
      //left 
       turn(3,0);
    }else if(read_signal[0]==1){
    //right 
      turn(1,0);
     }else{
       //drive
     drive(read_signal[2],1); // drive
     }
 // move camera  servo :  
 int dir = read_signal[1]; 
 
 if(read_signal[3] == -1){ 
     // cam base servo 
     base_cam_angle=set_servo_angle(1,base_cam_angle+dir*3,20);
    }
 if(read_signal[3] == 1){ 
   // cam top servo 
     top_cam_angle=set_servo_angle(-1,top_cam_angle+dir*3,20);
  }
   

} 
//===end of rc_car ==========

// 
long distance_sensor(){
 // get distance from distance sensor 
 digitalWrite(d_trig_pin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(d_trig_pin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(d_trig_pin, LOW);
 long  duration = pulseIn(d_echo_Pin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 long distance = duration/58.2;
 return distance;
 
}
// end of distance 
//========
int set_servo_angle(int servo_switch,int servo_angle,int servo_time){
  
  // set servo with servo_angle during servo_time 
  servo_angle =constrain(servo_angle,0, 179); 
 // get the servo 
 switch(servo_switch){
    case 1: 
      base_servo.write(servo_angle);
      break;
    case -1: 
      angle_servo.write(servo_angle); 
      break; 
    case 2: 
      sensor_servo.write(servo_angle); 
      break; 
    default: 
      break; 
  }
 
 delay(servo_time);
 return servo_angle;
}
// end of camera 

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
//===end of get_signal===
//============================
void drive(int direction,int delay_time){
  // car drive 
  // direction : 1 forward  or -1 backward  or stop 
  int high_pin[] = {ft_left_1,ft_right_1,bk_left_1,bk_right_1}; 
  int low_pin[] = {ft_left_2,ft_right_2,bk_left_2,bk_right_2};
  if(direction==1){
    set_high_low(high_pin,4,low_pin,4);
  }else if(direction==-1){
    set_high_low(low_pin,4,high_pin,4);
  }else{
    // stop 
    set_high_low(high_pin,0,high_pin,4);
    set_high_low(high_pin,0,low_pin,4);
  }
  delay(delay_time);
}
//==end of drive ====

//===========
void turn(int direction, int turn_time){
  // car turn left or right or stop 
  // direction : 
  // 1: front right 
  // 2: back right 
  // 3: front left 
  // 4: back left 

  //========
 //  int high_pin[] = {ft_left_1,bk_left_1}; 
 //  int low_pin[] = {ft_left_2,ft_right_2,bk_left_2,bk_right_2,ft_right_1,bk_right_1};
  if(direction==1){ // turn Front right 
    
  int high_pin[] = {ft_right_1,bk_right_1}; 
  //int high_pin[] = {ft_right_1}; 
  int low_pin[] = {ft_left_1,ft_left_2,ft_right_2,bk_left_1,bk_left_2,bk_right_2};
    set_high_low(high_pin,2,low_pin,6);
  }else if(direction==2){ // turn back right 
     int high_pin[] = {ft_right_1,bk_right_1}; 
     int low_pin[] = {ft_left_1,ft_left_2,ft_right_2,bk_left_1,bk_left_2,bk_right_2};
    set_high_low(low_pin,6,high_pin,2);
  }else if(direction==3){// front left 
    int high_pin[] = {ft_left_1,bk_left_1}; 
    int low_pin[] = {ft_left_2,ft_right_2,bk_left_2,bk_right_2,ft_right_1,bk_right_1};
    set_high_low(high_pin,2,low_pin,6);
  }else if(direction==4){ // back left 
    int high_pin[] = {ft_left_1,bk_left_1}; 
    int low_pin[] = {ft_left_2,ft_right_2,bk_left_2,bk_right_2,ft_right_1,bk_right_1};
    set_high_low(low_pin,6,high_pin,2);
  }else{
    // stop 
   int high_pin[] = {ft_left_1,bk_left_1}; 
   int low_pin[] = {ft_left_2,ft_right_2,bk_left_2,bk_right_2,ft_right_1,bk_right_1};
    set_high_low(high_pin,0,high_pin,2);
    set_high_low(high_pin,0,low_pin,6);
  }
  delay(turn_time);
}
//===================end of turn ======

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

//== serial moniter print 

void  terminal_print(int ter, char *str ){

if(ter ==1){
  Serial.print(str);
}

if(ter == 2){
  xbee_Serial.println(str);
}

}
// == get xbee cmd 
char get_xbee_cmd(){
  
 char xcmd; 
   if(xbee_Serial.available()){
       xcmd = xbee_Serial.read(); 
     }else{
       xcmd ='0';
     }
   return xcmd ; 
} 
// sweep distance sensor 
void sweep(int min_angel ,int max_angle ){
  
//int max_angle =179; 
//int min_angel = 0 ; 
 
sensor_angle=set_servo_angle(2,sensor_angle+incre,20);
  if(sensor_angle>=max_angle){
    incre=-3;
  }
  if(sensor_angle<=min_angel){
    incre=3;
  }
  
}

void set_angle_array(int min_angle,int max_angle,int anglge_size){
    int interval  =  (max_angle-min_angle)/(anglge_size-1); 
    for(int i=0;i<anglge_size; i++){
      angle_array[i] = min_angle + i*interval; 
    }
}


void sweep_around(int show){

  // sweep distinace sensor around at angle_array, and collect 
  // distance sensor value 
  //    int ii ; // index of angle array 
     TURN_BACK = 1 ;
     TURN_LEFT = 0 ; 
     TURN_RIGHT = 0;
     MAX_DIS_INDEX=0; 
    for(int i=0;i<anglge_size; i++){
      // set sensor servo angle 
      sensor_angle=set_servo_angle(2,angle_array[i],200);
      // get distance 
      dis_array[i] = distance_sensor();
      if(dis_array[i] >=dis_array[MAX_DIS_INDEX]  ){
         MAX_DIS_INDEX = i;
       }
      if(dis_array[i] <= dis_limit){ 
        // stop 
        drive(0,100);
       }else{ // if any of dis > dis_limit , not turn  back 
        TURN_BACK = 0; 
       }  
      //delay(500); 
      // set angle back to central 
      sensor_angle=set_servo_angle(2,CENTRAL_ANGLE,1);

      // cal if turn left or right 
      if(i<anglge_size/2 & dis_array[i] < dis_limit){
        // if any right side dis < dis limit , turn left 
        TURN_LEFT = 1; 
      }
      if(i > anglge_size/2 & dis_array[i] < dis_limit){
        // if any right side dis < dis limit , turn right 
        TURN_RIGHT = 1; 
      }
     if(show ==1){
      Serial.print(i);
      Serial.print(":");
      Serial.print(angle_array[i]);
      Serial.print(":");
      Serial.print(dis_array[i]); 
      Serial.print("|");
     }
    } // end of for loop
   if(show==1){
      Serial.print("\n");
      Serial.print("TURN_BACK:: ");
      Serial.print(TURN_BACK);
      Serial.print("TURN_LEFT:: ");
      Serial.print(TURN_LEFT);
      Serial.print("TURN_RIGHT:: ");
      Serial.print(TURN_RIGHT);
      Serial.print("\n");
    }
    
}

void think(int show){
  // think how to move based on the distance values 
  int turn_dir = 0  ;  // 1 right, 3:left
  
  // calculate turn direction based on the distance sensor values
  if(TURN_RIGHT ==1 & TURN_LEFT==1){ 
    // if turn left and right all true 
    TURN_BACK =1 ; 
  }else if(TURN_RIGHT==1){ // any left side sensor < dis_limit; turn right 
    turn_dir =1 ; 
  } else if(TURN_LEFT==1){ // any right side sensor < dis_limit; turn left
    turn_dir = 3; 
  }
  

  
  // if all distinace less than limit, i.e. no way out
  // turn around 
  if(TURN_BACK ==1){ 
    turn_around(turn_dir);
    if(show ==1){
     Serial.println("turn around"); 
    }
   }else{
     // move toward to the angle with maximum distance 
     // turn to angle 
     turn(turn_dir,500);
     drive(0,500);
     
     /*
     int central_angle = angle_array[anglge_size/2]; // central angle 
     int pos =   MAX_DIS_INDEX;
     if(pos>= anglge_size) {pos = 2*(anglge_size-1)-MAX_DIS_INDEX ; }
     int max_angle = angle_array[pos];
     int diff = max_angle - central_angle ; 
     int turn_dir; 
     if(diff>=0){
      turn_dir = 3;  // turn left 
     }else{
      turn_dir = 1;  // turn right 
     } 
     int turn_time = (abs(diff)/90.0)*400;  
     turn(turn_dir,turn_time);
     drive(0,0);
     */
     if(show == 1){
      //Serial.print("central angle:: ");
      //Serial.print( central_angle);
       
      Serial.print("\n");
     }
   }
   
  

}

void turn_around(int dir){
   // turn around 
   // dir :: 1: right ; 3: left 
   if(dir == 0) { // if no dir, choose right 
      dir = 1; 
    }
      drive(-1,500); // back 
      drive(0,500);  // stop 
      turn(dir,1000);
       // turn 
      drive(0,500); // stop 
}

