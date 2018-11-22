PseudoGPIO is based on a simple protocol based on the exchange of single-byte packets through a serial port.

## General Packet Format
The protocol allows for 8 different commands, encoded in the 3 higher-order bits of the packet. The remaining bits shall be interpreted differently, according to the command being issued.
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="3">Command</td>
    <td colspan="5">[Command-Dependent]</td>
  </tr>
</table>


## Commands
No reply from the target shall be expected for any command, except where indicated.

Commands that do not provide a reply shall be assumed to have been executed correctly.

### Hello
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="1">0</td>
    <td colspan="1">0</td>
    <td colspan="1">0</td>
    <td colspan="5">Don't Care</td>
  </tr>
</table>

This command checks if the target board is alive and interpreting commands correctly. It also allows to retrieve the protocol version implemented by the board. The reply shall be a single byte with the following format:
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="1">1</td>
    <td colspan="1">1</td>
    <td colspan="1">0</td>
    <td colspan="1">1</td>
    <td colspan="4">Protocol Version</td>
  </tr>
</table>

Currently, only version 1 of the protocol is defined.

### PinMode
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="1">0</td>
    <td colspan="1">0</td>
    <td colspan="1">1</td>
    <td colspan="4">Pseudopin #</td>
    <td colspan="1">Mode</td>
  </tr>
</table>

This command sets the mode of a pseudopin. `Mode` shall be interpreted as follows:

* 0: Input
* 1: Output

### DigitalRead
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="1">0</td>
    <td colspan="1">1</td>
    <td colspan="1">0</td>
    <td colspan="4">Pseudopin #</td>
    <td colspan="1">Don't Care</td>
  </tr>
</table>

This command requests the status of a digital pseudopin. The reply is a single byte, of which only the LSB shall be interpreted as follows:

* 0: Low Level
* 1: High Level

### DigitalWrite
<table>
  <tr align="center">
    <th>Bit</th>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr align="center">
    <th>Meaning</th>
    <td colspan="1">0</td>
    <td colspan="1">0</td>
    <td colspan="1">1</td>
    <td colspan="4">Pseudopin #</td>
    <td colspan="1">Level</td>
  </tr>
</table>

This command sets the level of a digital pseudopin. `Level` shall be interpreted as in the `DigitalRead` command, i.e.:

* 0: Low Level
* 1: High Level

## Pseudopin Numbers
All commands involving a pseudopin number expect an integer in the range 0-15, which is generally mapped to the correspondingly numbered physical pin on the target board, but there might be exceptions depending on the particular target.

* Arduino Uno/Nano and other ATmega328-based Arduinos: Pseudopins are mapped to the correspondingly numbered pins
  * Pins 0 and 1 are used by the serial port and thus are not available
  * Pins 14 and 15 do not exist
    * Thus pseudopins 0/1/14/15 have no meaning on these boards
* STM32F103xx-based boards, such as Blue Pill, Black Pill, etc: Pseudopins 0-15 are mapped to physical pins PA0-PA15, with the following exceptions:
  * PA11 and PA12 are used by the USB connection, thus pseudopins 11 and 12 are mapped to PB11 and PB12
    * Note that PB12 drives the onboard led on the Black Pill (with inverted logic)
  * PA13 and PA14 are only available through the SWD header on the Black Pill

