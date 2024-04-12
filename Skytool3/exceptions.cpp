//
//  exceptions.cpp
//  Skytool3
//
//  Created by Manith Kha on 14/3/2024.
//

#include "exceptions.h"


CodedException::CodedException(uint16_t code) : code(code) {}

const char * CodedException::what() {
    switch (code) {
        case 0x01:
            return "The numer of bytes to convert is larger than the size of the unsigned integer type.";
            break;
        case 0x02:
            return "The file could not be opened.";
            break;
        case 0x03:
            return "Invalid baud rate.";
            break;
        case 0x04:
            return "Error setting serial port parameters.";
            break;
        case 0x05:
            return "Invalid serial port.";
            break;
        case 0x06:
            return "Data exceeds I2C buffer length.";
            break;
        case 0x07:
            return "An incorrect number of bytes was read from the serial port.";
            break;
        case 0x08:
            return "The interface adapter device did not acknowledge.";
            break;
        case 0x09:
            return "The PN532 did not acknowledge.";
            break;
        case 0x0A:
            return "The PN532 detected multiple RFID tags (only one is supported).";
            break;
        case 0x0B:
            return "The target data location is not a valid data location of a MIFARE 1K card.";
            break;
        case 0x0C:
            return "The requested operation can only be performed on a magic MIFARE_1K card.";
            break;
        case 0x0D:
            return "The specified data location should not be encrypted/decrypted on a Skylander.";
            break;
        case 0x0E:
            return "Trying to encrypt/decrypt an already encrypted/decrypted Skylander.";
            break;
        case 0x0F:
            return "This operation should only be done on a decrypted Skylander.";
            break;
        case 0x10:
            return "Checksum error for Skylander.";
            break;
        case 0x11:
            return "Unknown checksum type.";
            break;
        case 0x12:
            return "Skylander maximum name length is 15 characters.";
            break;
        case 0x13:
            return "Skylander maximum name XP is ____.";
            break;
        case 0x14:
            return "Invalid hat name.";
            break;
        case 0x15:
            return "The backup that you are trying to load doesn't match the UID of the Skylander that it is being uploaded to.";
            break;
        case 0x16:
            return "The requested area is not a valid data area on a skylander.";
            break;
        default:
            return "unknown error code";
    }
}


/*
 #include "errorCodes.h"

 using namespace std;
 bool errorFlag = false;

 bool checkError(int code) {
     if (code == 0) return true;
     
     if (code < 0x30) {
         cout << "PN532 Error: " << endl;
     } else if (code < 0x40) {
         cout << "Interface Error: " << endl;
     } else if (code < 0x60) {
         cout << "Skylander Error: " << endl;
     } else {
         cout << "Miscellaneous Error: " << endl;
     }
     
     switch (code) {
     
         case 0x01:
             cout << "Time Out, the target has not answered" << endl;
             break;
         case 0x02:
             cout << "A CRC error has been detected by the CIU" << endl;
             break;
         case 0x03:
             cout << "A Parity error has been detected by the CIU" << endl;
             break;
         case 0x04:
             cout << "During an anti-collision/select operation (ISO/IEC14443-3 Type A and ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been detected" << endl;
             break;
         case 0x05:
             cout << "Framing error during Mifare operation" << endl;
             break;
         case 0x06:
             cout << "An abnormal bit-collision has been detected during bit wise anti-collision at 106 kbps" << endl;
             break;
         case 0x07:
             cout << "Communication buffer size insufficient" << endl;
             break;
         case 0x09:
             cout << "RF Buffer overflow has been detected by the CIU (bit BufferOvfl of the register CIU_Error)" << endl;
             break;
         case 0x0A:
             cout << "In active communication mode, the RF field has not been switched on in time by the counterpart (as defined in NFCIP-1 standard)" << endl;
             break;
         case 0x0B:
             cout << "RF Protocol error (cf. Error! Reference source not found., description of the CIU_Error register)" << endl;
             break;
         case 0x0C:
             cout << "PN532 did not acknowledge." << endl;
             break;
         case 0x0D:
             cout << "Temperature error: the internal temperature sensor has detected overheating, and therefore has automatically switched off the antenna drivers" << endl;
             break;
         case 0x0E:
             cout << "Internal buffer overflow" << endl;
             break;
         case 0x10:
             cout << "Invalid parameter (range, format, ...)" << endl;
             break;
         case 0x12:
             cout << "DEP Protocol: The PN532 configured in target mode does not support the command received from the initiator (the command received is not one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ, RLS_REQ)" << endl;
             break;
         case 0x13:
             cout << "DEP Protocol, Mifare or ISO/IEC14443-4: The data format does not match to the specification.  Depending on the RF protocol used, it can be: Bad length of RF received frame, Incorrect value of PCB or PFB, Invalid or unexpected RF received frame, NAD or DIDincoherence." << endl;
             break;
         case 0x14:
             cout << "MIFARE Authentication error." << endl;
             break;
         case 0x23:
             cout << "ISO/IEC14443-3: UID Check byte is wrong." << endl;
             break;
         case 0x25:
             cout << "DEP Protocol: Invalid device state, the system is in a state which does not allow the operation." << endl;
             break;
         case 0x26:
             cout << "Operation not allowed in this configuration (host controller Interface)." << endl;
             break;
         case 0x27:
             cout << "This command is not acceptable due to the current context of the PN532 (Initiator vs. Target, unknown target number, Target not in the good state, ...)." << endl;
             break;
         case 0x29:
             cout << "The PN532 configured as target has been released by its initiator." << endl;
             break;
         case 0x2A:
             cout << "PN532 and ISO/IEC14443-3B only: the ID of the card does not match, meaning that the expected card has been exchanged with another one." << endl;
             break;
         case 0x2B:
             cout << "PN532 and ISO/IEC14443-3B only: the card previously activated has disappeared." << endl;
             break;
         case 0x2C:
             cout << "Mismatch between the NFCID3 initiator and the NFCID3 target in DEP 212/424 kbps passive." << endl;
             break;
         case 0x2D:
             cout << "An over-current event has been detected." << endl;
             break;
         case 0x2E:
             cout << "NAD missing in DEP frame." << endl;
             break;
         case 0x2F:
             cout << "Currently only supports one MIFARE card at a time." << endl;
             break;
             
         case 0x30:
             cout << "Invalid serial port." << endl;
             break;
         case 0x31:
             cout << "Input exceeds Interface buffer size." << endl;
             break;
         case 0x32:
             cout << "Incorrect number of bytes received." << endl;
             break;
         case 0x33:
             cout << "Arduino did not acknowledge." << endl;
             break;
             
             
         case 0x40:
             cout << "Operation is only possible on a magic card." << endl;
             break;
         case 0x41:
             cout << "Invalid data location." << endl;
             break;
             
         case 0x50:
             cout << "Skylander is already decrypted." << endl;
             break;
         case 0x51:
             cout << "Skylander is already encrypted." << endl;
             break;
         case 0x52:
             cout << "Target data block does not need to be encrypted/decrypted." << endl;
             break;
         case 0x53:
             cout << "Skylander should be decrypted before attempting this operation." << endl;
             break;
         case 0x54:
             cout << "Invalid checksum type." << endl;
             break;
         case 0x55:
             cout << "Checksum error." << endl;
             break;
         case 0x56:
             cout << "Invalid data area (should be 0, 1, or 2)." << endl;
             break;
         case 0x57:
             cout << "Input is too large for destination." << endl;
             break;
         case 0x58:
             cout << "Invalid character." << endl;
             break;
         case 0x59:
             cout << "UIDs do not match." << endl;
             break;
         case 0x5A:
             cout << "Invalid hat." << endl;
             break;
             
         case 0x60:
             cout << "Could not open file." << endl;
             break;
         
         
             
             
             
         default:
             cout << "Unknown error" << endl;
     }
     errorFlag = true;
     return false;
     
 }

 */
