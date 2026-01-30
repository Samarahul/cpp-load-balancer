#include <iostream>
#include <string>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./backend <port>\n";
        return 1;
    }

    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    listen(server_fd, 10);
    cout << "Backend running on port " << port << endl;

    while (true)
    {
        int client = accept(server_fd, nullptr, nullptr);
        if (client < 0)
            continue;

        char buffer[4096];
        recv(client, buffer, sizeof(buffer), 0); // read request (ignored)

        string body = "Response from backend " + to_string(port) + "\n";

        string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            to_string(body.size()) + "\r\n\r\n" +
            body;

        send(client, response.c_str(), response.size(), 0);
        close(client);
    }
}
