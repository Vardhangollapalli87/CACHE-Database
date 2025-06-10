

/*tree.h*/

#ifndef TREE
#define TREE

#define _GNU_SOURCE

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#include<assert.h>
#include<errno.h>
#include<fcntl.h>

/*
    /
      /Users/
      /Users/jonas
      /Users/jim

            /Users/login/
*/

#define TagRoot 1 /*00 01*/
#define TagNode 2 /*00 10*/
#define TagLeaf 4 /*01 00*/ 

#define NoError 0
// #define ExampleFile "./wl.txt"

// typedef void* Nullptr;
// Nullptr nullptr =0;

// void* nullptr = NULL;


#define find_last(x)    find_last_linear(x)
#define find_leaf(x,y)  find_leaf_linear(x,y)
#define lookup(x,y)     lookup_linear(x,y)
#define find_node(x)    find_node_linear(x)

#define reterr(x) \
    errno =x; \
    return NULL

#define Print(x) \
        { \
            zero(buf, 256); \
            snprintf((char*)buf, sizeof(buf), "%s", (x)); \
            size = (int16_t) strlen((char*)buf); \
            if (size) { \
                ssize_t bytes_written = write(fd, (char*)buf, size); \
                if (bytes_written == -1) { \
                    perror("write failed"); \
                } \
            } \
        }


typedef unsigned int int32;  //4-bytes
typedef unsigned short int int16;  //2-bytes
typedef unsigned char int8;        //1-byte
typedef unsigned char Tag;

struct s_node {
    Tag tag;
    struct s_node* north;  //up
    struct s_node* west;   //left
    struct s_leaf* east;   //right

    //path
    int8 path[256];
};

typedef struct s_node Node;


//leaf
struct s_leaf {
    Tag tag;
    union u_tree *west;  //left either node or leaf
    struct s_leaf* east; //right

    int8 key[128];
    int8 *value;

    int16 size;
};

typedef struct s_leaf Leaf;


union u_tree{
    Node n;    // ethier Node or Leaf can share the memory space given to the union
    Leaf l;
};

typedef union u_tree Tree;


// Declare root (defined in tree.c)
extern Tree root;




int8* indent(int8);
void print_tree(int,Tree*);
void zero(int8*,int16);
Leaf* find_leaf_linear(int8*,int8*);
int8* lookup_linear(int8* ,int8*);
Node* find_last_west(Node* root);
Node* find_node_linear(int8*);
Node* create_node(Node*,int8*);
Leaf* find_last_linear(Node*);
Leaf* create_leaf(Node*,int8*,int8*,int16);
int32 attach_node(Node*,Node*);
Leaf* find_leaf_with_key(Node* parent, int8* key);
// int8* example_path(int8);
// int32 example_leaves(void);
// Tree *example_tree(void);
// int8* example_duplicate(int8*);



#endif