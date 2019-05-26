#include <SPI.h>
#include <TimerOne.h>
#include<EEPROM.h>

#include "DMD.h"
#include "MyBigFont.h"
const int buzz = A0;

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

char scrolldata[60];
int  uc_i;

void clear_scrolldata()
{
  unsigned int uc;
  for (uc = 0; uc < 120; uc++)
    scrolldata[uc] = '\0';
}



void clearSerialBuffer(void)       // maximum length of serial buffer is 256 characters
{
  while ( Serial.available() > 0 )
  {
    Serial.read();
  }
}
/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
  --------------------------------------------------------------------------------------*/
char gsm[64], irq_gsm = 0;
char msg[64], mobilenum[11] = "9396671541";

void gsm_init(void);
char gsm_clear_msg(char num);
void gsm_send_num_mesg1(char *num, char *msg1);
void read_msg();


void read_gsm(void)
{
  while (Serial.available())
  {
    gsm[irq_gsm] = Serial.read();
    irq_gsm++;
    if (irq_gsm > 62)
      irq_gsm = 63;
  }
}



void ScanDMD()
{
  dmd.scanDisplayBySPI();
}




/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
  --------------------------------------------------------------------------------------*/
void setup(void)
{
  pinMode(buzz, OUTPUT);
  digitalWrite(buzz, LOW);

  //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 1000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
 dmd.selectFont(MyBigFont);
  //dmd.drawString(0,  0, "       WELCOME", strlen("       WELCOME"), GRAPHICS_NORMAL );
  dmd.drawString(0,  0, "LED BOARD", strlen("LED BOARD"), GRAPHICS_NORMAL ); delay(3000);

  Serial.begin(9600); delay(100);
  dmd.clearScreen( true );
  gsm_init();
  gsm_clear_msg(1);
  gsm_clear_msg(2);

  digitalWrite(buzz, HIGH);
}


/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
  --------------------------------------------------------------------------------------*/
void loop(void)
{

  unsigned char e_count;
  unsigned int uc_i = 0;
  int i;

  long start = millis();
  long timer = start;
  boolean ret = false;

  dmd.clearScreen( true );
  dmd.drawString(0,  0, "WELCOME", strlen("WELCOME"), GRAPHICS_NORMAL );  delay(2000);


  for (uc_i = 0; uc_i < 63; uc_i++)
  {
    scrolldata[uc_i] = EEPROM.read(uc_i);
  }


  dmd.clearScreen( true );

  while (1)
  {


    if (Serial.available() > 0)
    {
      read_gsm();
      Serial.print("AT+CMGR=1"); irq_gsm = 0;  Serial.println(); delay(500);

      read_gsm();

      if (gsm[2] == '+' && gsm[3] == 'C' && gsm[4] == 'M' && gsm[5] == 'G' && gsm[6] == 'R' && gsm[7] == ':')
      {
        dmd.clearScreen( true );
        dmd.drawString(0,  0, "                        ", strlen("                         "), GRAPHICS_NORMAL );  delay(1000);
        digitalWrite(buzz, LOW);
        dmd.drawString(0,  0, "NEW MSG", strlen("NEW MSG"), GRAPHICS_NORMAL );  delay(2000);

        read_msg();
        digitalWrite(buzz, HIGH);
        irq_gsm = 0;
        {
          gsm_send_num_mesg1(mobilenum, (char *)"MSG Updated successfully");
          clear_scrolldata();
          strcpy(scrolldata, msg);

          strcat(scrolldata, " ");
          dmd.clearScreen( true );
          dmd.drawMarquee(scrolldata, strlen(scrolldata), (32 * DISPLAYS_ACROSS) - 1, 0);


          for (uc_i = 0; (scrolldata[uc_i] != '\0' && uc_i < 64); uc_i++)
          {
            EEPROM.write(uc_i, scrolldata[uc_i]); delay(50);
          }

          //strcat(scrolldata, msg2);
          strcat(scrolldata, " ");
          dmd.clearScreen( true );
          dmd.drawMarquee(scrolldata, strlen(scrolldata), (32 * DISPLAYS_ACROSS) - 1, 0);

        }
      }
      else
      {
        read_gsm(); delay(500);  irq_gsm = 0;
      }
    }


    dmd.drawMarquee(scrolldata, strlen(scrolldata), (32 * DISPLAYS_ACROSS) - 1, 0);

    ret = false;

    while (!ret)
    {
      if ((timer + 60) < millis())
      {
        ret = dmd.stepMarquee(-1, 0);
        timer = millis();
      }
    }



  }
}


void gsm_init(void)
{
  unsigned int ui_t1;
  char E_count = 0;

gsminit:
  dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
  dmd.drawString(0,  0, "GSM INIT..", strlen("GSM INIT.."), GRAPHICS_NORMAL);
  Serial.print("ATE0"); irq_gsm = 0; Serial.println(); delay(500);

  read_gsm();  E_count = 0;
AT:
  delay(100);
  Serial.print("AT"); irq_gsm = 0; Serial.println(); delay(500);
  read_gsm();

  if (gsm[2] == 'O' && gsm[3] == 'K')
  {
  }
  else
  {
    E_count++;
   dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "gsm not c", strlen("gsm not c"), GRAPHICS_NORMAL); delay(500);
    if (E_count > 9)
    {
      E_count = 0;
      goto gsminit;
    }
    goto AT;
  }


  read_gsm(); E_count = 0;
ATCPIN:
  Serial.print("AT+CPIN?");  Serial.println();  irq_gsm = 0;  delay(500);
  read_gsm();

  if (gsm[2] == '+' && gsm[3] == 'C' && gsm[4] == 'P' && gsm[5] == 'I' && gsm[6] == 'N' && gsm[7] == ':' && gsm[8] == ' ' && gsm[9] == 'R')
  {
  }
  else
  {
    E_count++;
    dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "sim check", strlen("sim check"), GRAPHICS_NORMAL); delay(800);
    if (E_count > 20)
    {
      E_count = 0;
      goto gsminit;
    }
    goto ATCPIN;
  }


  read_gsm();  E_count = 0;
ATCMGF:
  Serial.print("AT+CMGF=1"); irq_gsm = 0;  Serial.println(); delay(500);
  read_gsm();

  if (gsm[2] == 'O' && gsm[3] == 'K')
  {
  }
  else
  {
    E_count++;
    dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "TEXT erro", strlen("TEXT erro"), GRAPHICS_NORMAL); delay(800);

    if (E_count > 20)
    {
      E_count = 0;
      goto gsminit;
    }
    goto ATCMGF;
  }


  read_gsm(); E_count = 0;
ATCNMI:
  Serial.print("AT+CNMI=2,1"); irq_gsm = 0; Serial.println(); delay(500);
  read_gsm();

  if (gsm[2] == 'O' && gsm[3] == 'K')
  {
  }
  else
  {
    E_count++;
    dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "SMSS erro", strlen("SMSS erro"), GRAPHICS_NORMAL); delay(800);
    if (E_count > 20)
    {
      E_count = 0;
      goto gsminit;
    }
    goto ATCNMI;
  }




  read_gsm(); E_count = 0;
ATW:
  Serial.print("AT&W"); irq_gsm = 0; Serial.println();  delay(500);
  read_gsm();

  if (gsm[2] == 'O' && gsm[3] == 'K')
  {
  }
  else
  {
    E_count++;
    dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "AT&T erro", strlen("AT&T erro"), GRAPHICS_NORMAL); delay(800);
    if (E_count > 9)
    {
      E_count = 0;
      goto gsminit;
    }
    goto ATW;
  }
}




char gsm_clear_msg(char num)
{
  char E_count = 0;

  E_count = 0;
ATCMGD:
  Serial.print("AT+CMGD="); Serial.print(num, DEC);     irq_gsm = 0;  Serial.println(); delay(500);
  read_gsm();

  if (gsm[2] == 'O' && gsm[3] == 'K')
  {
  }
  else
  {
    E_count++;
    dmd.drawString(0,  0, "                          ", strlen("                          "), GRAPHICS_NORMAL);
    dmd.drawString(0,  0, "DEL error", strlen("DEL error"), GRAPHICS_NORMAL); delay(800);
    if (E_count > 50)
    {
      E_count = 0;
      return (0);
    }
    goto ATCMGD;
  }
  return (1);
}

void gsm_send_num_mesg1(char *num, char *msg1)
{
  Serial.print("AT+CMGS="); Serial.print('"');  Serial.print((String)num);  Serial.print('"');  Serial.println("");  delay(2000);
  Serial.print((String)msg1);  Serial.print("  ");   delay(500);
  Serial.write(0x1A); delay(500);
  //read_gsm();
  //while (!Serial.available());
  delay(2000);
  irq_gsm = 0;
  read_gsm();
  Serial.println(); delay(200);
}



void read_msg()
{

  Serial.println("AT+CMGR=1");
  read_gsm();
  if (gsm[2] == '+' && gsm[3] == 'C' && gsm[4] == 'M' && gsm[5] == 'G' && gsm[6] == 'R' && gsm[7] == ':')
  {
    for (char i = 20; i < 30; i++)
    {
      mobilenum[i - 20] = gsm[i];
    }
    for (char i = 30; mobilenum[i] != '\n'; i++)
    {
      msg[i - 30] = gsm[i];
    }

  }

  mobilenum[10] = '\0';
  delay(500);
}
