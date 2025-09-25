# pi-camera-streamer

**pi-camera-streamer** is a high-performance, multi-threaded C++ service that runs on a Raspberry Pi 5. It captures live video from a connected camera and streams it over the network to a receiver on another machine (e.g., a laptop). Designed for real-time use cases, the system emphasizes low-latency, reliable delivery using POSIX threads and Linux system APIs.

---

## 🚀 Features

### 📦 On Raspberry Pi (Sender)
- 📸 **Camera capture** via V4L2 or `libcamera`
- ⚙️ **Real-time encoding** to MJPEG or H.264
- 🧵 **Multi-threaded pipeline** using POSIX threads
- 🚀 **Buffered frame queue** to decouple capture, encode, and send stages
- 🌐 **Network transport** over TCP (UDP support optional)
- 💾 Optional local archiving to disk or RAM
- 🧠 Configurable resolution, frame rate, and bitrate

### 💻 On Laptop (Receiver)
- 🔌 **Socket listener** for incoming streams
- 📥 **Stream decoder** using FFmpeg
- 💾 **File writer** to archive received video (e.g., `.mp4`)
- 👀 **Real-time display** with OpenCV (optional)
- 📊 Basic logging and health stats (future)

---

## 🗂 Project Structure

```
pi-camera-streamer/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│ ├── capture.hpp
│ ├── encoder.hpp
│ ├── sender.hpp
│ ├── buffer.hpp
│ └── config.hpp
├── src/
│ ├── main.cpp
│ ├── capture.cpp
│ ├── encoder.cpp
│ ├── sender.cpp
│ └── ...
├── receiver/
│ ├── CMakeLists.txt
│ ├── include/
│ │ ├── receiver.hpp
│ │ └── writer.hpp
│ └── src/
│ ├── main.cpp
│ ├── receiver.cpp
│ └── writer.cpp
└── build/ ← (ignored by git)
```

---

## 🧰 Requirements

### Raspberry Pi (Sender)
- Raspberry Pi OS (Lite or Full)
- Camera module (CSI or USB)
- `ffmpeg`, `v4l2-utils`, `libcamera`
- C++17 compiler (`g++`)
- CMake

### Laptop (Receiver)
- Linux or macOS
- `ffmpeg`, `libav`, `OpenCV` (optional for viewer)
- CMake + `g++`

---

## 🚀 Quick Start

### On Pi (Sender)

```bash
git clone <your-private-repo>
cd pi-camera-streamer
mkdir build && cd build
cmake ..
make
./pi-camera-streamer --dest-ip <laptop-ip> --port 5000 --device /dev/video0
```

### On Laptop (Receiver)

```bash
git clone <your-private-repo>
cd pi-camera-streamer/receiver
mkdir build && cd build
cmake ..
make
./video-receiver --listen-port 5000 --output out.mp4
```

---

## 🛠 Roadmap

- [ ] UDP transport with optional FEC
- [ ] TLS/SSL secure transport
- [ ] Live OpenCV display mode
- [ ] REST or gRPC API control interface
- [ ] System health monitoring
