/*
Header for the message structure for the message queue
 - key: key of the message (int)
 - value1: value1 of the message, 256 bytes (255 characters + '\0') (char *)
 - value2:
    - N_value2: number of elements in the vector (int)
    - V_value2: vector of doubles (double *) (32 elements)
*/

#define MAX 256

// Request message

typedef struct {
    int op;                 /* Operation code: 0 -> init, 1 -> set_value, 2 -> get_value, 3 -> modify_value, 4 -> delete_key, 5 -> exist */
    int key;                /* Key of the message */
    char value1[MAX];       /* Value1 of the message */
    int N_value2;           /* Number of elements in the vector */
    double V_value2[32];    /* Vector of doubles */
    int client_sd;          /* Socket descriptor of the client */
} Request;

// Response message
typedef struct {
    int key;                /* Key of the message */
    char value1[MAX];       /* Value1 of the message */
    int N_value2;           /* Number of elements in the vector */
    double V_value2[32];    /* Vector of doubles */
    int res;                /* Result of the operation: 0 -> success, -1 -> error */
} Response;