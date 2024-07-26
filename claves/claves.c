/*
Code that implements the functions defined in claves.h
*/

#include "claves.h"

// ENVIRONMENT VARIABLES
char* PORT_TUPLAS;  // Port where the server is listening
char* IP_TUPLAS;    // IP where the server is listening

struct sockaddr_in server_addr = {0};  // Server and client addresses
int sd;                                // Server socket descriptor

/*
* Maximum size of a request message in a string:
* op + <space> + key + <space> + value1 + <space> + N_value2 + <space> + V_value2[0] + <space> + ...
* 1 char for the op
* 1 char for the space
* 12 chars for the key
* 1 char for the space
* 256 chars for the value1
* 1 char for the space
* 2 chars for the N_value2
* 1 char for the space
* 32 * 325 chars for the V_value2
* 31 * 1 char for the spaces between the elements of the V_value2
* 1 char for the \0
* In total, 1 + 1 + 12 + 1 + 256 + 1 + 2 + 1 + 32 * 325 + 31 + 1 = 10706

* Note:
* A positive int requires a maximum of 12 characters
* A double requires a maximum of 325 characters
*/
char buffer[10706];                                 // Buffer for the messages



int get_env_variables() {
    PORT_TUPLAS = getenv("PORT_TUPLAS");
    IP_TUPLAS = getenv("IP_TUPLAS");
    if (PORT_TUPLAS == NULL || IP_TUPLAS == NULL) {
        perror("Error getting environment variables\n");
        return -1;
    }
    return 0;
}

int establish_socket_connection() {
    // Create the socket
    // AF_INET (IPv4 Internet protocols)
    // SOCK_STREAM (Sequenced, reliable, two-way, connection-based byte streams),
    // IPPROTO_TCP (TCP protocol)
    if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Error creating the socket\n");
        return -1;
    }

    // Get the environment variables for the PORT and the IP of the server
    if (get_env_variables() < 0) { return -1; }

    if (strcmp(IP_TUPLAS, "localhost") == 0) {
        strcpy(IP_TUPLAS, LOCALHOST);
    }

    // Fill the server address
    server_addr.sin_family = AF_INET;   // IPv4
    server_addr.sin_port = htons(atoi(PORT_TUPLAS));    // htons: host to network short (translates from host little-endian to network big-endian byte order)
    server_addr.sin_addr.s_addr = inet_addr(IP_TUPLAS);

    // Connect to the server
    int retries = 0;
    while (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {    // When connect() is called, the OS assigns a random high-numbered port to the client
        if (retries > MAX_RETRIES) {
            printf("The server is not available. Exiting...\n");
            // Print traces
            printf("Traces:\n");
            printf("IP_TUPLAS: %s\n", IP_TUPLAS);
            printf("server_addr.sin_addr.s_addr in point-decimal: %s\n", inet_ntoa(server_addr.sin_addr));
            printf("PORT_TUPLAS: %s\n", PORT_TUPLAS);
            printf("server_addr.sin_port in host byte order: %d\n", ntohs(server_addr.sin_port));
            return -2;  // To differentiate between a connection error and the rest of errors
        }
        if (errno == ECONNREFUSED) {
            printf("Connection refused. Check that the server is running. Trying again...\n");
        }
        // The pending connections queue is full
        else if (errno == EAGAIN) {
            printf("The pending connections queue is full. Trying again...\n");
        }
        else {
            perror("Error connecting to the server\n");
        }
        
        sleep(1 << retries);  // Exponential backoff
        retries++;
    }

    return 0;
}

int init(){
    // Inicializamos el servicio de elementos clave-valor1-valor2
    // Destruimos todas las tuplas que estuvieran almacenadas previamente
    // Devuelve 0 en caso de éxito y -1 en caso de error.

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Init operation code to the buffer
    sprintf(buffer, "%d", INIT);   // sprintf automatically adds the '\0' at the end of the string


    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {    // + 1 to include the '\0'
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    if (readLine(sd, buffer, 3) < 0) {  // The maximum response is 3 characters (-1\0)
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    int res = atoi(buffer);

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));
    
    // Return the response
    return res;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    // Almacena la tupla (key, value1, value2) en el servicio de elementos clave-valor1-valor2
    // Si ya existía una tupla con clave key, se sobreescribe con la nueva tupla
    // Devuelve 0 en caso de éxito y -1 en caso de error.

    // Handling errors in arguments

    // If any argument is NULL, we return -1
    if (value1 == NULL || V_value2 == NULL){
        return -1;
    }

    // if value1 has more tha 256 Bytes, we return -1
    if (strlen(value1) > 256){
        return -1;
    }
    
    // If N_value2 is not between 1 and 32, we return -1
    if (N_value2 < 1 || N_value2 > 32){
        return -1;
    }

    // If V_value2 has more than 32 elements, we return -1
    if (sizeof(V_value2) > 32){
        return -1;
    }

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Set_value operation code, the key, the value1 and the N_value2 to the buffer
    sprintf(buffer, "%d %d %s %d", SET_VALUE, key, value1, N_value2);

    // Add the values of the vector V_value2 to the buffer next to the N_value2
    for (int i = 0; i < N_value2; i++) {
        sprintf(buffer + strlen(buffer), " %lf", V_value2[i]);
    }

    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {    // + 1 to include the '\0'
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    if (readLine(sd, buffer, 3) < 0) {  // The maximum response is 3 characters (-1\0)
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    int res = atoi(buffer);

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));

    // Return the response
    return res;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2){
    // Obtiene el valor1 y el valor2 asociados a la clave key
    // Devuelve 0 en caso de éxito y -1 en caso de error, por ejemplo, si no existe un elemento con dicha clave o si se
    // produce un error de comunicaciones.

    // Handling errors in arguments

    // If any argument is NULL, we return -1
    if (value1 == NULL || V_value2 == NULL){
        return -1;
    }

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Get_value operation code and the key to the buffer
    sprintf(buffer, "%d %d", GET_VALUE, key);

    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    // The response is as follows:
    // error_code value1 N_value2 V_value2[0] V_value2[1] ... V_value2[N_value2 - 1]
    // error_code: maximum 2 characters
    // value1: maximum 256 characters
    // N_value2: maximum 2 characters
    // V_value2: maximum 325 characters
    // Total: 2 + <space> + 256 + <space> + 2 + <space> + 325 * 32 + 31 <spaces> + 1 = 10695

    if (readLine(sd, buffer, 10695) < 0) {
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    // Parse the response
    char *token = strtok(buffer, " ");  // Split the buffer into tokens separated by spaces
    int res = atoi(token);

    // If the response is an error, we return -1 without copying the values
    if (res == -1) {
        return -1;
    }

    // Copy the value1
    token = strtok(NULL, " ");  // Get the next token
    strcpy(value1, token);

    // Copy the N_value2
    token = strtok(NULL, " ");
    *N_value2 = atoi(token);

    // Copy the V_value2
    for (int i = 0; i < *N_value2; i++) {
        token = strtok(NULL, " ");
        V_value2[i] = atof(token);  // Convert the token to a double
    }

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));

    // Return the response
    return res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    // Modifica los valores asociados a la clave key
    // Devuelve 0 en caso de éxito y -1 en caso de error, por ejemplo, si no existe un elemento con dicha clave o si se
    // produce un error en las comunicaciones. También se devolverá -1 si el valor N_value2 está fuera
    // de rango.

    // Handling errors in arguments

    // If any argument is NULL, we return -1
    if (value1 == NULL || V_value2 == NULL){
        return -1;
    }

    // if value1 has more tha 256 Bytes, we return -1
    if (strlen(value1) > 256){
        return -1;
    }
    
    // If N_value2 is not between 1 and 32, we return -1
    if (N_value2 < 1 || N_value2 > 32){
        return -1;
    }

    // If V_value2 has more than 32 elements, we return -1
    if (sizeof(V_value2) > 32){
        return -1;
    }

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Modify_value operation code, the key, the value1 and the N_value2 to the buffer
    sprintf(buffer, "%d %d %s %d", MODIFY_VALUE, key, value1, N_value2);

    // Add the values of the vector V_value2 to the buffer next to the N_value2
    for (int i = 0; i < N_value2; i++) {
        sprintf(buffer + strlen(buffer), " %lf", V_value2[i]);
    }

    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    if (readLine(sd, buffer, 3) < 0) {
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    int res = atoi(buffer);

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));

    // Return the response
    return res;
}

int delete_key(int key){
    // Borra el elemento cuya clave es key
    // Devuelve 0 en caso de éxito y -1 en caso de error. En caso de que la clave no exista
    // también se devuelve -1.

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Delete_key operation code and the key to the buffer
    sprintf(buffer, "%d %d", DELETE_KEY, key);

    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    if (readLine(sd, buffer, 3) < 0) {
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    int res = atoi(buffer);

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));

    // Return the response
    return res;
}

int exist(int key){
    // Determina si existe un elemento con clave key
    // Devuelve 1 en caso de que exista y 0 en caso de que no exista. En caso de error se
    // devuelve -1. Un error puede ocurrir en este caso por un problema en las comunicaciones.

    // Establish the connection
    int error = establish_socket_connection();
    if (error < 0) { return error; }

    // Copy the Exist operation code and the key to the buffer
    sprintf(buffer, "%d %d", EXIST, key);

    // Send the message
    if (sendMessage(sd, buffer, (strlen(buffer) + 1)) < 0) {
        perror("Error sending the message\n");
        return -1;
    }

    // Receive the response
    if (readLine(sd, buffer, 3) < 0) {
        perror("Error receiving the message\n");
        return -1;
    }

    // Close the socket
    close(sd);

    int res = atoi(buffer);

    // Clean the buffer
    memset(buffer, 0, sizeof(buffer));

    // Return the response
    return res;
}
