/*
 * Ethernets.cpp
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#include "Ethernets.h"


/// Runtime (Stack) Objects
uint8_t     mac[]  = { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };
#ifdef ETHERNET_STATIC_IP
    IPAddress   IPaddr(128, 169, 0, 13);
#endif
#ifdef ETHERNET_UDPCLIENT
    // UDP client, mostly for getting NTP time
    #define     localPort   8888 // local port to listen for UDP packets
    EthernetUDP Udp;
    #define NTP_PACKET_SIZE 48      // NTP time stamp is in the first 48 bytes of the message
    byte pb[NTP_PACKET_SIZE];       // buffer to hold incoming and outgoing packets
    byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
    IPAddress timeServerIp(192, 43, 244, 18);
#endif //ETHERNET_UDPCLIENT
#ifdef ETHERNET_WEBSERVER
    EthernetServer server(80); // (port 80 is default for HTTP)
#endif //ETHERNET_WEBSERVER


/// Functions
bool SetupEthernet ( void )
{
    bool success = false;

#ifdef INTERFACE_ETHERNET
    Serial.print("Starting Ethernet... ");

#ifdef ETHERNET_DYNAMIC_IP
    int eth_stat = Ethernet.begin(mac);
    if (0 == eth_stat)
    {
        Serial.println(" Problem starting ethernet !");
    }
    else
    {
        Serial.print("Ethernet started, IP = ");
        Serial.println( Ethernet.localIP() );

        success = true;
    }
#else
    Ethernet.begin(mac, IPaddr);
    Serial.print("Ethernet started, IP = ");
    Serial.println( Ethernet.localIP() );
    status.ethernet_valid = status.VALID;
#endif //ETHERNET_DYNAMIC_IP

    return success;
}


bool SetupUdpClient ( void )
{
    bool success = false;

#ifdef INTERFACE_ETHERNET
#ifdef ETHERNET_UDPCLIENT
    success = (bool)Udp.begin(localPort);
#endif //ETHERNET_UDPCLIENT
#endif // INTERFACE_ETHERNET

    return success;
}


bool SetupWebServer ( void )
{
    bool success = false;

#ifdef INTERFACE_ETHERNET
#ifdef ETHERNET_WEBSERVER
        server.begin();
#endif //ETHERNET_WEBSERVER
#endif // INTERFACE_ETHERNET

    return success;
}


#ifdef ETHERNET_UDPCLIENT
void sendNTPpacket(const byte *address)
/**
 * send an NTP request to the time server at the given address
 */
{
    // set all bytes in the buffer to 0
    memset(pb, 0, NTP_PACKET_SIZE);

    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    pb[0] = 0b11100011;   // LI, Version, Mode
    pb[1] = 0;     // Stratum, or type of clock
    pb[2] = 6;     // Polling Interval
    pb[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    pb[12]  = 49;
    pb[13]  = 0x4E;
    pb[14]  = 49;
    pb[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(pb,NTP_PACKET_SIZE);
    Udp.endPacket();
}


uint64_t getNTPtime()
{
    // send an NTP packet to a time server
    sendNTPpacket(timeServer);

    // wait to see if a reply is available
    delay(1000);
    if ( Udp.available() )
    {
        // read the packet into the buffer
        Udp.read(pb, NTP_PACKET_SIZE);      // New from IDE 1.0,

        // NTP contains four timestamps with an integer part and a fraction part
        // we only use the integer part here
        unsigned long t1, t2, t3, t4;
        t1 = t2 = t3 = t4 = 0;
        for (int i=0; i< 4; i++)
        {
            t1 = t1 << 8 | pb[16+i];
            t2 = t2 << 8 | pb[24+i];
            t3 = t3 << 8 | pb[32+i];
            t4 = t4 << 8 | pb[40+i];
        }

        // part of the fractional part
        // could be 4 bytes but this is more precise than the 1307 RTC
        // which has a precision of ONE second
        // in fact one byte is sufficient for 1307
        float f1,f2,f3,f4;
        f1 = ((long)pb[20] * 256 + pb[21]) / 65536.0;
        f2 = ((long)pb[28] * 256 + pb[29]) / 65536.0;
        f3 = ((long)pb[36] * 256 + pb[37]) / 65536.0;
        f4 = ((long)pb[44] * 256 + pb[45]) / 65536.0;

        // NOTE:
        // one could use the fractional part to set the RTC more precise
        // 1) at the right (calculated) moment to the NEXT second!
        //    t4++;
        //    delay(1000 - f4*1000);
        //    RTC.adjust(DateTime(t4));
        //    keep in mind that the time in the packet was the time at
        //    the NTP server at sending time so one should take into account
        //    the network latency (try ping!) and the processing of the data
        //    ==> delay (850 - f4*1000);
        // 2) simply use it to round up the second
        //    f > 0.5 => add 1 to the second before adjusting the RTC
        //   (or lower threshold eg 0.4 if one keeps network latency etc in mind)
        // 3) a SW RTC might be more precise, => ardomic clock :)


        // convert NTP to UNIX time, differs seventy years = 2208988800 seconds
        // NTP starts Jan 1, 1900
        // Unix time starts on Jan 1 1970.
        const unsigned long seventyYears = 2208988800UL;
        t1 -= seventyYears;
        t2 -= seventyYears;
        t3 -= seventyYears;
        t4 -= seventyYears;


        //Serial.println("T1 .. T4 && fractional parts");
        //PrintDateTime(DateTime(t1)); Serial.println(f1,4);
        //PrintDateTime(DateTime(t2)); Serial.println(f2,4);
        //PrintDateTime(DateTime(t3)); Serial.println(f3,4);

        //PrintDateTime(t4);
        char ntpTime[21];
        //GetDatetimeString(t4, ntpTime);
        Serial.println(f4,4);
        Serial.println();

        // Adjust timezone and DST... in my case substract 4 hours for Chile Time
        // or work in UTC?
        t4 -= (3 * 3600L);     // Notice the L for long calculations!!
        t4 += 1;               // adjust the delay(1000) at begin of loop!
        if (f4 > 0.4) t4++;    // adjust fractional part, see above
    }

    else
    {
        Serial.println("Failed to get NTP UDP packet!");
    }
}


#endif // #ifdef ETHERNET_UDPCLIENT

#endif // #ifdef INTERFACE_ETHERNET
