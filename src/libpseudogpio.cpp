#include <cstdio>
#include <cstring>
#include <cerrno>   /* ERROR Number Definitions          */
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions         */
#include "libpseudogpio.h"

uint8_t PseudoGPIO::makePGByte (const PgCommand cmd, const uint8_t pinNo, const uint8_t arg) {
	return (static_cast<uint8_t> (cmd) << 5U) | ((pinNo & 0x0F) << 1U) | (arg & 0x01);
}

void PseudoGPIO::pinMode (const uint8_t pinNo, const PgPinMode mode) {
	uint8_t b = makePGByte (PGCMD_PINMODE, pinNo, static_cast<uint8_t> (mode));

	if (write (fd, &b, 1) != 1) {
		fprintf (stderr, "pinMode failed\n");
	}
}

void PseudoGPIO::digitalWrite (const uint8_t pinNo, const PgPinLevel level) {
	uint8_t b = makePGByte (PGCMD_DIGITALWRITE, pinNo, static_cast<uint8_t> (level));

	if (write (fd, &b, 1) != 1) {
		fprintf (stderr, "digitalWrite failed\n");
	}
}

PseudoGPIO::PgPinLevel PseudoGPIO::digitalRead (const uint8_t pinNo) {
	PgPinLevel ret = PGPL_LOW;		// ...

	uint8_t b = makePGByte (PGCMD_DIGITALREAD, pinNo);

	if (write (fd, &b, 1) != 1) {
		fprintf (stderr, "digitalRead failed\n");
	} else if (read (fd, &b, 1) != 1) {
		// Well, this can't really happen since we have an infinite timeout
		fprintf (stderr, "No data from digitalRead\n");
	} else {
		ret = (b != 0 ? PGPL_HIGH : PGPL_LOW);
	}

	return ret;
}


/******************************************************************************/


PseudoGPIO::PseudoGPIO (): fd (-1) {
}

PseudoGPIO::~PseudoGPIO() {
	if (fd >= 0)
		close (fd);
}

bool PseudoGPIO::begin (const char* portdev, const unsigned int speed) {
	bool ret = false;

	fd = open (portdev, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		fprintf (stderr, "Cannot open serial port %s\n", portdev);
	} else {
#if 0
		settings.c_cflag &= ~PARENB;   		// No Parity
		settings.c_cflag &= ~CSTOPB; 		// Stop bits = 1
		settings.c_cflag &= ~CSIZE;			// Clears the Mask
		settings.c_cflag &= ~CRTSCTS;		// Turn off hardware based flow control (RTS/CTS).
		settings.c_cc[VMIN]  = 1;			// Read single bytes
		settings.c_iflag &= ~(IXON | IXOFF | IXANY);			// Turn off software based flow control (XON/XOFF).
		//~ settings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);	// NON Cannonical mode
#endif
		struct termios oldtio, newtio;

		tcgetattr (fd, &oldtio); /* save current port settings */

        memset (&newtio, 0x00, sizeof (newtio));

		// Start setting speed - Shamelessly stolen from Boost
		speed_t baud = B0;
		switch (speed) {
			// Do POSIX-specified rates first.
			case 0: baud = B0; break;
			case 50: baud = B50; break;
			case 75: baud = B75; break;
			case 110: baud = B110; break;
			case 134: baud = B134; break;
			case 150: baud = B150; break;
			case 200: baud = B200; break;
			case 300: baud = B300; break;
			case 600: baud = B600; break;
			case 1200: baud = B1200; break;
			case 1800: baud = B1800; break;
			case 2400: baud = B2400; break;
			case 4800: baud = B4800; break;
			case 9600: baud = B9600; break;
			case 19200: baud = B19200; break;
			case 38400: baud = B38400; break;
			// And now the extended ones conditionally.
			# ifdef B7200
			case 7200: baud = B7200; break;
			# endif
			# ifdef B14400
			case 14400: baud = B14400; break;
			# endif
			# ifdef B57600
			case 57600: baud = B57600; break;
			# endif
			# ifdef B115200
			case 115200: baud = B115200; break;
			# endif
			# ifdef B230400
			case 230400: baud = B230400; break;
			# endif
			# ifdef B460800
			case 460800: baud = B460800; break;
			# endif
			# ifdef B500000
			case 500000: baud = B500000; break;
			# endif
			# ifdef B576000
			case 576000: baud = B576000; break;
			# endif
			# ifdef B921600
			case 921600: baud = B921600; break;
			# endif
			# ifdef B1000000
			case 1000000: baud = B1000000; break;
			# endif
			# ifdef B1152000
			case 1152000: baud = B1152000; break;
			# endif
			# ifdef B2000000
			case 2000000: baud = B2000000; break;
			# endif
			# ifdef B3000000
			case 3000000: baud = B3000000; break;
			# endif
			# ifdef B3500000
			case 3500000: baud = B3500000; break;
			# endif
			# ifdef B4000000
			case 4000000: baud = B4000000; break;
			# endif
			default:
				/* Bad speed - Since B0 is not a real baud speed (it just instructs
				 * the modem to "hang up"), we can use it here for a small kludge
				 */
				baud = B0;
				break;
		}

		if (baud != B0) {
			newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;
			newtio.c_iflag = IGNPAR;
			newtio.c_oflag = 0;

			/* set input mode (non-canonical, no echo,...) */
			newtio.c_lflag = 0;

			newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
			newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

			tcflush(fd, TCIFLUSH);
			tcsetattr(fd,TCSANOW,&newtio);

			if (tcsetattr (fd, TCSANOW, &newtio) != 0) {
				fprintf (stderr, "Cannot set serial port parameters");
			} else {
				ret = true;
			}
		}
	}

	return ret;
}
