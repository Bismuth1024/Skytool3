//
//  main.cpp
//  Skytool3
//
//  Created by Manith Kha on 13/3/2024.
//

#include <stdio.h>
#include "misc.h"
#include "PN532.h"
#include "MIFARE_1K.h"
#include "MD5.h"
#include "CRC.h"
#include "AES.h"
#include "Skylander.h"

int main() {
    
    Interface* interface = new Interface("/dev/cu.usbserial-AR0KL3OY");
    //interface->setDebug();
    interface->begin(115200);
    
    PN532* pn = new PN532(interface);
    pn->setDebug(true);
    pn->getFirmwareVersion();
    pn->SAMConfig();
    
    /*
    Skylander* sk = new Skylander(pn);
    
    
    sk->read(pn, true);
    Encryption::decrypt(sk);
    sk->printInfo();
    sk->dump();
     */
    
    Skylander* sk = new Skylander("dumps/6/Figures/Bad Juju.dump");
    Encryption::decrypt(sk);
    sk->printInfo();
     

    
     
    

    
    
}
