#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cctype>

using namespace std;

struct Backend
{
    string ip;
    int port;
};

vector<Backend> backends;
int current_index = 0;
mutex rr_mutex;

/* -------- Load backend config -------- */
void loadConfig()
{
    ifstream file("config.json");
    string line;

    while (getline(file, line))
    {
        size_t pos = line.find("\"port\"");
        if (pos != string::npos)
        {
            Backend b;
            b.ip = "127.0.0.1";

            // find ':' after "port"
            size_t colon = line.find(":", pos);
            size_t comma = line.find(",", colon);

            string port_str = line.substr(colon + 1, comma - colon - 1);
            b.port = stoi(port_str);

            backends.push_back(b);
        }
    }

    if (backends.empty())
    {
        cerr << "No backends loaded\n";
        exit(1);
    }
}

/* -------- Round Robin -------- */
Backend getNextBackend()
{
    lock_guard<mutex> lock(rr_mutex);
    Backend b = backends[current_index];
    current_index = (current_index + 1) % backends.size();
    return b;
}

/* -------- Client handler -------- */
void handleClient(int client_socket)
{
    char buffer[4096];

    int bytes_from_client = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_from_client <= 0)
    {
        close(client_socket);
        return;
    }

    Backend backend = getNextBackend();
    cout << "Forwarding to backend " << backend.port << endl;

    int backend_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in backend_addr{};
    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(backend.port);
    inet_pton(AF_INET, backend.ip.c_str(), &backend_addr.sin_addr);

    if (connect(backend_socket, (sockaddr *)&backend_addr, sizeof(backend_addr)) < 0)
    {
        perror("connect");
        close(client_socket);
        close(backend_socket);
        return;
    }

    // forward exact request size
    send(backend_socket, buffer, bytes_from_client, 0);

    int bytes_from_backend = recv(backend_socket, buffer, sizeof(buffer), 0);
    if (bytes_from_backend > 0)
    {
        send(client_socket, buffer, bytes_from_backend, 0);
    }

    close(backend_socket);
    close(client_socket);
}

/* -------- Main -------- */
int main()
{
    loadConfig();

    int lb_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (lb_socket < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in lb_addr{};
    lb_addr.sin_family = AF_INET;
    lb_addr.sin_port = htons(8080);
    lb_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(lb_socket, (sockaddr *)&lb_addr, sizeof(lb_addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    listen(lb_socket, 10);
    cout << "Load Balancer running on port 8080" << endl;

    while (true)
    {
        int client = accept(lb_socket, nullptr, nullptr);
        if (client >= 0)
        {
            thread(handleClient, client).detach();
        }
    }
}
