/* $LastChangedDate: 2015-08-10 08:53:54 -0700 (Mon, 10 Aug 2015) $ */
/* $Rev: 58 $      $Author: peters $ */

#include "serial.h"

/*Open serial port at a named port and speed option */
int serial_open (int device, int speed) 
{
	struct termios t;

	if ((serial_fd[device] = open(serial_dev[device], O_RDWR|O_NOCTTY|O_NONBLOCK )) < 0) 
	{
	  sprintf (serial_errorstr, "serial_open :: Failed to open %s.", serial_dev[device]);
	  return _ERROR;
	}

	//set port transfer mode
	fcntl(serial_fd[device], F_SETOWN, getpid());
	fcntl(serial_fd[device], F_SETFL, 0);
	
	bzero (&t, sizeof (t));
	t.c_cc [VMIN] = 1; 
	t.c_cc [VTIME] = 5;
	t.c_cflag &= ~PARENB;
	t.c_cflag &= ~CSTOPB;
	t.c_cflag &= ~CRTSCTS;
	t.c_cflag &= ~CSIZE;
	t.c_cflag |= (CS8 | CLOCAL | CREAD);
	t.c_iflag &= ~(IXON | IXOFF);
	t.c_iflag |= IGNPAR;
	t.c_oflag=0;
	t.c_lflag=0;

	if(speed==9600)
	  {
	    // Copy input and output speeds to struct termios t. //
	    if (cfsetispeed (&t, B9600) < 0)
	      SERIAL_ERROR ("serial_open :: Error setting serial comm input speed.")
	    if (cfsetospeed (&t, B9600) < 0)
	      SERIAL_ERROR ("serial_open :: Error setting serial comm output speed.")
	  }
	else // just assume 115200
	  {
	    if (cfsetispeed (&t, B115200) < 0)
	      SERIAL_ERROR ("serial_open :: Error setting serial comm input speed.")
	    if (cfsetospeed (&t, B115200) < 0)
	      SERIAL_ERROR ("serial_open :: Error setting serial comm output speed.")
	  }
	// Throw away any input data (noise). //
	if (tcflush (serial_fd[device], TCIFLUSH) < 0)
		SERIAL_ERROR ("serial_open :: Error executing serial serial flush.")
				
	// Now set the terminal port attributes. //
	if (tcsetattr (serial_fd[device], TCSANOW, &t) < 0)
		SERIAL_ERROR ("serial_open :: Error setting serial terminal attributes.")

	Timeout.tv_usec = 5000000;
	Timeout.tv_sec  = 0;

	FD_SET(serial_fd[device], &readfs);
	return _NO_ERROR;
}


int serial_close (int device) 
{
	// If there's an error while closing. //
	if (close (serial_fd[device]) < 0)
		SERIAL_ERROR ("serial_close :: Error closing the serial file descriptor.")
	return _NO_ERROR;
}
