# pi-camera-streamer

**pi-camera-streamer** is a high-performance, multi-threaded C++ service that runs on a Raspberry Pi 5, captures live video from a connected camera, and streams it over the network to a receiver on another machine (e.g., a laptop). Designed for real-time use cases, this system emphasizes low-latency, reliable delivery using POSIX threads and Linux system APIs.

---

## 🚀 Features

### On Raspberry Pi (Sender)
- 📸 **Camera capture** via V4L2 or `libcamera`
- ⚙️ **Real-time encoding** to MJPEG or H.264
- 🧵 **Multi-threaded pipeline** using POSIX threads
- 🚀 **Buffered frame queue** for decoupling capture, encode, and send
- 🌐 **Network transport** over TCP (UDP optional)
- 💾 Optional local archiving (to disk or RAM buffer)
- 🧠 Configurable frame size, rate, resolution, and bitrate

### On Laptop (Receiver)
- 🔌 **Socket listener** for incoming video streams
- 📥 **Stream decoder** using FFmpeg
- 💾 **File writer** to archive received video (e.g., `.mp4`)
- 👀 **Real-time viewer** via OpenCV (optional)
- 📊 Logging and basic health stats

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

 

### On Pi (Sender)

```bash
git clone <your-private-repo>
cd pi-camera-streamer
mkdir build && cd build
cmake ..
make
./pi-camera-streamer --dest-ip <laptop-ip> --port 5000 --device /dev/video0
