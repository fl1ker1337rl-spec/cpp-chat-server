# 🚀 C++ TCP Chat Server

A robust, lightweight, and asynchronous multiclient TCP server built with **C++17** and **Qt 6**, designed to handle real-time messaging and user management using a JSON-based protocol.

---

## 🛠️ Tech Stack & Tools
* **Language:** C++17
* **Framework:** Qt 6 (`QTcpServer`, `QTcpSocket`, `QJsonDocument`, `QJsonObject`, `QJsonArray`)
* **Communication Protocol:** TCP / JSON payloads
* **Build System:** CMake

---

## 🏗️ Architecture & Core Features
* **Event-Driven Networking:** Inherits from `QTcpServer` and overrides `incomingConnection()` to manage incoming client sockets dynamically.
* **Stateful Client Mapping:** Maintains an active registry of connected clients using `QMap<QString, ClientInfo>` mapped by unique UUIDs.
* **Real-time Broadcasting:** Automatically broadcasts updated online user lists to all connected clients upon login or disconnect events.
* **Message Routing:** Parses incoming JSON packages and securely forwards private messages to the targeted recipient ID with injected sender metadata.

---

## 📋 Server Console Logs & Output

Here is a sample console output demonstrating the server lifecycle, client registrations, and message forwarding:

<img width="1132" height="160" alt="image" src="https://github.com/user-attachments/assets/3cdaddb8-758c-4b54-a1e0-9dc664362d43" />
