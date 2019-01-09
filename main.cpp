#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <stdlib.h>
#include <zconf.h>
#include <sqlite3.h>
#include <string>

class Server
{
    int sock, listener;
//    char buf[1024];
    int bytes_read;
public:

    void openSock()
    {
        struct sockaddr_in addr;

        listener = socket(AF_INET, SOCK_STREAM, 0);
        if(listener < 0)
        {
            perror("socket");
            exit(1);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(3425);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("bind");
            exit(2);
        }
    }
    void serverWork();

    void closeSock()
    {

    }
    void dbConnect()
    {
        int rc;

        rc = sqlite3_open("test.db", &db);

        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stderr, "Opened database successfully\n");
        }
    }
    void dbClose()
    {
        sqlite3_close(db);
    };
    std::string dbRequest(std::string req)
    {
        char* zErrMsg = 0;
        int rc = sqlite3_exec(db, req.c_str(), callback, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Table created successfully\n");
        }
    }
private:
    sqlite3 *db;

    static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }
};

void Server::serverWork()
{
    listen(listener, 1);

    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }

        while(1)
        {
            int size = 0;
            bytes_read = recv(sock, &size, sizeof(int), 0);
            char *buf = new char [size];
            bytes_read = recv(sock, buf, size, 0);
            if(bytes_read <= 0) break;
            size = 9;
            send(sock, &size, sizeof(int), 0);
            buf = "12345678\0";
            send(sock, buf, size, 0);
            printf(buf);
        }


        close(sock);
    }
}

int main()
{
    Server app;
    app.openSock();

    app.serverWork();




  return 0;
}