# Readme Web Client with REST API Communication #

The purpose of this code is to communicate with a server using the HTTP protocol to execute various commands. We started with the skeleton provided in the laboratory.

The code requires the server at the address "34.254.242.81" and port 8080 to be operational in order to communicate with it.

## Structure

The structure for sending payloads to the server is in JSON format, for which we use functions like "json_object_set_string" to add fields to the "root_object."

We have created 3 functions to build the request structure:
- Get: Contains cookies and JWT (JSON Web Token). We use a matrix-like structure to store cookies, even though we will have only the session cookie.
- Post: Similar to Get, but we add body_data and fields like content_type (which will always be JSON) or content_length.
- Delete: Similar to the Get method, with only the header changed, and we no longer need cookies.

## Commands
Command input is read using fgets, so we take into account that commands will be saved with a newline character ("\n").

For commands that use POST:
> We format the input data as a JSON object.

For commands POST, GET, DELETE:
> We send them to the server using the open socket and then save the response.

- Register:
    - We read the username and password, verifying that they do not contain spaces.
    - To check for errors, we need to create a copy of the response to be able to use strtok.
    - If we find 2, it means the registration was successful; otherwise, we display the error.

- Login:
    - Similar to the register process, we send a POST request and check if we receive a session ID cookie.
    - If we receive it, we save it; if not, we display an error.
    - For subsequent functions, we will use this session ID to verify if we are logged in.
    - From now on, we will include it in the payload.

- Enter Library:
    - We send a GET request to the server with the session ID to verify access.
    - We receive a JWT as a response, which we will save. To extract it, we use strstr and then format it slightly using strtok.
    - For subsequent functions, we will use the JWT to ensure we are in the library.

- Get Books:
    - This command will return a list of books.
    - We format the list for a better display.
    - We know that the list always starts with a specific string. If it is not found, it means there are no books in the library, and we display the corresponding message.

- Get Book:
    - We ask for the book ID and send it using the POST method.
    - We display the specific fields, formatting them similarly to how we did for "Get Books."
    - If the page with the given ID does not exist, we show an error message.
    - We create the URL by concatenating the input ID at the end.

- Add Book:
    - We ask for all the necessary fields as input.
    - We use fgets to avoid issues with spaces and, consequently, remove the newline character ("\n") using strtok each time.
    - We loop through the page_count to verify that each character is a digit using the isDigit function.
    - If all fields are valid, we receive a success message and display that the book has been added.

- Delete Book:
    - We use the DELETE method and, similar to "Get Book," we send the request to a URL.

- Logout:
    - We send a GET request to log out.
    - We clear the session ID and JWT to prevent using other commands until we log in again.

- Exit:
    - This command exits the loop and stops the program.
