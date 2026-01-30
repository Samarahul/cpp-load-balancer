# C++ Load Balancer (Round Robin)

A basic **multithreaded load balancer** implemented in **C++ using TCP socket programming**.  
This project demonstrates how incoming client requests can be distributed across multiple backend servers using the **Round Robin scheduling algorithm**.

---

## 📌 Features
- TCP socket-based communication
- Multiple backend servers
- Round-robin load balancing
- Multithreading using `std::thread`
- Thread-safe backend selection using mutex
- Configuration-based backend management

---

## 🧱 Architecture

Client  
⬇  
Load Balancer (Port 8080)  
⬇  
Backend Servers (Ports 9001, 9002, 9003)

---

## 📂 Project Structure

cpp-load-balancer/
├── backend.cpp
├── load_balancer_mt.cpp
├── config.json
├── README.md
└── .gitignore


---

## ⚙️ How to Compile

```bash
g++ backend.cpp -o backend
g++ load_balancer_mt.cpp -o lb -pthread
▶️ How to Run
1. Start backend servers (each in a separate terminal)
./backend 9001
./backend 9002
./backend 9003
2. Start the load balancer
./lb
3. Test using curl
curl http://localhost:8080
Each request will be forwarded to a different backend server in a round-robin manner.

📈 Sample Output
Response from backend 9001
Response from backend 9002
Response from backend 9003
🎓 Learning Outcomes
Understanding how load balancers work internally

Practical experience with socket programming in C++

Multithreading and synchronization using mutex

Debugging real-world networking issues

🚀 Future Improvements
Backend health checks

Least-connections load balancing

Non-blocking I/O

HTTPS support

👤 Author
Rahul Sama
