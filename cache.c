

/*cache22.c*/

#include "cache.h"


int32 handle_create(Client*,int8*,int8*);
int32 handle_insert(Client*,int8*,int8*);
int32 handle_select(Client*,int8*,int8*);
int32 handle_print(Client*, int8*,int8*);


bool scontinuation;
bool ccontinuation;


CmdHandler handlers[] = {
    { (int8*)"CREATE", handle_create },
    { (int8*)"INSERT", handle_insert },
    { (int8*)"SELECT", handle_select },
    { (int8*)"PRINT", handle_print }
};



Callback getcmd(int8* cmd) {
    Callback cb = 0;
    int16 n, arrlen = sizeof(handlers) / sizeof(handlers[0]);

    for (n = 0; n < arrlen; n++) {
        if (!strcmp((char*)cmd, (char*)handlers[n].cmd)) {
            cb = handlers[n].handler;
            break;
        }
    }
    return cb;
}

int32 handle_print(Client* cli, int8* arg1, int8* arg2) {
    (void)arg1;  // Unused
    (void)arg2;  // Unused
    dprintf(cli->s, "\n--- Database Tree Structure ---\n");
    print_tree(cli->s, &root);  // Call the corrected function
    dprintf(cli->s, "\n--- End of Tree ---\n");
    return 1;
}

bool is_folder(int8* path) {
    if (!path || strlen((char*)path) == 0) {
        return false;
    }

    // Traverse the tree to check if a Node with the given path exists
    Node* current = find_node(path);
    
    return (current != NULL);  // If node exists, it's a folder
}


int32 handle_create(Client* cli, int8* folder, int8* args) {


    if (!(args == NULL || strlen((char*)args) == 0 || isspace((int)args[0]))) {
        dprintf(cli->s, " ---> 'Incorrect use of CREATE Command.'\n\n");
        return -1;
    }

    else if (strlen((char*)folder) == 0) {
        dprintf(cli->s, "Error: Folder path is required.\n\n");
        return -1;
    }


    else if(is_folder(folder)){
        dprintf(cli->s, "Error:The Folder is Already exist.\n\n");
        return -1;
    }


    else if(!is_folder(folder) && (!args || strlen((char*)args) == 0)){

        //Folder Not present with no args

        Node* n = find_last_west((Node*)&root);
        assert(n);

        Node* new = create_node(n,folder);
        assert(new);
            
        int32 res = attach_node(n,new);  

        if(res == 1){
            dprintf(cli->s, "The Folder %s created !.\n\n",(char*)folder);
        }
        else {
            dprintf(cli->s, "Folder creation Failed !.\n\n");
        }

        return res;
    }

    return -1;
}




int32 handle_insert(Client* cli, int8* folder, int8* args) {

    if (!folder || strlen((char*)folder) == 0 || !args || strlen((char*)args) == 0) {
        dprintf(cli->s, "  ---> 'Incorrect use of INSERT Command.'\n\n");
        return -1;
    }

    else if (!is_folder(folder)) {
        dprintf(cli->s, "Error: Folder does not exist.\n\n");
        return -1;
    }

    // Parse the key=value argument


    char key[128] = {0}, value[256] = {0};
    char *delimiter = strchr((char*)args, '=');


    if (!delimiter) {
        dprintf(cli->s, "Error: Argument must be in key=value format.\n\n");
        return -1;
    }

    strncpy(key, (char*)args, delimiter - (char*)args);
    strcpy(value, delimiter + 1);

    if (strlen(key) == 0 || strlen(value) == 0) {
        dprintf(cli->s, "Error: Both key and value must be provided.\n\n");
        return -1;
    }

    //check if key already exists

    if(lookup_linear(folder,(int8*)key)){
        dprintf(cli->s, "Error: The Key provided already EXISTS!.\n\n");
        return -1;
    }

    // Create a new leaf (key-value)

    Node* parent = find_node(folder);
    Leaf* new_leaf = create_leaf(parent , (int8*)key, (int8*)value, (int16)strlen((char*)value));
    if (!new_leaf) {

        dprintf(cli->s, "Error: Failed to create key-value pair.\n\n");
        return -1;

    }

    else {

        dprintf(cli->s, "Key-Value pair (%s=%s) inserted into %s.\n\n", key, value, (char*)folder);

        return 1;

    }

    return -1;


    
}



int32 handle_select(Client* cli, int8* folder, int8* key) {


    if (!folder || strlen((char*)folder) == 0 || !key || strlen((char*)key) == 0) {
        dprintf(cli->s, "  ---> 'Incorrect use of SELECT Command.'\n\n");
        return -1;
    }

    if (!is_folder(folder)) {
        dprintf(cli->s, "Error: Folder does not exist.\n\n");
        return -1;
    }

    // Find the parent folder (node)
    Node* parent = find_node(folder);
    if (!parent) {
        dprintf(cli->s, "Error: Specified folder does not exist.\n\n");
        return -1;
    }

     // Search for the key in the leaf nodes

    Leaf* leaf = find_leaf_with_key(parent, key);

    if (!leaf) {
        dprintf(cli->s, "Error: Key '%s' not found in folder '%s'.\n\n", key, folder);
        return -1;
    }

    // Print the value of the key
    else{

        dprintf(cli->s, "Value of key '%s' in folder '%s': %s\n\n", key, folder, leaf->value);

        return 1;
    }

    return -1;
}




/*

    struct sockaddr_in {
        sa_family_t    sin_family;   // Address family (AF_INET for IPv4)
        in_port_t      sin_port;     // Port number (16-bit, must be in network byte order)
        struct in_addr sin_addr;     // IP address (32-bit)
    };

*/


// void zero(int8* buf,int16 size){
//     int8 *p;
//     int16 n;

//     for(n=0,p=buf;n<size;n++,p++){
//         *p=0;
//     }
// }


// abc = {
//     func_select, "select"
// }

void childloop(Client * cli){
    // sleep(1);


    // select /Users/jobj/ key
    // create /Users/logins/
    // insert /Users/jobj key = value

    // zero(buf,256);
    char buf[256] = {0};
    char cmd[256] = {0}, folder[256] = {0}, args[256] = {0};

    int bytes_read = read(cli->s, buf, 255);
    if (bytes_read <= 0) {  
        perror("Client disconnected or read failed\n");
        close(cli->s);
        ccontinuation = false;  
        return;
    }

    printf("Received: %s\n", buf);

    char *p = buf;
    char *token = strtok(p, " \n\r");
    
    if (token) strncpy(cmd, token, sizeof(cmd) - 1);
    token = strtok(NULL, " \n\r");
    if (token) strncpy(folder, token, sizeof(folder) - 1);
    token = strtok(NULL, " \n\r");
    if (token) strncpy(args, token, sizeof(args) - 1);

    // dprintf(cli->s, "\ncmd:\t%s\nfolder:\t%s\nargs:\t%s\n", cmd, folder, args);

    // Command execution
    int32 res = 0;

    if (strcmp(cmd, "CREATE") == 0) {
        res = handle_create(cli, (int8*)folder, (int8*)args);
    } 
    else if (strcmp(cmd, "INSERT") == 0) {
        res = handle_insert(cli, (int8*)folder, (int8*)args);
    } 
    else if (strcmp(cmd, "SELECT") == 0) {
        res = handle_select(cli, (int8*)folder, (int8*)args);
    } 
    else if (strcmp(cmd, "PRINT") == 0) {  // New PRINT command
        res = handle_print(cli, (int8*)folder, (int8*)args);
    } 
    else {
        dprintf(cli->s, "Error: Unknown command.\n\n");
    }

    printf(res == 1 ? "Instruction executed!\n\n" : "Execution Failed!\n\n");

    return;
}

void mainloop(int s){
    
    //client

    struct sockaddr_in cli;
    int32 len = sizeof(cli);
    int s2;
    char *ip;
    Client* client;
    int16 port;
    pid_t pid;



    s2 = accept(s,(struct sockaddr*)&cli,(unsigned int*) &len);

    if(s2 < 0){
        return;
    }

    // port = (int16)htons((int)cli.sin_port);
    port = ntohs(cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);

    printf("Connection from %s:%d\n",ip,port);

    client = (Client*)malloc(sizeof(struct s_client));
    assert(client);


    zero((int8*)client,sizeof(struct s_client));

    client->s =s2;
    client->port = port;
    strncpy(client->ip,ip,15);

    pid = fork();
    if(pid){

        //parent process
        free(client);
        return;
    }
    else if(pid == 0){
        // child process
        dprintf(s2,"\n\nConnected to CACHE Database \n\n   ** USE CORRECT COMMANDS TO INTERACT WITH THE CACHE  **\n \t ---CREATE /path/ \n  \t ---INSERT /path/ key = value\n  \t ---SELECT /path/ key \n  \t ---PRINT (to print tree)\n\n");
        ccontinuation = true;
        while(ccontinuation)
            childloop(client);
        
        close(s2);
        free(client);
        exit(0);
    }
    else{
        perror("fork failed\n");
    }

    return;

}


int init_server(int16 port){

    struct sockaddr_in sock;
    int s;

    //setting up the socket structure
    sock.sin_family = AF_INET; // IPv4
    sock.sin_port = htons((int)port); //covert to networit_serverk byte order
    sock.sin_addr.s_addr = inet_addr(HOST);   // Set IP address to localhost


    //create the socket
    s= socket(AF_INET, SOCK_STREAM,0); // FIXED typo: SOCK_STREAM
    if (s == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("socket created successfully.\n");


    //bind the socket

    if( bind(s,(struct sockaddr *)&sock,sizeof(sock)) == -1 ){
        perror("Bind failed");
        close(s);
        exit(EXIT_FAILURE);
    }
        
    // Start listening

    if (listen(s, 20) == -1) {
        perror("Listen failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %s:%d\n",HOST,port);

    return s;
}

int main(int argc,char *argv[]) {

    // printf("ok\n");

    char* sport;
    int16 port;


    if(argc <2)
        sport =PORT;
    else 
        sport = argv[1];

    port = (int16)atoi(sport);

    scontinuation = true;

    int s=init_server(port);

    while(scontinuation)
        mainloop(s);

    printf("Shutting down...\n");
    close(s);
    return  0;
}
