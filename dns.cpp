#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "dns.h"
#include "udp.h"
#include <string.h>

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

static constexpr uint8_t
    SOCKET_NONE = 255,
    UDP_HEADER_SIZE = 8,
    DNS_HEADER_SIZE = 12,
    TTL_SIZE = 4,
    QUERY_FLAG = 0,
    DNS_PORT = 53;

#define RESPONSE_FLAG            (1<<15)
#define QUERY_RESPONSE_MASK      (1<<15)
#define OPCODE_STANDARD_QUERY    (0)
#define OPCODE_INVERSE_QUERY     (1<<11)
#define OPCODE_STATUS_REQUEST    (2<<11)
#define OPCODE_MASK              (15<<11)
#define AUTHORITATIVE_FLAG       (1<<10)
#define TRUNCATION_FLAG          (1<<9)
#define RECURSION_DESIRED_FLAG   (1<<8)
#define RECURSION_AVAILABLE_FLAG (1<<7)
#define RESP_NO_ERROR            (0)
#define RESP_FORMAT_ERROR        (1)
#define RESP_SERVER_FAILURE      (2)
#define RESP_NAME_ERROR          (3)
#define RESP_NOT_IMPLEMENTED     (4)
#define RESP_REFUSED             (5)
#define RESP_MASK                (15)
#define TYPE_A                   (0x0001)
#define CLASS_IN                 (0x0001)
#define LABEL_COMPRESSION_MASK   (0xC0)

#define SUCCESS          1
#define TIMED_OUT        -1
#define INVALID_SERVER   -2
#define TRUNCATED        -3
#define INVALID_RESPONSE -4

void DNSClient::begin(uint32_t ip)
{
    iDNSServer = ip;
    iRequestId = 0;
}

DNSClient::DNSClient(UDP *udp) : iUdp(udp)
{
}

int DNSClient::inet_aton(const char *aIPAddrString, uint32_t &aResult)
{
    // See if we've been given a valid IP address
    const char* p = aIPAddrString;

    while (*p && ((*p == '.') || (*p >= '0') || (*p <= '9') ))
        p++;

    if (*p == '\0')
    {
        // It's looking promising, we haven't found any invalid characters
        p = aIPAddrString;
        int segment =0;
        int segmentValue =0;
        while (*p && (segment < 4))
        {
            if (*p == '.')
            {
                // We've reached the end of a segment
                if (segmentValue > 255)
                {
                    // You can't have IP address segments that don't fit in a byte
                    return 0;
                }
                else
                {
                    uint8_t *ptrResult = (uint8_t *)&aResult;
                    ptrResult[segment] = (uint8_t)segmentValue;
                    segment++;
                    segmentValue = 0;
                }
            }
            else
            {
                // Next digit
                segmentValue = (segmentValue*10)+(*p - '0');
            }
            p++;
        }
        // We've reached the end of address, but there'll still be the last
        // segment to deal with
        if ((segmentValue > 255) || (segment > 3))
        {
            // You can't have IP address segments that don't fit in a byte,
            // or more than four segments
            return 0;
        }
        else
        {
            uint8_t *ptrResult = (uint8_t *)&aResult;
            ptrResult[segment] = (uint8_t)segmentValue;
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

int DNSClient::getHostByName(const char* aHostname, uint32_t &aResult)
{
    int ret =0;

    if (inet_aton(aHostname, aResult))
        return 1;

    if (iDNSServer == 0)
        return INVALID_SERVER;
	
    // Find a socket to use
    //if (iUdp->begin(1024+(millis() & 0xF)) == 1)
    if (iUdp->begin(1024) == 1)
    {
        // Try up to three times
        int retries = 0;
//        while ((retries < 3) && (ret <= 0))
        {
            // Send DNS request
            ret = iUdp->beginPacket(iDNSServer, DNS_PORT);
            if (ret != 0)
            {
                // Now output the request data
                ret = BuildRequest(aHostname);
                if (ret != 0)
                {
                    // And finally send the request
                    ret = iUdp->endPacket();
                    if (ret != 0)
                    {
                        // Now wait for a response
                        int wait_retries = 0;
                        ret = TIMED_OUT;
                        while ((wait_retries < 3) && (ret == TIMED_OUT))
                        {
                            ret = ProcessResponse(5000, aResult);
                            wait_retries++;
                        }
                    }
                }
            }
            retries++;
        }

        // We're done with the socket now
        iUdp->stop();
    }

    return ret;
}

uint16_t DNSClient::BuildRequest(const char *aName)
{
    iRequestId++;
    uint16_t twoByteBuffer;
    iUdp->write((uint8_t*)&iRequestId, sizeof(iRequestId));
    twoByteBuffer = htons(QUERY_FLAG | OPCODE_STANDARD_QUERY | RECURSION_DESIRED_FLAG);
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    twoByteBuffer = htons(1);  // One question record
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    twoByteBuffer = 0;  // Zero answer records
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    const char* start =aName;
    const char* end =start;
    uint8_t len;

    while (*end)
    {
        // Find out how long this section of the name is
        end = start;

        while (*end && (*end != '.') )
            end++;

        if (end-start > 0)
        {
            // Write out the size of this section
            len = end-start;
            iUdp->write(&len, sizeof(len));
            // And then write out the section
            iUdp->write((uint8_t*)start, end-start);
        }
        start = end+1;
    }

    // We've got to the end of the question name, so
    // terminate it with a zero-length section
    len = 0;
    iUdp->write(&len, sizeof(len));
    // Finally the type and class of question
    twoByteBuffer = htons(TYPE_A);
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

    twoByteBuffer = htons(CLASS_IN);  // Internet class of question
    iUdp->write((uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
    // Success!  Everything buffered okay
    return 1;
}


uint16_t DNSClient::ProcessResponse(uint16_t aTimeout, uint32_t &aAddress)
{
    // Wait for a response packet
    for (uint8_t i = 0; iUdp->parsePacket() <= 0; i++)
    {
        if (i >= 0xf0)
            return TIMED_OUT;

        _delay_ms(50);
    }

    // We've had a reply!
    // Read the UDP header
    uint8_t header[DNS_HEADER_SIZE]; // Enough space to reuse for the DNS header

    if ( (iDNSServer != iUdp->remoteIP()) || (iUdp->remotePort() != DNS_PORT) )
        return INVALID_SERVER;

    // Read through the rest of the response
    if (iUdp->available() < DNS_HEADER_SIZE)
        return TRUNCATED;
    
    iUdp->read(header, DNS_HEADER_SIZE);

    uint16_t header_flags = htons(*((uint16_t*)&header[2]));
    // Check that it's a response to this request
    if ( ( iRequestId != (*((uint16_t*)&header[0])) ) ||
        ((header_flags & QUERY_RESPONSE_MASK) != (uint16_t)RESPONSE_FLAG) )
    {
        // Mark the entire packet as read
        iUdp->flush();
        return INVALID_RESPONSE;
    }
    // Check for any errors in the response (or in our request)
    // although we don't do anything to get round these
    if ( (header_flags & TRUNCATION_FLAG) || (header_flags & RESP_MASK) )
    {
        // Mark the entire packet as read
        iUdp->flush();
        return -5; //INVALID_RESPONSE;
    }

    // And make sure we've got (at least) one answer
    uint16_t answerCount = htons(*((uint16_t*)&header[6]));
    if (answerCount == 0 )
    {
        // Mark the entire packet as read
        iUdp->flush();
        return -6; //INVALID_RESPONSE;
    }

    // Skip over any questions
    for (uint16_t i =0; i < htons(*((uint16_t*)&header[4])); i++)
    {
        // Skip over the name
        uint8_t len;
        do
        {
            iUdp->read(&len, sizeof(len));
            if (len > 0)
            {
                // Don't need to actually read the data out for the string, just
                // advance ptr to beyond it
                while (len--)
                    iUdp->read(); // we don't care about the returned byte
            }
        } while (len != 0);

        // Now jump over the type and class
        for (int i =0; i < 4; i++)
            iUdp->read(); // we don't care about the returned byte
    }

    // Now we're up to the bit we're interested in, the answer
    // There might be more than one answer (although we'll just use the first
    // type A answer) and some authority and additional resource records but
    // we're going to ignore all of them.

    for (uint16_t i =0; i < answerCount; i++)
    {
        // Skip the name
        uint8_t len;
        do
        {
            iUdp->read(&len, sizeof(len));
            if ((len & LABEL_COMPRESSION_MASK) == 0)
            {
                // It's just a normal label
                if (len > 0)
                {
                    // And it's got a length
                    // Don't need to actually read the data out for the string,
                    // just advance ptr to beyond it
                    while(len--)
                        iUdp->read(); // we don't care about the returned byte
                }
            }
            else
            {
                // This is a pointer to a somewhere else in the message for the
                // rest of the name.  We don't care about the name, and RFC1035
                // says that a name is either a sequence of labels ended with a
                // 0 length octet or a pointer or a sequence of labels ending in
                // a pointer.  Either way, when we get here we're at the end of
                // the name
                // Skip over the pointer
                iUdp->read(); // we don't care about the returned byte
                // And set len so that we drop out of the name loop
                len = 0;
            }
        } while (len != 0);

        // Check the type and class
        uint16_t answerType;
        uint16_t answerClass;
        iUdp->read((uint8_t*)&answerType, sizeof(answerType));
        iUdp->read((uint8_t*)&answerClass, sizeof(answerClass));

        // Ignore the Time-To-Live as we don't do any caching
        for (int i =0; i < TTL_SIZE; i++)
            iUdp->read(); // we don't care about the returned byte

        // And read out the length of this answer
        // Don't need header_flags anymore, so we can reuse it here
        iUdp->read((uint8_t*)&header_flags, sizeof(header_flags));

        if ( (htons(answerType) == TYPE_A) && (htons(answerClass) == CLASS_IN) )
        {
            if (htons(header_flags) != 4)
            {
                // It's a weird size
                // Mark the entire packet as read
                iUdp->flush();
                return -9;//INVALID_RESPONSE;
            }

            iUdp->read((uint8_t *)&aAddress, 4);
            return SUCCESS;
        }
        else
        {
            // This isn't an answer type we're after, move onto the next one
            for (uint16_t i =0; i < htons(header_flags); i++)
                iUdp->read(); // we don't care about the returned byte
        }
    }

    // Mark the entire packet as read
    iUdp->flush();

    // If we get here then we haven't found an answer
    return -10;//INVALID_RESPONSE;
}



