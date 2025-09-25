# pi-camera-streamer (Sender – Raspberry Pi)

**pi-camera-streamer** is a high-performance, multi-threaded C++ service that runs on a Raspberry Pi 5. It captures live video from a connected camera and streams it over the network to a receiver running on another machine (e.g., a laptop).  

⚡ This repository only covers the **Pi sender**.  
A separate repository will be created for the **receiver** application.  

---

## 🚀 Features (Sender – Raspberry Pi)

- 📸 **Camera capture** via V4L2 or `libcamera`
- ⚙️ **Real-time encoding** to MJPEG or H.264
- 🧵 **Multi-threaded pipeline** using POSIX threads
- 🚀 **Buffered frame queue** to decouple capture, encode, and send stages
- 🌐 **Network transport** over TCP (UDP support planned)
- 💾 Optional local archiving to disk or RAM
- 🧠 Configurable resolution, frame rate, and bitrate

---

## 🗂 Project Structure (Sender Only)

```
pi-camera-streamer/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── capture.hpp
│   ├── encoder.hpp
│   ├── sender.hpp
│   ├── buffer.hpp
│   └── config.hpp
├── src/
│   ├── main.cpp
│   ├── capture.cpp
│   ├── encoder.cpp
│   ├── sender.cpp
│   └── ...
└── build/   ← (ignored by git)
```

---

## 🧰 Requirements

### Raspberry Pi (Sender)
- Raspberry Pi OS (Lite or Full)
- Camera module (CSI or USB)
- `ffmpeg`, `v4l2-utils`, `libcamera`
- C++23 compiler (`g++` 13+ recommended)
- CMake

---

## 🚀 Quick Start (Sender on Pi)

```bash
git clone <your-private-repo>
cd pi-camera-streamer
mkdir build && cd build
cmake ..
make
./pi-camera-streamer --dest-ip <laptop-ip> --port 5000 --device /dev/video0
```

---

## 🛠 Roadmap

- [ ] Create **separate repo** for the Receiver
- [ ] Add UDP transport with optional FEC
- [ ] Add TLS/SSL secure transport
- [ ] Optional local archiving mode
- [ ] REST or gRPC API control interface
- [ ] System health monitoring
