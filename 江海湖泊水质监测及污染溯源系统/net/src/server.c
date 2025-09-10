#include "../include/server.h"

SensorData sensor_data;
Client clients[MAX_CLIENTS];
int client_count = 0;
int epoll_fd;
int server_fd;
volatile sig_atomic_t running = 1;

// 设置文件描述符为非阻塞模式
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL failed");
        return -1;
    }
    
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL failed");
        return -1;
    }
    
    return 0;
}

// 查找客户端索引
int find_client_index(int fd) {
    int i = 0;
    for ( i = 0; i < client_count; i++) {
        if (clients[i].fd == fd) {
            return i;
        }
    }
    return -1;
}

// 添加客户端
int add_client(int fd) {
    if (client_count >= MAX_CLIENTS) {
        fprintf(stderr, "达到最大客户端数量限制\r\n");
        return -1;
    }
    
    // 初始化客户端数据
    clients[client_count].fd = fd;
    memset(clients[client_count].buffer, 0, BUFFER_SIZE);
    clients[client_count].buffer_len = 0;
    clients[client_count].response_sent = 0;
    
    // 设置为非阻塞模式
    if (set_nonblocking(fd)) {
        close(fd);
        return -1;
    }
    
    // 添加到epoll
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;  // 边缘触发模式 + 挂起事件
    
    if (epoll_ctl(epoll_fd,  EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl EPOLL_CTL_ADD failed");
        close(fd);
        return -1;
    }
    
    client_count++;
    return client_count - 1;
}

// 移除客户端
void remove_client(int index) {
    if (index < 0 || index >= client_count) {
        return;
    }
    
    // 从epoll中移除
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clients[index].fd, NULL);
    
    // 关闭套接字
    close(clients[index].fd);
    
    // 移动最后一个客户端填补空缺
    if (index < client_count - 1) {
        clients[index] = clients[client_count - 1];
    }
    
    client_count--;
}

// 检查文件是否存在
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// 发送HTTP响应头
int send_http_header(int client_fd, const char *content_type, int content_length, int status_code) {
    const char *status_text;
    switch (status_code) {
        case 200: status_text = "OK"; break;
        case 404: status_text = "Not Found"; break;
        case 405: status_text = "Method Not Allowed"; break;
        default: status_text = "OK"; break;
    }
    
    char header[BUFFER_SIZE];
    int len = snprintf(header, BUFFER_SIZE,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_text, content_type, content_length);
    
    return send(client_fd, header, len, 0);
}

// 处理数据请求 - 返回JSON格式传感器数据
void handle_data_request(int client_fd, int client_index,data_t *data) {

    sensor_data.temperature = data->temperature;
    sensor_data.conductivity = data->conductivity;
    sensor_data.latitude = data->latitude;
    sensor_data.longitude = data->longitude;
    sensor_data.turbidity = data-> turbidity;
    
    char json_response[512];
    int len = snprintf(json_response, sizeof(json_response),
        "{"
        "\"temperature\": %.1f,"
        "\"conductivity\": %.1f,"
        "\"turbidity\": %.1f,"
        "\"ph\": %.1f,"
        "\"longitude\": %.6f,"
        "\"latitude\": %.6f,"
        "\"timestamp\": %ld"
        "}",  
        sensor_data.temperature,
        sensor_data.conductivity,
        sensor_data.turbidity,
        sensor_data.ph,
        sensor_data.longitude,
        sensor_data.latitude,
        (long)sensor_data.last_updated
        );
    
    if (send_http_header(client_fd, "application/json", len, 200) > 0) {
        send(client_fd, json_response, len, 0);
    }
    
    // 标记响应已发送
    clients[client_index].response_sent = 1;
}

// 处理HTML页面请求
void handle_html_request(int client_fd, int client_index) {
   const char *html_content = NULL;
    long file_size = 0;
    char *content = NULL;
    FILE *file = NULL;
    
    if (file_exists("./net/src/index.html")) {
        file = fopen("./net/src/index.html", "r");
    }
    
    if (file) {
        // 获取文件大小
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // 读取文件内容
        content = malloc(file_size + 1);
        if (content) {
            size_t read_size = fread(content, 1, file_size, file);
            if (read_size != (size_t)file_size) {
                free(content);
                content = NULL;
            } else {
                content[file_size] = '\0';
            }
        }
        fclose(file);
    }
    
    if (content) {
        // 发送HTML响应
        if (send_http_header(client_fd, "text/html", file_size, 200) > 0) {
            send(client_fd, content, file_size, 0);
        }
        free(content);
    } else {
        // 默认HTML页面
        const char *html = "<!DOCTYPE html><html><head><title>实时参数监控</title>"
                           "<style>body { font-family: Arial, sans-serif; }"
                           "h1 { color: #333; }</style></head>"
                           "<body><h1>实时参数监控系统</h1>"
                           "<p>请访问 <a href=\"/data\">/data</a> 获取传感器数据</p>"
                           "</body></html>";
        if (send_http_header(client_fd, "text/html", strlen(html), 200) > 0) {
            send(client_fd, html, strlen(html), 0);
        }
    }
    
    // 标记响应已发送
    clients[client_index].response_sent = 1;
}

// 处理客户端请求
void process_request(int client_index,data_t *data) {
    Client *client = &clients[client_index];
    int client_fd = client->fd;
    
    // 解析HTTP请求
    char method[16], path[256], protocol[16];
    if (sscanf(client->buffer, "%s %s %s", method, path, protocol) != 3) {
        const char *response = "<html><body><h1>400 Bad Request</h1></body></html>";
        if (send_http_header(client_fd, "text/html", strlen(response), 400) > 0) {
            send(client_fd, response, strlen(response), 0);
        }
        client->response_sent = 1;
        return;
    }
    
    // 只处理GET请求
    if (strcmp(method, "GET") != 0) {
        const char *response = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        if (send_http_header(client_fd, "text/html", strlen(response), 405) > 0) {
            send(client_fd, response, strlen(response), 0);
        }
        client->response_sent = 1;
        return;
    }

    
    // 路由请求
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        handle_html_request(client_fd, client_index);
    } else if (strstr(path, "/data") == path) {
        handle_data_request(client_fd, client_index,data);
    } else if (strcmp(path, "/background.jpg") == 0) {
        handle_image_request(client_fd, client_index);
    } else {
        const char *response = "<html><body><h1>404 Not Found</h1></body></html>";
        if (send_http_header(client_fd, "text/html", strlen(response), 404) > 0) {
            send(client_fd, response, strlen(response), 0);
        }
        client->response_sent = 1;
    }

}

// 处理客户端数据
void handle_client_data(int client_index,data_t *data) {
    Client *client = &clients[client_index];
    ssize_t bytes_read;
    
    // 读取所有可用数据
    while ((bytes_read = recv(client->fd, client->buffer + client->buffer_len, 
                             BUFFER_SIZE - client->buffer_len - 1, 0)) > 0) {
        client->buffer_len += bytes_read;
        client->buffer[client->buffer_len] = '\0';
        
        // 检查是否收到完整的HTTP请求（以\r\n\r\n结束）
        if (strstr(client->buffer, "\r\n\r\n") != NULL) {
            process_request(client_index,data);
            return;
        }
    }
    
    // 处理错误或连接关闭
    if (bytes_read == 0) {
        // 客户端关闭连接
        remove_client(client_index);
    } else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("读取客户端数据失败");
        remove_client(client_index);
    }
}

// 初始化服务器
int init_server()
{
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
      int nZero=0;
    // 创建epoll实例
    epoll_fd = epoll_create1(0);
    int nSendBuf=65*1024; // 设置为32K
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        return -1;
    }
    
    // 创建套接字文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        close(epoll_fd);
        return -1;
    }
   
    // 设置套接字选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    setsockopt(server_fd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
    //setsockopt(server_fd,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(int));
  
    address.sin_family = AF_INET;
    
    // 使用固定IP地址
    if (inet_pton(AF_INET, SERVER_IP, &address.sin_addr) <= 0) {
        perror("无效的服务器IP地址");
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    
    address.sin_port = htons(PORT);
    
    // 绑定套接字到端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    
    // 开始监听
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    
    // 设置服务器套接字为非阻塞
    if (set_nonblocking(server_fd)) {
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    
    // 将服务器套接字添加到epoll
    struct epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;  // 边缘触发模式
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl EPOLL_CTL_ADD failed");
        close(server_fd);
        close(epoll_fd);
        return -1;
    }
    
    printf("服务器初始化成功，使用epoll模型\r\n");
    printf("监听地址: %s:%d\r\n", SERVER_IP, PORT);
    return 0;
}
// 处理图片请求
void handle_image_request(int client_fd, int client_index) {
    FILE *file = NULL;
    long file_size = 0;
    size_t read_size = 0;
    ssize_t size = 0;
    int count = 4096;
    char *image_data = NULL;
    char *pos = NULL;
    if (file_exists("./net/src/background.jpg")) {
        printf("image exit\r\n");
        file = fopen("./net/src/background.jpg", "rb");
    }
    else{
         printf("image not exit\r\n");
    }
    
    if (file) {
        // 获取文件大小
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // 读取图片数据
        image_data = malloc(file_size);
        if (image_data) {
            read_size = fread(image_data, 1, file_size, file);
            printf("read_size = %d\r\n",read_size);
            if (read_size != (size_t)file_size) {
                free(image_data);
                image_data = NULL;
            }
        }
        
    }
    
    if (image_data) {
        // 发送图片响应
        if (send_http_header(client_fd, "image/jpeg", file_size, 200) > 0) {
            
            size = send(client_fd, image_data , read_size, 0);   
            printf("size = %d\r\n",size);
        }
        free(image_data);

    } else {
        // 默认响应
        const char *html = "<html><body><h1>404 Image Not Found</h1></body></html>";
        if (send_http_header(client_fd, "text/html", strlen(html), 404) > 0) {
            send(client_fd, html, strlen(html), 0);
        }
    }
    
    // 标记响应已发送
    clients[client_index].response_sent = 1;

    fclose(file);
}
