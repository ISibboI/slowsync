#include "SSHConnection.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <stdexcept>
#include <wait.h>
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>

SSHConnection::SSHConnection(const std::string &sshCommand,
                             const std::string &sshUser,
                             const std::string &sshHost,
                             const std::string &remoteCommand) :
        sshCommand(sshCommand),
        sshUser(sshUser),
        sshHost(sshHost),
        remoteCommand(remoteCommand),
        childPid(0) {
    // We're only doing IPC in this class, so it should be fine to do that here
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        throw std::runtime_error("could not ignore SIGPIPE");
    }
}

bool SSHConnection::connect() {
    int toSSH[2];
    int fromSSH[2];
    int error = 0;

    error |= pipe2(toSSH, O_CLOEXEC);
    error |= pipe2(fromSSH, O_CLOEXEC);

    sshInFD = fromSSH[0];
    sshOutFD = toSSH[1];

    pid_t pid = fork();
    if (pid == -1) {
        return false;
    }

    if (!pid) {
        // Child process
        dup2(toSSH[0], 0);
        dup2(fromSSH[1], 1);

        const char* path = sshCommand.c_str();
        size_t filenameOffset = sshCommand.rfind('/');
        if (filenameOffset == std::string::npos) {
            filenameOffset = 0;
        }  else {
            filenameOffset++;
        }
        const char* firstArgument = path + filenameOffset;
        execlp(path, firstArgument, "-C", (sshUser + "@" + sshHost).c_str(), remoteCommand.c_str(), 0);
        return false;
    }

    error |= close(toSSH[0]);
    error |= close(fromSSH[1]);
    childPid = pid;

    // Set pipes to non-blocking
    int result = fcntl(sshOutFD, F_GETFL);
    error |= result == -1;
    error |= fcntl(sshOutFD, F_SETFL, result | O_NONBLOCK);
    result = fcntl(sshInFD, F_GETFL);
    error |= result == -1;
    error |= fcntl(sshInFD, F_SETFL, result | O_NONBLOCK);

    return error == 0;
}

void SSHConnection::terminate() {
    if (childPid == 0) {
        return;
    }

    int childState;
    pid_t waitResult = waitpid(childPid, &childState, WNOHANG);
    if (waitResult == -1) {
        std::cout << "Error waiting for SSH child process: " << strerror(errno) << std::endl;
    }

    if (waitResult) {
        if (!WIFEXITED(childState)) {
            std::cout << "Killing child process" << std::endl;
            if (kill(childPid, SIGTERM)) {
                std::cout << "Error killing child process: " << strerror(errno) << std::endl;
            }
        }
        childPid = 0;
        std::cout << "SSH terminated with exit code " << WEXITSTATUS(childState) << std::endl;
    } else {
        std::cout << "Killing child process" << std::endl;
        if (kill(childPid, SIGTERM)) {
            throw std::runtime_error("could not kill child");
        }
        childPid = 0;
        std::cout << "SSH terminated with SIGTERM" << std::endl;
    }
}

ssize_t SSHConnection::send(void *buf, size_t count) {
    // std::cout << "Sending up to " << count << " bytes" << std::endl;
    ssize_t result = write(sshOutFD, buf, count);
    if (result == -1) {
        std::cout << "Could not write to SSH: " << strerror(errno) << std::endl;
        terminate();
    }
    return result;
}

ssize_t SSHConnection::receive(void *buf, size_t count) {
    // std::cout << "Receiving up to " << count << " bytes" << std::endl;
    int available;
    if (ioctl(sshInFD, FIONREAD, &available)) {
        std::cout << "Could not check bytes available: " << strerror(errno) << std::endl;
        return -1;
    }
    // std::cout << "Bytes available: " << available << std::endl;
    if (available == 0) {
        return 0;
    }

    ssize_t size = read(sshInFD, buf, count);
    if (size == -1) {
        std::cout << "Error reading from ssh: " << strerror(errno) << std::endl;
        terminate();
    }
    return size;
}
