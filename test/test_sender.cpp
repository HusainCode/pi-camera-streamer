#include <gtest/gtest.h>
#include "sender.hpp"
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// ============================================================================
// Mock TCP Server for Testing
// ============================================================================

class MockTCPServer {
public:
    MockTCPServer(int port) : m_port(port), m_serverFd(-1), m_clientFd(-1), m_running(false) {}

    ~MockTCPServer() {
        stop();
    }

    bool start() {
        m_serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_serverFd < 0) {
            return false;
        }

        int opt = 1;
        setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(m_port);

        if (bind(m_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            ::close(m_serverFd);
            return false;
        }

        if (listen(m_serverFd, 1) < 0) {
            ::close(m_serverFd);
            return false;
        }

        m_running = true;
        m_acceptThread = std::thread(&MockTCPServer::acceptLoop, this);
        return true;
    }

    void stop() {
        m_running = false;

        if (m_clientFd >= 0) {
            ::close(m_clientFd);
            m_clientFd = -1;
        }

        if (m_serverFd >= 0) {
            ::close(m_serverFd);
            m_serverFd = -1;
        }

        if (m_acceptThread.joinable()) {
            m_acceptThread.join();
        }
    }

    bool waitForConnection(int timeout_ms = 5000) {
        auto start = std::chrono::steady_clock::now();
        while (!isConnected()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            if (elapsed > timeout_ms) {
                return false;
            }
        }
        return true;
    }

    bool isConnected() const {
        return m_clientFd >= 0;
    }

    std::vector<uint8_t> receiveFrame(int timeout_ms = 1000) {
        if (m_clientFd < 0) {
            return {};
        }

        // Set receive timeout
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        setsockopt(m_clientFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // Receive frame size
        uint32_t frame_size_net;
        ssize_t received = recv(m_clientFd, &frame_size_net, sizeof(frame_size_net), 0);
        if (received != sizeof(frame_size_net)) {
            return {};
        }

        uint32_t frame_size = ntohl(frame_size_net);
        if (frame_size == 0 || frame_size > 10 * 1024 * 1024) { // Max 10MB
            return {};
        }

        // Receive frame data
        std::vector<uint8_t> frame(frame_size);
        size_t total_received = 0;
        while (total_received < frame_size) {
            received = recv(m_clientFd, frame.data() + total_received, frame_size - total_received, 0);
            if (received <= 0) {
                return {};
            }
            total_received += received;
        }

        return frame;
    }

private:
    void acceptLoop() {
        while (m_running) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            // Set accept timeout
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 100000; // 100ms
            setsockopt(m_serverFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

            int client_fd = accept(m_serverFd, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd >= 0) {
                m_clientFd = client_fd;
                break;
            }
        }
    }

    int m_port;
    int m_serverFd;
    int m_clientFd;
    bool m_running;
    std::thread m_acceptThread;
};

// ============================================================================
// Test Fixture
// ============================================================================

class SenderTest : public ::testing::Test {
protected:
    static constexpr int TEST_PORT = 15000;
    static constexpr const char* TEST_IP = "127.0.0.1";

    void SetUp() override {
        // Start mock server
        m_server = std::make_unique<MockTCPServer>(TEST_PORT);
        ASSERT_TRUE(m_server->start()) << "Failed to start mock server";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override {
        m_server->stop();
    }

    std::unique_ptr<MockTCPServer> m_server;
};

// ============================================================================
// Construction Tests
// ============================================================================

TEST_F(SenderTest, Construction) {
    Sender sender(TEST_IP, TEST_PORT);
    // Should construct without error
}

// ============================================================================
// Start/Stop Tests
// ============================================================================

TEST_F(SenderTest, StartSuccess) {
    Sender sender(TEST_IP, TEST_PORT);
    EXPECT_TRUE(sender.start());

    ASSERT_TRUE(m_server->waitForConnection());
    EXPECT_TRUE(m_server->isConnected());

    sender.stop();
}

TEST_F(SenderTest, StartAlreadyRunning) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Starting again should succeed (no-op)
    EXPECT_TRUE(sender.start());

    sender.stop();
}

TEST_F(SenderTest, StopWithoutStart) {
    Sender sender(TEST_IP, TEST_PORT);
    // Should not crash
    sender.stop();
}

TEST_F(SenderTest, MultipleStartStop) {
    Sender sender(TEST_IP, TEST_PORT);

    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(sender.start());
        ASSERT_TRUE(m_server->waitForConnection());
        sender.stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// ============================================================================
// Frame Sending Tests
// ============================================================================

TEST_F(SenderTest, SendSingleFrame) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Send a frame
    std::vector<uint8_t> test_frame = {1, 2, 3, 4, 5};
    sender.enqueueFrame(test_frame);

    // Receive on server side
    auto received = m_server->receiveFrame(2000);
    ASSERT_EQ(received.size(), test_frame.size());
    EXPECT_EQ(received, test_frame);

    sender.stop();
}

TEST_F(SenderTest, SendMultipleFrames) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Send multiple frames
    std::vector<std::vector<uint8_t>> test_frames = {
        {1, 2, 3},
        {4, 5, 6, 7, 8},
        {9, 10}
    };

    for (const auto& frame : test_frames) {
        sender.enqueueFrame(frame);
    }

    // Receive all frames
    for (size_t i = 0; i < test_frames.size(); ++i) {
        auto received = m_server->receiveFrame(2000);
        ASSERT_EQ(received.size(), test_frames[i].size()) << "Frame " << i << " size mismatch";
        EXPECT_EQ(received, test_frames[i]) << "Frame " << i << " content mismatch";
    }

    sender.stop();
}

TEST_F(SenderTest, SendLargeFrame) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Send a large frame (1MB)
    std::vector<uint8_t> large_frame(1024 * 1024);
    for (size_t i = 0; i < large_frame.size(); ++i) {
        large_frame[i] = static_cast<uint8_t>(i % 256);
    }

    sender.enqueueFrame(large_frame);

    // Receive on server side
    auto received = m_server->receiveFrame(5000);
    ASSERT_EQ(received.size(), large_frame.size());
    EXPECT_EQ(received, large_frame);

    sender.stop();
}

TEST_F(SenderTest, SendEmptyFrame) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Try to send empty frame (should be rejected)
    std::vector<uint8_t> empty_frame;
    sender.enqueueFrame(empty_frame);

    // Should not receive anything
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto received = m_server->receiveFrame(500);
    EXPECT_TRUE(received.empty());

    sender.stop();
}

TEST_F(SenderTest, EnqueueFrameWithoutStart) {
    Sender sender(TEST_IP, TEST_PORT);

    // Should not crash, but frame won't be sent
    std::vector<uint8_t> frame = {1, 2, 3};
    sender.enqueueFrame(frame);
}

// ============================================================================
// VGA Frame Simulation Test
// ============================================================================

TEST_F(SenderTest, SendVGAFrame) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    // Simulate VGA RGB frame (640x480x3)
    const size_t vga_size = 640 * 480 * 3;
    std::vector<uint8_t> vga_frame(vga_size, 128); // Gray frame

    sender.enqueueFrame(vga_frame);

    auto received = m_server->receiveFrame(5000);
    ASSERT_EQ(received.size(), vga_size);

    sender.stop();
}

// ============================================================================
// Throughput Test
// ============================================================================

TEST_F(SenderTest, Throughput) {
    Sender sender(TEST_IP, TEST_PORT);
    ASSERT_TRUE(sender.start());
    ASSERT_TRUE(m_server->waitForConnection());

    const int frame_count = 30;
    const size_t frame_size = 64 * 1024; // 64KB frames

    auto start = std::chrono::steady_clock::now();

    // Send frames
    for (int i = 0; i < frame_count; ++i) {
        std::vector<uint8_t> frame(frame_size, static_cast<uint8_t>(i));
        sender.enqueueFrame(frame);
    }

    // Receive all frames
    int received_count = 0;
    for (int i = 0; i < frame_count; ++i) {
        auto received = m_server->receiveFrame(2000);
        if (received.size() == frame_size) {
            received_count++;
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_EQ(received_count, frame_count);

    double throughput_mbps = (frame_count * frame_size * 8.0) / (duration_ms / 1000.0) / 1e6;
    std::cout << "[PERF] Throughput: " << throughput_mbps << " Mbps, "
              << "Duration: " << duration_ms << " ms" << std::endl;

    sender.stop();
}

// ============================================================================
// Stress Test
// ============================================================================

TEST_F(SenderTest, RapidStartStop) {
    Sender sender(TEST_IP, TEST_PORT);

    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(sender.start());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        sender.stop();
    }
}

// ============================================================================
// Destructor Test
// ============================================================================

TEST_F(SenderTest, DestructorStopsGracefully) {
    {
        Sender sender(TEST_IP, TEST_PORT);
        ASSERT_TRUE(sender.start());
        ASSERT_TRUE(m_server->waitForConnection());

        std::vector<uint8_t> frame = {1, 2, 3};
        sender.enqueueFrame(frame);

        // Sender destructor should clean up gracefully
    }

    // Should be able to reconnect after cleanup
    Sender sender2(TEST_IP, TEST_PORT);
    EXPECT_TRUE(sender2.start());
}
