#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include <ctype.h> /* for isDigit function */

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char *cpyResponse;
    char *errRes;
    int sockfd;

    // Credentials
    char username[60];
    char password[60];

    char *code;
    char *errCode;

    // SesId
    char *sesId;
    char **sesIds = malloc(sizeof(char *));
    sesIds[0] = malloc(200);

    // JWT
    int *JWToken;

    // Json
    JSON_Value *root_value;
    JSON_Object *root_object;
    char *payload;

    while (1)
    {
        char *comanda = malloc(200 * sizeof(char));

        fgets(comanda, 200, stdin);
        errCode = NULL;

        // Register
        if (strcmp(comanda, "register\n") == 0)
        {
            int noSpace = 1;
            root_value = json_value_init_object();
            root_object = json_value_get_object(root_value);
            printf("username=");
            fgets(username, 60, stdin);
            printf("password=");
            fgets(password, 60, stdin);

            if (strchr(username, ' ') || strchr(password, ' '))
            {
                noSpace = 0;
                printf("Error: No space allowed in usernamee or password\n");
            }

            if (noSpace == 1)
            {
                json_object_set_string(root_object, "username", username);
                json_object_set_string(root_object, "password", password);

                payload = json_serialize_to_string_pretty(root_value);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_post_request("34.254.242.81", "/api/v1/tema/auth/register", "application/json", payload, strlen(payload), NULL, 0, NULL);

                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                cpyResponse = malloc(strlen(response));
                strcpy(cpyResponse, response);

                errCode = strtok(cpyResponse, " ");
                errCode = strtok(NULL, " ");

                if (errCode[0] == '2')
                {
                    printf("200 OK\n\n");
                }
                else
                {
                    errRes = strtok(strstr(response, "error"), "}");
                    puts(errRes);
                }
                close_connection(sockfd);
                json_free_serialized_string(payload);
                json_value_free(root_value);
            }
        }

        // LOGIN
        if (strcmp(comanda, "login\n") == 0)
        {
            int noSpace = 1;
            root_value = json_value_init_object();
            root_object = json_value_get_object(root_value);
            printf("username=");
            fgets(username, 60, stdin);
            printf("password=");
            fgets(password, 60, stdin);

            if (strchr(username, ' ') || strchr(password, ' '))
            {
                noSpace = 0;
                printf("Error: No space allowed in usernamee or password\n");
            }

            if (noSpace == 1)
            {
                json_object_set_string(root_object, "username", username);
                json_object_set_string(root_object, "password", password);

                payload = json_serialize_to_string_pretty(root_value);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_post_request("34.254.242.81", "/api/v1/tema/auth/login", "application/json", payload, strlen(payload), NULL, 0, NULL);

                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                cpyResponse = malloc(strlen(response));
                strcpy(cpyResponse, response);

                errCode = strtok(cpyResponse, " ");
                errCode = strtok(NULL, " ");

                if (errCode[0] == '2')
                {
                    sesId = strtok(strstr(response, "connect.sid"), ";");
                    printf("200 OK - Bun Venit\n\n");
                }
                else
                {
                    errRes = strtok(strstr(response, "error"), "}");
                    puts(errRes);
                }

                close(sockfd);
                json_free_serialized_string(payload);
                json_value_free(root_value);
                free(cpyResponse);
            }
        }

        // ENTER LIBRARY
        if (strcmp(comanda, "enter_library\n") == 0)
        {
            if (sesId != NULL)
            {
                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                strcpy(sesIds[0], sesId);
                message = compute_get_request("34.254.242.81", "/api/v1/tema/library/access", NULL, sesIds, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                cpyResponse = malloc(strlen(response));
                strcpy(cpyResponse, response);

                errCode = strtok(cpyResponse, " ");
                errCode = strtok(NULL, " ");

                if (errCode[0] != '2')
                {
                    errRes = strtok(strstr(response, "error"), "}");
                    puts(errRes);
                }
                else
                {
                    printf("200 OK - Succes\n\n");
                    JWToken = strstr(response, "token");
                    JWToken = strtok(JWToken, "\r\n");
                    JWToken = &JWToken[2];
                    JWToken = strtok(JWToken, "\"}");
                }
            }
            else
            {
                printf("Error - you must login first\n");
            }
        }

        // Get Books
        if (strcmp(comanda, "get_books\n") == 0)
        {
            if (sesId != NULL)
            {
                if (JWToken != NULL)
                {
                    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                    strcpy(sesIds[0], sesId);
                    message = compute_get_request("34.254.242.81", "/api/v1/tema/library/books", NULL, NULL, 0, JWToken);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    if (strstr(response, "[{") != NULL)
                    {
                        char *booklist = strstr(response, "[{");

                        JSON_Value *booksJ = json_parse_string(booklist);

                        booklist = json_serialize_to_string_pretty(booksJ);
                        puts(booklist);
                    }
                    else
                    {
                        printf("Library is empty \n");
                    }
                }
                else
                {
                    printf("Error: Must acces library first\n");
                }
            }
            else
            {
                printf("Error - you must login first\n");
            }
        }

        // Get book by id
        if (strcmp(comanda, "get_book\n") == 0)
        {
            if (sesId != NULL)
            {
                if (JWToken != NULL)
                {
                    char url[50] = "/api/v1/tema/library/books/";
                    printf("id=");
                    char id[10];
                    scanf("%s", id);

                    strcat(url, id);
                    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                    strcpy(sesIds[0], sesId);
                    message = compute_get_request("34.254.242.81", url, NULL, NULL, 0, JWToken);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    if (strstr(response, "id") != NULL)
                    {
                        char *booklist = strstr(response, "{");

                        JSON_Value *booksJ = json_parse_string(booklist);

                        booklist = json_serialize_to_string_pretty(booksJ);
                        puts(booklist);
                    }
                    else
                    {
                        printf("Book not found \n");
                    }
                }
                else
                {
                    printf("Error - you must enter library first \n");
                }
            }
            else
            {
                printf("Error - you must login first\n");
            }
        }

        // Add book
        if (strcmp(comanda, "add_book\n") == 0)
        {
            if (sesId != NULL)
            {
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);

                char *title;
                title = malloc(sizeof(char) * 100);
                char *author;
                author = malloc(sizeof(char) * 100);
                char *genre;
                genre = malloc(sizeof(char) * 100);
                char *publisher;
                publisher = malloc(sizeof(char) * 100);
                char *page_count;
                page_count = malloc(sizeof(char) * 100);

                int invPageCount = 0;

                if (JWToken != NULL)
                {
                    printf("title=");
                    fgets(title, 60, stdin);
                    title = strtok(title, "\n");

                    printf("author=");
                    fgets(author, 60, stdin);
                    author = strtok(author, "\n");

                    printf("genre=");
                    fgets(genre, 60, stdin);
                    genre = strtok(genre, "\n");

                    printf("publisher=");
                    fgets(publisher, 60, stdin);
                    publisher = strtok(publisher, "\n");

                    printf("page_count=");
                    fgets(page_count, 60, stdin);
                    page_count = strtok(page_count, "\n");

                    for (int i = 0; i < strlen(page_count); i++)
                    {
                        if (!isdigit(page_count[i]))
                        {
                            invPageCount = 1;
                        }
                    }

                    if (invPageCount == 0)
                    {
                        json_object_set_string(root_object, "title", title);
                        json_object_set_string(root_object, "author", author);
                        json_object_set_string(root_object, "genre", genre);
                        json_object_set_string(root_object, "publisher", publisher);
                        json_object_set_number(root_object, "page_count", atoi(page_count));

                        payload = json_serialize_to_string_pretty(root_value);

                        sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                        message = compute_post_request("34.254.242.81", "/api/v1/tema/library/books", "application/json", payload, strlen(payload), NULL, 0, JWToken);
                        send_to_server(sockfd, message);
                        response = receive_from_server(sockfd);
                        printf("200 OK - Book added succesfuly \n");
                    }
                    else
                    {
                        printf("Error: Page Count should be a number\n");
                    }
                }
                else
                {
                    printf("Error: Must acces library  first \n");
                }
            }
            else
            {
                printf("Error: Must login first \n");
            }
        }

        if (strcmp(comanda, "delete_book\n") == 0)
        {
            if (sesId != NULL)
            {
                if (JWToken != NULL)
                {
                    char url[50] = "/api/v1/tema/library/books/";
                    printf("id=");
                    char id[10];
                    scanf("%s", id);

                    strcat(url, id);
                    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                    strcpy(sesIds[0], sesId);
                    message = compute_delete_request("34.254.242.81", url, NULL, NULL, 0, JWToken);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    cpyResponse = malloc(strlen(response));
                    strcpy(cpyResponse, response);

                    errCode = strtok(cpyResponse, " ");
                    errCode = strtok(NULL, " ");

                    if (errCode[0] != '2')
                    {
                        errRes = strtok(strstr(response, "error"), "}");
                        puts(errRes);
                    }
                    else
                    {
                        printf("200 OK - Succes\n\n");
                    }
                }
                else
                {
                    printf("Error: Must enter library first \n");
                }
            }
            else
            {
                printf("Error: Must login first \n");
            }
        }

        if (strcmp(comanda, "logout\n") == 0)
        {

            if(sesId != NULL)
            {
                strcpy(sesIds[0], sesId);
                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request("34.254.242.81", "/api/v1/tema/library/access", NULL, sesIds, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                printf("Logout Succesfuly\n");
                sesId = NULL;
                JWToken = NULL;
            } else {
                printf("You have to be loged in to log out\n");
            }
        }

        // EXIT
        if (strcmp(comanda, "exit\n") == 0)
        {
            break;
        }
        free(comanda);
    }

    return 0;
}
