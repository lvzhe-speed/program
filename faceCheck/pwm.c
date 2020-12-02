#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
int pitchPwmPin=18;
int yawPwmPin=13;

int main()
{
		//setup();
	int Steer_interval=50;
	int direction=1;
	wiringPiSetupGpio();
	pinMode(pitchPwmPin,PWM_OUTPUT);
	pinMode(yawPwmPin,PWM_OUTPUT);
	 pwmSetMode(PWM_MODE_MS);

	pwmSetClock(192);
	pwmSetRange(2000);
	while(1)
	{
		pwmWrite(yawPwmPin,Steer_interval);
		pwmWrite(pitchPwmPin,50);
		if(direction)
		{
			Steer_interval+=10;
			delay(500);
			if(Steer_interval>=250){
					direction=0;
			}
		}
		else
		{
			Steer_interval-=10;
			delay(500);
			if(Steer_interval<=50)direction=1;
		}
     }

	return 0;

}
