/*
  Author: Ramazan Burak Korkmaz
*/


const int Rpwm2 = 2; // pwm output - motor A- SİYAH
const int Lpwm2 = 3; // pwm output - motor B- BEYAZ
const int MotorRight_R_EN = 4; // Pin to control the clockwise direction of Right Motor- GRi
const int MotorRight_L_EN = 5; // Pin to control the counterclockwise direction of Right Motor- BORDO
const int Rpwm1 = 6; // pwm output - motor- A MAVİ
const int Lpwm1 = 7; // pwm output - motor- B YEŞİL
const int MotorLeft_R_EN = 8; // Pin to control the clockwise direction of Left Motor- TURUNCU
const int MotorLeft_L_EN = 9; // Pin to control the counterclockwise direction of Left Motor- SARI




void SetMotors(int controlCase) {
  switch (controlCase) {
    case 1:
      digitalWrite(MotorRight_R_EN, HIGH);
      digitalWrite(MotorRight_L_EN, HIGH);
      digitalWrite(MotorLeft_R_EN, HIGH);
      digitalWrite(MotorLeft_L_EN, HIGH);
      break;
    case 2:
      digitalWrite(MotorRight_R_EN, LOW);
      digitalWrite(MotorRight_L_EN, LOW);
      digitalWrite(MotorLeft_R_EN, LOW);
      digitalWrite(MotorLeft_L_EN, LOW);
      break;
  }
}// End of SetMotors

// Stop
void stop_Robot() {
  SetMotors(2);
  analogWrite(Rpwm1, 0);
  analogWrite(Lpwm1, 0);
  analogWrite(Rpwm2, 0);
  analogWrite(Lpwm2, 0);
}// End of stop_Robot

//Forward
void Forward(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, mspeed);
  	analogWrite(Lpwm1, 0);
  	analogWrite(Rpwm2, 0);
  	analogWrite(Lpwm2, mspeed);
}// End of Forward

// Backward
void Backward(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, 0);
  	analogWrite(Lpwm1, mspeed);
  	analogWrite(Rpwm2, mspeed);
  	analogWrite(Lpwm2, 0);
}// End of Backward

// LeftTurn
void LeftTurn(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, 0);
  	analogWrite(Lpwm1, mspeed);
  	analogWrite(Rpwm2, 0);
  	analogWrite(Lpwm2, mspeed);
}// End of LeftTurn

// RightTurn
void RightTurn(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, mspeed);
  	analogWrite(Lpwm1, 0);
  	analogWrite(Rpwm2, mspeed);
  	analogWrite(Lpwm2, 0);
}// End of RightTurn

// SlowLeftTurn
void SlowLeftTurn(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, mspeed);
  	analogWrite(Lpwm1, 0);
  	analogWrite(Rpwm2, mspeed);
  	analogWrite(Lpwm2, 0);
}// End of SlowLeftTurn

// SlowRightTurn
void SlowRightTurn(int mspeed){
    SetMotors(1);
	  analogWrite(Rpwm1, 0);
  	analogWrite(Lpwm1, mspeed);
  	analogWrite(Rpwm2, 0);
  	analogWrite(Lpwm2, mspeed);
}// End of SlowRightTurn

// StopCar
void StopCar(){
    SetMotors(1);
	  analogWrite(Rpwm1, 0);
  	analogWrite(Lpwm1, 0);
  	analogWrite(Rpwm2, 0);
  	analogWrite(Lpwm2, 0);
}// End of StopCar

// SetupMotors
void SetupMotors(){
  
  pinMode(MotorRight_R_EN, OUTPUT); //Initiates Motor Channel A1 pin
  pinMode(MotorRight_L_EN, OUTPUT); //Initiates Motor Channel A2 pin
  pinMode(MotorLeft_R_EN, OUTPUT); //Initiates Motor Channel B1 pin
  pinMode(MotorLeft_L_EN, OUTPUT); //Initiates Motor Channel B2 pin

  pinMode(Rpwm1, OUTPUT);
  pinMode(Lpwm1, OUTPUT);
  pinMode(Rpwm2, OUTPUT);
  pinMode(Lpwm2, OUTPUT);
  stop_Robot();
}// End of SetupMotors
