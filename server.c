#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define PORT 8000
#define SIZE 8
#define MSGSIZE 1024

char *opID;
char file[128];
char type[128];
int clock;
char *comp;
char *inFile;
void serve(int s)
{
    char buffer[MSGSIZE];
    int size, i = 0;
    struct stat buf;
    const char space[2] = " ";
    FILE *sin = fdopen(s, "r");
    FILE *sout = fdopen(s, "w");

    // Reads the request from the client
    while (fgets(buffer, MSGSIZE, sin) != NULL)
    {
        printf("%d - [%s]\n", ++i, buffer);
        if (i == 1)
        {
            int pointerState = 1;
            opID = strtok(buffer, space);
            if (opID != NULL){
                if (pointerState == 2){
                    if (strcmp(strcpy(file, opID), "/") == 0){
                        strcpy(file, "file.txt");
                        strcpy(type,".txt");
                        char *filename = "file.txt";
                        FILE *newFile = fopen(filename, "w");
                        if (newFile == NULL){
                            return NULL;
                        }
                        struct dirent *nextP;
                        DIR *prevP = opendir(".");
                        if (prevP == NULL){
                            return NULL;
                        }
                        while ((nextP = readdir(prevP)) != NULL)
                        {
                            fprintf(newFile, "%s\n", nextP->d_name);
                        }
                        fclose(newFile);
                        closedir(prevP);
                    }
                    else{
                        strcpy(file, opID + 1);
                        strcpy(type,opID + 1);
                    }
                }
                pointerState++;
                opID = strtok(NULL, space);
            }
        }
        if (buffer[0] == '\r' && buffer[1] == '\n'){
            break;
        }
    }
    sleep(1);

     // Builds response
    sprintf(buffer, "HTTP/1.0 200 OK\r\n");
    fputs(buffer, sout);

    sprintf(buffer, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
    fputs(buffer, sout);
    strcpy(comp,type);
    inFile = strstr(comp, ".html");
    if (inFile){
        sprintf("HTML");
        fputs(buffer, sout);       
    }
    inFile = strstr(comp, ".jpg");
    if (inFile){
        sprintf("JPG");
        fputs(buffer, sout);
    }
    inFile = strstr(comp, ".png");
    if (inFile){
        sprintf("PNG");
        fputs(buffer, sout);
    }
    inFile = strstr(comp, ".txt");
    if (inFile) {
        sprintf("TXT");
        fputs(buffer, sout);
    }
    FILE *fin = fopen(file, "r");
    while ((size = fread(buffer, 1, MSGSIZE, fin)) != 0){
        size = fwrite(buffer, 1, size, sout);
    }
    fflush(0);
}

int main() {
    int sd, sdo, size, r;
    struct sockaddr_in sin, pin;
    socklen_t addrlen;

    // 1. Crear el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    // 2. Asociar el socket a IP:port
    r = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
    if (r < 0) {
        perror("bind");
        return -1;
    }
    // 3. Establecer backlog
    listen(sd, 5);

    addrlen = sizeof(pin);
    // 4. Esperar conexion
    while( (sdo = accept(sd, (struct sockaddr *)  &pin, &addrlen)) > 0) {
        //if(!fork()) {
            printf("Connected from %s\n", inet_ntoa(pin.sin_addr));
            printf("Port %d\n", ntohs(pin.sin_port));

            serve(sdo);

            close(sdo);
            exit(0);
        //}
    }
    close(sd);

    sleep(1);

}

