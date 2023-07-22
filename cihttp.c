//
// Created by aaron on 5/2/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>

#define PORT 80
#define MAX_REQUEST_SIZE 1024
#define RESPONSE_BUFFER_SIZE 1024
#define HEADER_CODE_SIZE 32

FILE* file;
int server_sock, client_sock;

void interruptHandler(){
    printf("\n\nInterrupt Signal detected, graceful termination commencing...\n");
    if(file != NULL)
        fclose(file);

    close(server_sock);
    close(client_sock);
    exit(EXIT_FAILURE);
}

void send_page(int client_sock, FILE* file){
    char buffer[RESPONSE_BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        ssize_t bytes_sent = send(client_sock, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            exit(1);
        }
    }

}

const char* get_last_modified_timestamp(FILE* file) {
    struct stat file_stat;
    if (fstat(fileno(file), &file_stat) == -1) {
        perror("Failed to get file stats, aborting...");
        exit(EXIT_FAILURE);
    }

    time_t last_modified_time = file_stat.st_mtime;
    struct tm *last_modified_tm = gmtime(&last_modified_time);
    char *last_modified_str = asctime(last_modified_tm);

    // Remove trailing newline character from the timestamp
    size_t last_modified_str_len = strlen(last_modified_str);
    if (last_modified_str_len > 0 && last_modified_str[last_modified_str_len - 1] == '\n'){
        last_modified_str[last_modified_str_len - 1] = '\0';
    }
    return last_modified_str;
}

void handle_request(int client_sock, const char *request, bool isHead, bool notImplemented){
    // Extract requested file path
    char path[MAX_REQUEST_SIZE];

    // Entirely for formatting purposes, going to remove everything past first line of request
    char* first_newline = strchr(request, '\n');
    if (first_newline)
        *first_newline = '\0';
    printf("Request:\n%s\n"
           "\nResponse:\n", request);

    // Sets path for file
    sscanf(request, "GET %s", path);

    // Prepend www/ to the path, set header code
    char file_path[MAX_REQUEST_SIZE + 4];
    char header_code[HEADER_CODE_SIZE];

    // Test cases to check whether 501, homepage, favicon.ico, or 404
    if (notImplemented) {
        sprintf(file_path, "www/501.html");
        sprintf(header_code, "HTTP/1.1 501 Not Implemented\r\n");
    }
    else if(strcmp(path, "/") == 0) {
        sprintf(file_path, "www/mainpage.html");
        sprintf(header_code, "HTTP/1.1 200 OK\r\n");
    }
    else if(strcmp(path, "/favicon.ico") == 0) {
        sprintf(file_path, "www/favicon.ico");
        sprintf(header_code, "HTTP/1.1 200 OK\r\n");
    }
    else {
        sprintf(file_path, "www/404.html");
        sprintf(header_code, "HTTP/1.1 404 Not Found\r\n");
    }

    // Open requested file
    file = fopen(file_path, "rb");
    if (file != NULL) {
        // Calculate file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Prepare response headers
        char headers[RESPONSE_BUFFER_SIZE];

        //Append code:
        sprintf(headers, "%s", header_code);
        sprintf(headers + strlen(headers),
                         "Server: cihttpd\r\n"
                         "Content-Length: %ld\r\n"
                         "Last-Modified: %s\r\n"
                         "\r\n",
                file_size, get_last_modified_timestamp(file));
        printf("%s", headers);

        // Sends page if not HEAD
        if(isHead == false) {
            send_page(client_sock, file);
        }
    } else {
        // File not found, send 404 response
        printf("File open error, aborting...\n");
        exit(EXIT_FAILURE);
    }
    // Closes file
    fclose(file);
}

int main() {
    signal(SIGINT, interruptHandler);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char request[MAX_REQUEST_SIZE];

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to the specified port
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Socket binding failed, aborting...");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 1) < 0) {
        perror("Listen failed, aborting...");
        exit(EXIT_FAILURE);
    }

    printf("|---------------------------------------|\n"
           "Server listening on port %d\n"
           "|---------------------------------------|\n", PORT);

    while (1) {
        // Accept incoming connection
        client_addr_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("Accept failed, aborting...");
            exit(EXIT_FAILURE);
        }

        // Receive HTTP request
        memset(request, 0, sizeof(request));
        ssize_t request_size = recv(client_sock, request, sizeof(request) - 1, 0);
        if (request_size < 0) {
            perror("Receive failed, aborting...");
            exit(EXIT_FAILURE);
        }

        // Determine request type
        char method[5];
        sscanf(request, "%4s", method);

        printf("\n|---------------------------------------|\n\n");
        // Call appropriate handler based on request type
        if (strcmp(method, "GET") == 0) {
            handle_request(client_sock, request, false, false);
        } else if (strcmp(method, "HEAD") == 0) {
            handle_request(client_sock, request, true, false);
        } else {
            handle_request(client_sock, request, false, true);
        }
        printf(""
               "|---------------------------------------|\n");
        // Close client socket
        close(client_sock);
    }

    // Close server socket
    close(server_sock);

    return (EXIT_SUCCESS);
}

