#include "sender.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <iostream>

// ============================================================================
// Constructor / Destructor
// ============================================================================

Sender::Sender(const std::string& dest_ip, int dest_port)
    : m_destIp(dest_ip),
      m_destPort(dest_port),
      m_socketFd(-1),
      m_running(false)
{
    // std::cout << "[Sender] Initialized for " << dest_ip << ":" << dest_port << std::endl;
}

Sender::~Sender()
{
    stop();
}

// ============================================================================
// Public Methods
// ============================================================================

bool Sender::start()
{
    if (m_running.load()) {
        return true;
    }

    // Connect to receiver
    if (!connectToReceiver()) {
        return false;
    }

    // Start sender thread
    m_running.store(true);
    m_senderThread = std::thread(&Sender::sendLoop, this);

    return true;
}

void Sender::stop()
{
    if (!m_running.load()) {
        return;
    }

    // Signal thread to stop
    m_running.store(false);
    m_cv.notify_all();

    // Wait for thread to finish
    if (m_senderThread.joinable()) {
        m_senderThread.join();
    }

    // Close socket
    if (m_socketFd >= 0) {
        ::close(m_socketFd);
        m_socketFd = -1;
    }

    // Clear queue
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_frameQueue.clear();
    }
}

void Sender::enqueueFrame(const std::vector<uint8_t>& frame)
{
    if (!m_running.load()) {
        return;
    }

    if (frame.empty()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_frameQueue.push_back(frame);
    }

    m_cv.notify_one();
}

// ============================================================================
// Private Methods
// ============================================================================

bool Sender::connectToReceiver()
{
    // Create socket
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        return false;
    }

    // Set socket options
    int opt = 1;
    setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_destPort);

    if (inet_pton(AF_INET, m_destIp.c_str(), &server_addr.sin_addr) <= 0) {
        ::close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    // Connect to receiver
    if (connect(m_socketFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ::close(m_socketFd);
        m_socketFd = -1;
        return false;
    }

    return true;
}

void Sender::sendLoop()
{
    while (m_running.load()) {
        std::vector<uint8_t> frame;

        // Wait for frame or stop signal
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this]() {
                return !m_frameQueue.empty() || !m_running.load();
            });

            if (!m_running.load()) {
                break;
            }

            if (m_frameQueue.empty()) {
                continue;
            }

            // Get frame from queue
            frame = std::move(m_frameQueue.front());
            m_frameQueue.erase(m_frameQueue.begin());
        }

        // Send frame size first (4 bytes, network byte order)
        uint32_t frame_size = htonl(static_cast<uint32_t>(frame.size()));
        ssize_t sent = send(m_socketFd, &frame_size, sizeof(frame_size), 0);
        if (sent != sizeof(frame_size)) {
            continue;
        }

        // Send frame data
        size_t total_sent = 0;
        while (total_sent < frame.size()) {
            sent = send(m_socketFd, frame.data() + total_sent, frame.size() - total_sent, 0);
            if (sent < 0) {
                break;
            }
            total_sent += sent;
        }
    }
}
