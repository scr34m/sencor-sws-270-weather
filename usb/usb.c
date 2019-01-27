// http://stackoverflow.com/q/26498582
// http://stackoverflow.com/q/15890903

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <errno.h>      // Error number definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <termios.h>    // POSIX terminal control definitions
#include <math.h>
#include <time.h>
#include <stdint.h>

FILE *fp;

typedef struct {
    unsigned int humidity;
    short temp;
    int ts;
} lastreading;

lastreading last;  

/**
 * 8 bit unknown
 * 4 bit 0000 = positive temp, 1111 = negative temp
 * 8 bit temperature
 * 4 bit unknown, always 1111
 * 8 bit humidity
*/
void parse(char *buf, int l)
{
  uint64_t value = 0;

  // ideal is 36
  if (l < 34 || l > 38) {
      printf("error: %d %s\n", l, buf);
      return;
  }

  int i = 0;
  for(i=0;i<l;i++) {
    if (buf[i] == '1') {
      value = value << 1;
      value += 1;
    } else if(buf[i] == '0') {
      value = value << 1;
    } else {
      printf("error: %d %s\n", l, buf);
      return;
    }
  }

  int ts = (int)time(NULL);

  time_t timer;
  char time_s[26];
  struct tm* tm_info;

  time(&timer);
  tm_info = localtime(&timer);

  strftime(time_s, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  unsigned int humidity = ((uint64_t)value & 0xFF);
  unsigned char negative = (((uint64_t)value >> 20) & 0x0F);
  short temp = (((uint64_t)value >> 12) & 0xFFF);
  if (negative == 0xf) {
    temp = 0xf000 + temp;
  }

  printf("time: %s temp: %2.1f humid: %d stream: %s", time_s, (float) temp / 10, humidity, buf);

  // wrong readings!?
  if (humidity < 20 || humidity > 100) {
    printf(" -- WRONG HUMIDITY\n");
    return;
  }

  if (ts - last.ts <= 1 && last.temp == temp && last.humidity == humidity) {
    printf(" -- REPEAT\n");
    return;
  }

  printf(" -- OK\n");

  last.ts = ts;
  last.temp = temp;
  last.humidity = humidity;

  fprintf(fp, "time: %d temp: %2.1f humid: %d\n", ts, (float) temp / 10, humidity);
  fflush(fp);
}

int process_buffer(char *buffer, int length)
{
  char *p1, *p2;
  int processed = 0;     
  char buf[256];
  
  p1 = buffer;
  while( (p2 = strchr(p1,'\n')) != NULL )
  {
    *p2 = '\0';
    strncpy(buf, p1, p2 - p1 - 1); // skip LF
    parse(buf, p2 - p1 - 1);
    processed += (p2 - p1) + 1;
    p1 = ++p2;
  }

  return processed;
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    perror("No device specified");
    return -1;
  }

  fp = fopen(argv[2], "a");
  if(fp==NULL) {
    perror("Error opening file.");
    return -1;
  }

  printf("Opening device... ");
  int USB = open(argv[1], O_RDONLY | O_NOCTTY | O_NONBLOCK);
  printf("opened.\n");

  fcntl(USB, F_SETFL, 0); // set non block io

  struct termios tty;
  memset (&tty, 0, sizeof tty);

  /* Error Handling */
  if (tcgetattr(USB, &tty) != 0) {
    printf("Error %d from tcgetattr: %s!\n", errno, strerror(errno));
    return -1;
  }

  /* Make raw */
  cfmakeraw(&tty);

  /* Set Baud Rate */
  cfsetospeed (&tty, (speed_t)B9600);
  cfsetispeed (&tty, (speed_t)B9600);

  /* Setting other Port Stuff */
  tty.c_cflag |= (CLOCAL | CREAD);   // Enable the receiver and set local mode
  tty.c_cflag &= ~CSTOPB;            // 1 stop bit
  tty.c_cflag &= ~CRTSCTS;           // Disable hardware flow control

  tty.c_cc[VMIN]  = 1;
  tty.c_cc[VTIME] = 2;

  /* Flush Port, then applies attributes */
  tcflush(USB, TCIOFLUSH);
  if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
    printf("Error %d from tcgetattr: %s!\n", errno, strerror(errno));
    return -1;
  }

  fd_set rfds;
  struct timeval to;
  to.tv_sec = 5;
  to.tv_usec = 0;
  
  FD_ZERO(&rfds);
  FD_SET(USB, &rfds);

  int n = 0;
  char tmp_buffer[16];
  int i;
  char buf[256];
  char buf2[256];
  int buf_p = 0;
  char *p1, *p2;

  do {
    memset(&tmp_buffer, '\0', sizeof tmp_buffer);
    int ret = select(USB+1, &rfds, NULL, NULL, &to);
    if (ret == -1) {
      perror("select");
    }

    // mabye only 32 byte readed from the socket (USB / serial internal buffer?)
    n = read(USB, &tmp_buffer, sizeof tmp_buffer);
    if (n > 0) {
      for(i=0;i<n;i++) {
        buf[buf_p+i]=tmp_buffer[i];
      }
      buf_p += n;
      int p = process_buffer(buf, buf_p);
      if (p < buf_p) {
        for(i=p;i<buf_p;i++) {
          buf[i - p]=buf[i];
        }
        buf_p = buf_p - p;
      } else {
        memset(&buf, '\0', sizeof buf);
        buf_p = 0;
      }
    } else {
	     printf("Error %d from read: %s!\n", errno, strerror(errno));
    }
    usleep(100 * 1000);
  } while(1);

  return 0;
}
