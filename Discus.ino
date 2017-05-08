

const int analogPin = 0;	//input for thermistor sensor
const int Relay1trig = 12;
const int Relay2trig = 8;
const int R1 = 20000;

int achievedtargettemp;
int htr_on;
int light_on;
int LEDpin = 9;
int hourmemory = 0;
int dawn = 7;
int dusk = 22;
int midday = (dawn + dusk) / 2;
int minPWM = 0;
int maxPWM = 100;
int stepsize = (maxPWM - minPWM) / (midday - dawn);
int PWM;
int hourtest;
int test = 0;

float C;
float raw;
float targettemp = 34.00;
float targettempdelta = 1;
float R2 = 0;
float sumraw;
float avgraw;
float buffer = 0;

#define THERMISTORPIN A0
#define SERIESRESISTOR 20000

/////////////////////////For OLED////////////////////////
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
B00000001, B11000000,
B00000001, B11000000,
B00000011, B11100000,
B11110011, B11100000,
B11111110, B11111000,
B01111110, B11111111,
B00110011, B10011111,
B00011111, B11111100,
B00001101, B01110000,
B00011011, B10100000,
B00111111, B11100000,
B00111111, B11110000,
B01111100, B11110000,
B01110000, B01110000,
B00000000, B00110000
};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
/////////////////////////////////////////////////////////////

////////////////////////////For RTC (DS3231)//////////////////////////
//SDA to A4, SCL to A5

#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
	return ((val / 10 * 16) + (val % 10));
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
	return ((val / 16 * 10) + (val % 16));
}
/////////////////////////////////////////////////////////////


void setup()
{
	Wire.begin();
	Serial.begin(9600);
	
	pinMode(Relay1trig, OUTPUT);
	digitalWrite(Relay1trig, HIGH);
	
	pinMode(Relay2trig, OUTPUT);
	digitalWrite(Relay2trig, HIGH);
	
	analogReference(EXTERNAL);
	
	//pinMode(LEDpin, OUTPUT);
	//digitalWrite(LEDpin, LOW);
	
	//set the initial time here, after setting the time, comment this section
	//DS3231 seconds, minutes, hours, day, date, month, year
	//setDS3231time(00,20,20,7,6,5,17);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();

	//analogWrite(LEDpin, minPWM);
}



////////////////////////////DS3231 coding//////////////////////////
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
	dayOfMonth, byte month, byte year)
{
	// sets time and date data to DS3231
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set next input to start at the seconds register
	Wire.write(decToBcd(second)); // set seconds
	Wire.write(decToBcd(minute)); // set minutes
	Wire.write(decToBcd(hour)); // set hours
	Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
	Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
	Wire.write(decToBcd(month)); // set month
	Wire.write(decToBcd(year)); // set year (0 to 99)
	Wire.endTransmission();
}


void readDS3231time(byte *second,
	byte *minute,
	byte *hour,
	byte *dayOfWeek,
	byte *dayOfMonth,
	byte *month,
	byte *year)
{
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set DS3231 register pointer to 00h
	Wire.endTransmission();
	Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
	// request seven bytes of data from DS3231 starting from register 00h
	*second = bcdToDec(Wire.read() & 0x7f);
	*minute = bcdToDec(Wire.read());
	*hour = bcdToDec(Wire.read() & 0x3f);
	*dayOfWeek = bcdToDec(Wire.read());
	*dayOfMonth = bcdToDec(Wire.read());
	*month = bcdToDec(Wire.read());
	*year = bcdToDec(Wire.read());
}


void displayTime()
{
	byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
	// retrieve data from DS3231
	readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
		&year);
	// send it to the serial monitor
	Serial.print(hour, DEC);
	hourtest = hour - test;
	// convert the byte variable to a decimal number when displayed
	Serial.print(":");
	if (minute < 10)
	{
		Serial.print("0");
	}
	Serial.print(minute, DEC);
	Serial.print(":");
	if (second < 10)
	{
		Serial.print("0");
	}
	Serial.print(second, DEC);
	Serial.print(" ");
	Serial.print(dayOfMonth, DEC);
	Serial.print("/");
	Serial.print(month, DEC);
	Serial.print("/");
	Serial.print(year, DEC);
	Serial.print(" Day of week: ");

	//////////////////////////////////////////////////////////////////

	display.setCursor(50, 0);
	display.print(dayOfMonth, DEC);
	display.print(",");

	display.setCursor(91, 0);
	display.print("20");
	display.print(year, DEC);



	if (hour < 10)
	{
		display.setCursor(40, 9);
		display.print("0");
		display.print(hour, DEC);
		display.print(":");
	}
	if (hour > 9)
	{
		display.setCursor(40, 9);
		display.print(hour, DEC);
		display.print(":");
	}

	if (minute < 10)
	{
		display.setCursor(58, 9);
		display.print("0");
		display.print(minute, DEC);
		display.print(":");
	}

	if (minute > 9)
	{
		display.setCursor(58, 9);
		display.print(minute, DEC);
		display.print(":");
	}

	if (second < 10)
	{
		display.setCursor(75, 9);
		display.print("0");
		display.print(second, DEC);
	}

	if (second > 9)
	{
		display.setCursor(75, 9);
		display.print(second, DEC);
	}

	//////////////////////////////////////////////////////////////////
	//////////////////////////////FOR DAY OF WEEK/////////////////////
	switch (dayOfWeek) {
	case 1:
		Serial.println("Sunday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Sun");

		break;
	case 2:
		Serial.println("Monday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Mon");

		break;
	case 3:
		Serial.println("Tuesday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Tue");

		break;
	case 4:
		Serial.println("Wednesday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Wed");

		break;
	case 5:
		Serial.println("Thursday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Thur");

		break;
	case 6:
		Serial.println("Friday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Fri");

		break;
	case 7:
		Serial.println("Saturday");

		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(20, 0);
		display.print("Sat");

		break;
	}


	//////////////////////////////FOR MONTH/////////////////////
	switch (month)
	{
	case 1:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Jan");

		break;

	case 2:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Feb");

		break;

	case 3:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Mar");

		break;

	case 4:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Apr");

		break;

	case 5:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("May");

		break;

	case 6:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Jun");

		break;

	case 7:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Jul");

		break;

	case 8:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Aug");

		break;

	case 9:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Sep");

		break;

	case 10:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Oct");

		break;

	case 11:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Nov");

		break;

	case 12:
		display.setTextSize(1);
		display.setTextColor(WHITE);
		display.setCursor(70, 0);
		display.print("Dec");

		break;
	}

}


void ReadTempAndHum()
{


	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 20);
	display.print("Temp:");
	display.print(C, 1);
	display.print("C");

	display.setCursor(65, 20);
	display.print("Fed:");
	display.print(1, 01);
	display.print("X");

	display.setCursor(0, 29);
	display.print("Set: ");
	display.print(targettemp, 1);
	display.print("C");

	display.setCursor(65, 29);
	display.print("Light:");
	display.print("ON");

	display.setCursor(0, 38);
	display.print("Dawn:");
	display.print(dawn);

	display.setCursor(65, 38);
	display.print("Htr:");
	if (htr_on == 1)
	{
		display.print("ON");
	}
	else
	{
		display.print("OFF");
	}
	//display.print(1, 01);

	display.setCursor(0, 47);
	display.print("Dusk:");
	display.print(dusk);

	display.setCursor(0, 56);
	display.print("PWM:");
	display.print(PWM);
	//display.print("%");

}

void LEDPWM(int x)
{
	if (x < dawn || x > dusk)
	{
		PWM = minPWM;
	}
	else
	{
		if (x <= midday)
		{
			PWM = ((x - dawn) * stepsize) + minPWM;
		}
		else
		{
			PWM = maxPWM - ((x - midday) * stepsize);
		}
	}
}

void loop()
{
	for (int i = 0; i <= 499; i++)
	{
		raw = analogRead(analogPin);
		sumraw += raw;
		delay(1);
	}
	avgraw = sumraw / 500;
	raw = avgraw;
	sumraw = 0;

	R2 = (1023 / raw) - 1;
	R2 = R1 / R2;

	Serial.println(R2);

	C = (-0.003647 * R2) + 60.517325;
	Serial.print("C: ");
	Serial.println(C);


	if (C < targettemp)
	{
		if (C >= (targettemp - targettempdelta) && achievedtargettemp == 1)
		{
			digitalWrite(Relay1trig, HIGH);
			htr_on = 0;
			delay(10);
		}
		else if (C >= (targettemp - targettempdelta) && achievedtargettemp == 0)
		{
			digitalWrite(Relay1trig, LOW);
			htr_on = 1;
			delay(10);
		}
		else
		{
			digitalWrite(Relay1trig, LOW);
			achievedtargettemp = 0;
			htr_on = 1;
			delay(10);
		}

	}
	else
	{
		digitalWrite(Relay1trig, HIGH);
		achievedtargettemp = 1;
		htr_on = 0;
		delay(10);
	}


	digitalWrite(Relay2trig, LOW);

	displayTime(); // display the real-time clock data on the Serial Monitor,
	ReadTempAndHum();

	
	if (hourtest != hourmemory)
	{
		LEDPWM(hourtest);
		hourmemory = hourtest;
	}
	
	Serial.println(hourtest);
	Serial.println(hourmemory);
	Serial.println(PWM);
	
	
	if (PWM < minPWM)
	{
		PWM = minPWM;
	}
	else if (PWM > maxPWM)
	{
		PWM = maxPWM;
	}
	
	analogWrite(LEDpin, PWM);
	
	delay(490);

	display.display();
	//The portion of the screen that shows the time and date are cleared
	display.fillRect(0, 0, 128, 20, BLACK);
	display.fillRect(0, 0, 56, 80, BLACK);
	display.fillRect(30, 20, 30, 50, BLACK);
	display.fillRect(90, 20, 30, 100, BLACK);

}
