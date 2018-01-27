#include "PackageConnection.hpp"

#include <stdexcept>
#include <iostream>
#include <limits>

PackageConnection::PackageConnection(SSHConnection *sshConnection) :
        sshConnection(sshConnection) {}

bool PackageConnection::connect() {
    if (!sshConnection->connect()) {
        std::cout << "SSH connection failed" << std::endl;
        return false;
    }
    std::vector<char> ping;
    send(ping);
    receive(ping);
    return ping.empty();
}

void PackageConnection::terminate() {
    sshConnection->terminate();
}

void PackageConnection::send(std::vector<char> &package) {
    if (package.size() > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Package too large");
    }
    std::cout << "Sending package of size " << package.size() << std::endl;
    auto size = (int) package.size();
    sendBuffer((char*) &size, sizeof(int));
    sendBuffer(package.data(), package.size());
    std::cout << "Sent package" << std::endl;
}

void PackageConnection::sendBuffer(char *buffer, const size_t size) {
    size_t sent = 0;
    while (sent < size) {
        ssize_t sendResult = sshConnection->send(buffer + sent, size - sent);
        if (sendResult == -1) {
            throw std::runtime_error("Error sending package");
        }
        sent += sendResult;
    }
}

void PackageConnection::receive(std::vector<char> &package) {
    int size;
    receiveBuffer((char*) &size, sizeof(int));
    if (size < 0) {
        throw std::runtime_error("Package size negative");
    }
    package.resize((unsigned) size);
    receiveBuffer(package.data(), (unsigned) size);
}

void PackageConnection::receiveBuffer(char *buffer, const size_t size) {
    size_t received = 0;
    while (received < size) {
        ssize_t receiveResult = sshConnection->receive(buffer + received, size - received);
        if (receiveResult == -1) {
            throw std::runtime_error("Error receiving package");
        }
        received += receiveResult;
    }
}