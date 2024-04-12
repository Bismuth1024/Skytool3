#include "Interface.h"


Interface::Interface(std::string portName) : debug(false), portName(portName), baudrate(0), portID(-1) {
	memset(buffer, 0, INTERFACE_BUFFER_LENGTH);
}
	
void Interface::begin(int baud) {
			
	//Convert baudrate
	if (convertBaud(baud) == -1) {
        throw CodedException(0x03);
	} else {
		baudrate = convertBaud(baud);
	}
	

	int port = open(portName.c_str(), O_RDWR); //Open port
	
	if (port < 0) { //Handle errors
   		std::cout << "Error " << errno << " from opening port: " << strerror(errno) << std::endl;
        throw CodedException(0x04);
	}
	
	struct termios tty; //For configuring port
	
	if(tcgetattr(port, &tty) != 0) { //Handle errors
    	std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
        throw CodedException(0x4);
	}
	
	tty.c_cflag &= ~PARENB; //Clear parity bit
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
	tty.c_cflag &= ~CSIZE; // Clear all the size bits before setting
	tty.c_cflag |= CS8; // 8 bits per byte
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON; //Disable canonical mode
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); //Receive raw data
	
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	
	tty.c_cc[VTIME] = 20;    // Wait for up to 2s (20 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;
	
	cfsetispeed(&tty, baudrate); //Set baud rate
	cfsetospeed(&tty, baudrate);
	
	if (tcsetattr(port, TCSANOW, &tty) != 0) { //Save and handle errors
    	std::cout << "Error " << errno << " setting port attributes: " << strerror(errno) << std::endl;
        throw CodedException(0x04);
	}
	
	portID = port;
}


int Interface::convertBaud(int baud)
{
    switch (baud) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    default: 
        return -1;
    }
}

void Interface::sendI2C(uint8_t i2caddr, uint8_t* data, uint8_t nBytes) {
	if (portID < 0) throw CodedException(0x05);
	if (nBytes > INTERFACE_BUFFER_LENGTH - 7) throw CodedException(0x06);
	
	buffer[0] = 0x00; //start marker 1
	buffer[1] = 0xff; //start marker 2
	buffer[2] = nBytes + 3; //total length of packet
	buffer[3] = ~(buffer[2]) + 1; // length checksum
	buffer[4] = nBytes; // length
	buffer[5] = i2caddr; // i2c address
	buffer[6] = 0x00; //read
	std::memcpy(buffer + 7, data, nBytes);
	
	if (debug) {
        std::cout << "Sending " << std::dec << +nBytes << " bytes to I2C address 0x" << std::hex << std::setfill('0') << std::setw(2) << +i2caddr << ": " << std::endl;
        printHexBytes(data, nBytes, -1);
	}
	
    sendAcknowledge(buffer, nBytes + 7);
}

void Interface::receiveI2C(uint8_t i2caddr, uint8_t* destination, uint8_t nBytes) {
    if (portID < 0) throw CodedException(0x05);

	buffer[0] = 0x00;
	buffer[1] = 0xff;
	buffer[2] = 3;
	buffer[3] = ~(buffer[2]) + 1;
	buffer[4] = nBytes;
	buffer[5] = i2caddr;
	buffer[6] = 0x01;


	if (debug) {
        std::cout << "Requesting " << std::dec << +nBytes << " bytes from I2C address 0x" << std::hex << std::setfill('0') << std::setw(2) << +i2caddr << ": " << std::endl;	}
	
	sendAcknowledge(buffer, 7);
	
    receive(destination, nBytes);
}



void Interface::setDebug(bool d) {
	debug = d;
}

void Interface::send(uint8_t* data, int nBytes) {
    if (portID < 0) throw CodedException(0x05);
	write(portID, data, nBytes);
	
	if (debug) {
        std::cout << "Sending " << std::dec << nBytes << " bytes:" << std::endl;
		printHexBytes(data, nBytes, -1);
	}
}


void Interface::receive(uint8_t* destination, int nBytes) {
    if (portID < 0) throw CodedException(0x05);
    ssize_t n = read(portID, destination, nBytes);

    if (debug) {
        std::cout << "Received " << std::dec << n << " bytes:" << std::endl;
        printHexBytes(destination, n, -1);
    }
    
    if (n != nBytes) throw CodedException(0x07);
}


void Interface::sendAcknowledge(uint8_t* data, int nBytes) {
	uint8_t response;
	
    send(data, nBytes);
	
    receive(&response, 1);
	
	if (response != INTERFACE_ACK) {
        throw CodedException(0x08);
	}
	
}


