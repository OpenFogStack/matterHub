#pragma once

#include <inet/TCPEndPoint.h>
constexpr size_t kMaxTcpActiveConnectionCount = 4;
constexpr size_t kMaxTcpPendingPackets        = 4;


// We inherit from TCPEndpoint, so we can implement all the
// callbacks we need in this class
class MQTTClient
{
public:
    MQTTClient(const chip::Inet::IPAddress & addr, uint16_t port);

private:
    MQTTClient();
    chip::Inet::TCPEndPoint * mEndpoint;
};

class MQTTMessage
{
};
