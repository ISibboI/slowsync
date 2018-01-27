#ifndef SLOWSYNC_BUFFEREDCONNECTION_HPP
#define SLOWSYNC_BUFFEREDCONNECTION_HPP

#include "SSHConnection.hpp"

#include <vector>

class PackageConnection {
public:
    explicit PackageConnection(SSHConnection* sshConnection);

    bool connect();
    void terminate();
    void send(std::vector<char>& package);
    void receive(std::vector<char>& package);

private:
    SSHConnection* sshConnection;
    const int send_idle_timeout = 10000;

    void sendBuffer(char* buffer, size_t size);
    void receiveBuffer(char* buffer, size_t size);
};


#endif //SLOWSYNC_BUFFEREDCONNECTION_HPP
