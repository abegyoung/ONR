
#include "serial.h"

/*Open serial port*/
int serial_open (void) 
{
	struct termios t;
	// Open a file descriptor to the serial port //
	if ((serial_fd = open (SERIAL_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK )) < 0) 
	{
		sprintf (serial_errorstr, "serial_open :: Failed to open the serial device %s.", SERIAL_DEV);
		return _ERROR;
	}

	//set port transfer mode
	fcntl(serial_fd, F_SETOWN, getpid());
	fcntl(serial_fd, F_SETFL, 0);
	
	bzero (&t, sizeof (t));
	t.c_cc [VMIN] = 0; 
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
	
	// Copy input and output speeds to struct termios t. //
	if (cfsetispeed (&t, B115200) < 0)
		SERIAL_ERROR ("serial_open :: Error setting serial comm input speed.")
		
	if (cfsetospeed (&t, B115200) < 0)
		SERIAL_ERROR ("serial_open :: Error setting serial comm output speed.")
			
	// Throw away any input data (noise). //
	if (tcflush (serial_fd, TCIFLUSH) < 0)
		SERIAL_ERROR ("serial_open :: Error executing serial serial flush.")
				
	// Now set the terminal port attributes. //
	if (tcsetattr (serial_fd, TCSANOW, &t) < 0)
		SERIAL_ERROR ("serial_open :: Error setting serial terminal attributes.")

	maxfd = MAX2(serial_fd, serial_fd)+1;
	Timeout.tv_usec = 5000000;
	Timeout.tv_sec  = 0;

	FD_SET(serial_fd, &readfs);

	return _NO_ERROR;
}



/*----------------------------------------------------------------*
 |  function: serial_close
 |  purpose : Close the connection to the serial control system.
 */
int serial_close (void) 
{
	// If there's an error while closing. //
	if (close (serial_fd) < 0)
		SERIAL_ERROR ("serial_close :: Error closing the serial file descriptor.")
		
		return _NO_ERROR;
}
