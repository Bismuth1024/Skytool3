#ifndef INTERFACE_HEADER_GUARD
#define INTERFACE_HEADER_GUARD

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "misc.h"
#include <stdint.h>
//#include "errorCodes.h"

#define INTERFACE_ACK  0xaa
#define INTERFACE_NACK 0x55
#define INTERFACE_BUFFER_LENGTH 0x100

/*
 A class for communicating with peripherals via protocols such as I2C.  Currently it's only used to communicate with
 the PN532 via an intermediary Arduino nano, but this could be extended in future.  
 
 The computer communicates with an Arduino via USB in my setup, which is then used to control other peripherals via I2C, SPI, etc.
 But any device can be used as long as it follows the below protocol.
 
 
 */


/*
 
 General protocol: Adapter device sends INTERFACE_ACK for good command, INTERFACE_NACK for bad command

I2C protocol: 
Host sends:
	Packet start marker - 0x00, 0xff
	Length of packet
	LCS - should be equal to !length
	number of bytes
	address
	r/w
	(data)
	end

*/

class Interface {
public:
    /*
     Interface: Creates an interface object for the specified device.
     
     portName: name of device, e.g. "/dev/cu.usbserial-AR0KL3OY"
     */
    Interface(std::string portName);
    
    /*
     begin: Opens communication with the device, at the specified baud rate.  Valid baud rates are:
    9600
    19200
    38400
    57600
    115200
    230400
     
     */
    void begin(int baud);
    
    /*
     setDebug: specifies whether or not to print debugging messages.
     */
    void setDebug(bool d = true);
    
    /*
     sendI2C: Send data to an I2C slave device
     
     i2caddr: Address of the slave device
     data: pointer to array of bytes to send
     nBytes: number of bytes to send
     */
    void sendI2C(uint8_t i2caddr, uint8_t* data, uint8_t nBytes);
    
    /*
     receiveI2C: Receive data from an I2C slave device
     
     i2caddr: Address of the slave device
     data: pointer to array of bytes to store the response
     nBytes: number of bytes to receive
     */
    void receiveI2C(uint8_t i2caddr, uint8_t* destination, uint8_t nBytes);
    
private:
    std::string portName; //Name of the device to write to, e.g. "/dev/cu.usbserial-AR0KL3OY"
    int portID; //handle for writing to the device
    int baudrate; //baudrate to use in serial communication with device
    bool debug; //Whether debug messages should be printed
    uint8_t buffer[INTERFACE_BUFFER_LENGTH]; //Buffer for storing I2C data in/out
    
    /*
     send: Send data to the adapter device
     
     data: pointer to array of bytes to send
     nBytes: number of bytes to send
     */
    void send(uint8_t* data, int nBytes);
    
    /*
     Receive: Receive data from the adapter device
     
     data: pointer to array of bytes to receive
     nBytes: number of bytes to receive
     */
    void receive(uint8_t* destination, int nBytes);
    
    /*
     sendAcknowledge: Send data to the adapter device, and check for a returned ACK (exception if no ACK)
     
     data: pointer to array of bytes to send
     nBytes: number of bytes to send
     */
    void sendAcknowledge(uint8_t* data, int nBytes);

    /*
     convertBaud: converts a baud rate into a constant used to set that baud rate for a serial port.
     
     baud: baud rate to convert
     
     return value: constant to be passed to cfsetispeed

     */
    int convertBaud(int baud);
};

#endif



