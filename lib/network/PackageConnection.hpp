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
    void sendBuffer(char* buffer, size_t size);
    void receiveBuffer(char* buffer, size_t size);

private:
    SSHConnection* sshConnection;
};


#endif //SLOWSYNC_BUFFEREDCONNECTION_HPP
