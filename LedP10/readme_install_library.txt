LedP10 Arduino Library Developed by: Deepak Agarwal and Devesh Samaiya at BITS Pilani, India
First release date - 30th April 2017, Sunday
Contact - Leave your comments at https://goo.gl/5Ktc2a

Instructions:

Copy the folder <LedP10> at C:\Program Files (x86)\Arduino\libraries

include it from sketch menu or by writing #include<LedP10.h>
Also include "TimerOne.h" and "SPI.h" libraries.

#include "TimerOne.h"
#include"SPI.h"

first call the function 
	<void init(int A, int B, int storepin, int oe, int no_of_panels)>
to customize the control pins, see the pinout for led p10 in the same folder.
Brightness is controlled by oe pin, it should be a PWM pin on board. For example pin 9 and 10 are PWM pins on arduino UNO and hence pin 9 is default oe pin.
SCLK is the storepin, which can be any digital pin.


clk and data pin are SPI clock and MOSI pin of arduino board. For arduino UNO these pins are pin 13 and 11 respectivley. For other board these pins may be different and should be connected accordingly.

clk- SPI clock pin
DATA- SPI MOSI pin.

default values if init function is not called are-
A= 3
B=4
storepin=8
oe=9
no_of_panels= 1


After initialization 5 functions are available-
	void showmsg_single_scroll(char msg[], int no_of_times,int speed, int font);
	void showmsg_double_scroll(char msg1_[],char msg2_[],int no_of_times1,int no_of_times2, int 	speed1_,int speed2, int font_);
	void showmsg_single_static(char msg_[],int font_);
	void showmsg_double_static(char msg1_[],char msg2_[],int font_);
	void setbrightness(uint8_t brightness_);

   	First function-showmsg_single_scroll is for scrolling of single message on P10 panel with 4 arguments which are message, no. of times of message scrolling, speed and font.
    	for message, pointer to a character string can be passed. For continuously running the message, INF can be passed in second argument, spped can be in range of 0-30, 
    	and font can be 0 or 1 as two type of fonts are currently supported for full screen messages. 
   
   	For second function-showmsg_double_scroll, there would be 7 arguments, 2 for messages, two for no. of times of message scrolling, two for speed, and last one for font. 
   	Specification being same as for single scrolling function, except only one font id provided for double scrolling and it's value should be 0.

   	Showmsg_single_static function takes two arguments
  	first argument can be an integer value or a pointer to character string or a string in double quotes.
  	second argument is to select font, for which rwo values i.e. 0 and 1 are supported

 	Function showmsg_double_static  takes three arguments, first two can be either pointer to string or integer independently. Third argument is to select font for which only 
  	one value is supported currently i.e. 0.
 
	Function setbrightness is to change the brightness of display and takes an argument of type uint8_t, it's value can be from 0 to 255. 255 is highest brightness, which is default 		brighness also and 0 is	lowest(off).
	
	Brightness is controlled by oe pin, it should be a PWM pin on board. For example pin 9 and 10 are PWM pins on arduino UNO and hence pin 9 is default oe pin.