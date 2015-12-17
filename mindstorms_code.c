/*#define GYRO         IN_1
#define LEFT_MOTOR   OUT_C
#define RIGHT_MOTOR  OUT_A
#define MOTORS       OUT_AC
#define OFFSET_SAMPLES 300
#define EMAOFFSET 0.0005

float tInterval;
float gOffset;
float gAngleGlobal = 0;
float motorPos = 0;
long RotationSum = 0, RotationPrev;
long motorDiff;
long RotationDeltaP3 = 0;
long RotationDeltaP2 = 0;
long RotationDeltaP1 = 0;

void GetGyroOffset()
{
  float gSum;
  int  i, gMin, gMax, g;

  ClearScreen();
  TextOut(0, LCD_LINE1, "Groep B - Segway");

  TextOut(0, LCD_LINE4, "Leg Robot neer");
  TextOut(0, LCD_LINE5, "Plat op de grond");
  TextOut(0, LCD_LINE6, "en wacht");

  Off(MOTORS);

  do {
    gSum = 0.0;
    gMin = 1000;
    gMax = -1000;
    for (i=0; i<OFFSET_SAMPLES; i++) {
      g = SensorHTGyro(GYRO);

      if (g > gMax)
        gMax = g;
      if (g < gMin)
        gMin = g;

      gSum += g;
      Wait(5);
    }
  } while ((gMax - gMin) > 1);

  gOffset = gSum / OFFSET_SAMPLES + 1.0;
}


void StartBeeps()
{
  int c;
  ClearScreen();
  TextOut(0, LCD_LINE1, "Groep B - segway");

  TextOut(20, LCD_LINE3, "Starten in");

  for (c=3; c>0;c--) {
    NumOut(47, LCD_LINE4, c);
    PlayTone(440,100);
    Wait(1000);
  }
  NumOut(47, LCD_LINE4, 0);
}


task main()
{
  SetSensorHTGyro(GYRO);
  Wait(50);
  GetGyroOffset();
  StartBeeps();
}

inline void GetGyroData(float &gyroSpeed, float &gyroAngle)
{
  float gyroRaw;

  gyroRaw = SensorHTGyro(GYRO);
  gOffset = EMAOFFSET * gyroRaw + (1-EMAOFFSET) * gOffset;
  gyroSpeed = gyroRaw - gOffset;

  gAngleGlobal += gyroSpeed*tInterval;
  gyroAngle = gAngleGlobal;
}

//---------------------------------------------------------------------

inline void GetMotorData(float &motorSpeed, float &motorPos)
{
  long RotationLeft, RotationRight, RotationDelta;

  RotationLeft = MotorRotationCount(LEFT_MOTOR);
  RotationRight = MotorRotationCount(RIGHT_MOTOR);

  RotationPrev = RotationSum;
  RotationSum = RotationLeft + RotationRight;
  motorDiff = RotationLeft - RotationRight;

  RotationDelta = RotationSum - RotationPrev;
  motorPos += RotationDelta;

  motorSpeed = (RotationDelta+RotationDeltaP1+RotationDeltaP2+RotationDeltaP3)/(4*tInterval);

  RotationDeltaP3 = RotationDeltaP2;
  RotationDeltaP2 = RotationDeltaP1;
  RotationDeltaP1 = RotationDelta;
}

inline void SteerControl(int power, int &powerLeft, int &powerRight)
{
  int powerSteer;

  powerSteer = 0.25 * (0 - motorDiff);

  powerLeft = power + powerSteer;
  powerRight = power - powerSteer;

  if (powerLeft > 100)   powerLeft = 100;
  if (powerLeft < -100)  powerLeft = -100;

  if (powerRight > 100)  powerRight = 100;
  if (powerRight < -100) powerRight = -100;
}

inline void CalcInterval(long cLoop)
{
  long tCalcStart;
  if (cLoop == 0) {
    tInterval = 0.0055;
    tCalcStart = CurrentTick();
  } else {
    tInterval = (CurrentTick() - tCalcStart)/(cLoop*1000.0);
  }
}

task taskBalance()
{
  Follows(main);

  float gyroSpeed, gyroAngle;
  float motorSpeed;

  int power, powerLeft, powerRight;
  long tMotorPosOK;
  long cLoop = 0;

  ClearScreen();
  TextOut(0, LCD_LINE1, "Groep - B");
  TextOut(0, LCD_LINE4, "Balans modus!");

  tMotorPosOK = CurrentTick();

  ResetRotationCount(LEFT_MOTOR);
  ResetRotationCount(RIGHT_MOTOR);

  while(true) {
     CalcInterval(cLoop++);
     GetGyroData(gyroSpeed, gyroAngle);
     GetMotorData(motorSpeed, motorPos);
     power = (gyroSpeed + 8.0 * gyroAngle)/0.8 + 0.05 * motorPos + 0.1 * motorSpeed;

     if (abs(power) < 100)
       tMotorPosOK = CurrentTick();

     SteerControl(power, powerLeft, powerRight);


     OnFwd(LEFT_MOTOR, powerLeft);
     OnFwd(RIGHT_MOTOR, powerRight);

     if ((CurrentTick() - tMotorPosOK) > 1500) {
       break;
     }
     Wait(10);
  }
  Off(MOTORS);
  ClearScreen();
  TextOut(0, LCD_LINE4, "Gevallen");
}

task taskControl()
{
  Follows(main);
  // Wait om eind scherm te tonen.
  Wait(10000);
}
*/