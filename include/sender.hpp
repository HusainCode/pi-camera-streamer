#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <netinet/in.h> // For sockaddr_in

/**
 * @file sender.hpp
 * @brief Declares the Sender class responsible for transmitting encoded video frames
 *        from the Raspberry Pi to a remote receiver over TCP.
 */

class Sender {
public:
    /**
     * @brief Construct a new Sender object.
     * @param dest_ip Destination IP address of the receiver.
     * @param dest_port Destination TCP port number.
     */
    Sender(const std::string& dest_ip, int dest_port);

    /**
     * @brief Destructor – ensures threads are stopped and sockets closed.
     */
    ~Sender();

    /**
     * @brief Initializes the network socket and starts the sending thread.
     * @return true if initialization succeeded; false otherwise.
     */
    bool start();

    /**
     * @brief Stops the sending thread and closes the connection.
     */
    void stop();

    /**
     * @brief Queues an encoded video frame for transmission.
     * @param frame Vector of bytes containing encoded frame data.
     */
    void enqueueFrame(const std::vector<uint8_t>& frame);

private:
    /**
     * @brief Thread loop that sends queued frames over TCP.
     */
    void sendLoop();

    /**
     * @brief Internal helper to establish TCP connection.
     * @return true if connection succeeded; false otherwise.
     */
    bool connectToReceiver();

private:
    std::string m_destIp;
    int m_destPort;
    int m_socketFd;

    std::thread m_senderThread;
    std::mutex m_queueMutex;
    std::condition_variable m_cv;

    std::vector<std::vector<uint8_t>> m_frameQueue;
    std::atomic<bool> m_running;
};
