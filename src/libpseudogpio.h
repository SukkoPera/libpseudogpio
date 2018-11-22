#ifndef _LIBPSEUDOGPIO_H_
#define _LIBPSEUDOGPIO_H_

#include <cstdint>

class PseudoGPIO {
public:
	enum PgCommand {
		PGCMD_HELLO = 0,
		PGCMD_PINMODE = 1,
		PGCMD_DIGITALREAD = 2,
		PGCMD_DIGITALWRITE = 3,
		PGCMD_ANALOGREAD = 4,	// Future extension
		PGCMD_ANALOGWRITE = 5,	// Ditto, plus arg won't fit in 1 bit
	};

	enum PgPinMode {
		PGPM_INPUT = 0,
		PGPM_OUTPUT = 1
	};

	enum PgPinLevel {
		PGPL_LOW = 0,
		PGPL_HIGH = 1
	};

	static const unsigned int DEFAULT_SPEED = 115200;

	PseudoGPIO ();
	~PseudoGPIO ();

	bool begin (const char* portdev, const unsigned int speed = DEFAULT_SPEED);

	void pinMode (const uint8_t pinNo, const PgPinMode mode);

	void digitalWrite (const uint8_t pinNo, const PgPinLevel level);

	PgPinLevel digitalRead (const uint8_t pinNo);

private:
	int fd;		//!< Serial port descriptor

	uint8_t makePGByte (const PgCommand cmd, const uint8_t pinNo, const uint8_t arg = 0);

};

#endif
