#ifndef SLOWSYNC_SSHCONNECTION_HPP
#define SLOWSYNC_SSHCONNECTION_HPP

#include <string>

class SSHConnection {
public:
    SSHConnection(
            const std::string& sshCommand,
            const std::string& sshUser,
            const std::string& sshHost,
            const std::string& remoteCommand);

    bool connect();
    void terminate();
    ssize_t send(void* buf, size_t count);
    ssize_t receive(void* buf, size_t count);

private:
    const std::string sshCommand;
    const std::string sshUser;
    const std::string sshHost;
    const std::string remoteCommand;

    pid_t childPid;

    int sshOutFD;
    int sshInFD;
};


#endif //SLOWSYNC_SSHCONNECTION_HPP
