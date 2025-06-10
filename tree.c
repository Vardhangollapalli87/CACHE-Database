
/*tree.c*/

#include "tree.h"
 
Tree root ={

    .n={ 
    .tag =(TagRoot | TagNode),
    .north =(Node *)&root,
    .west =0,
    .east = 0,
    .path = "/"
    }
};


// void print_tree(int fd,Tree* _root){


//     int8 indentation;
//     int8 buf[256];
//     int16 size;
//     Node* n;
//     Leaf* l,* last;

//     indentation =0;

//     for(n=(Node*)_root; n; n= n->west){
//         Print(indent(indentation++));
//         Print(n->path);
//         Print("\n");

//         if(n->east){

//             last = find_last(n);

//             if(last){

                
//                 for(l= n->east ;l!=0; l=l->east){
//                     // printf("last -n2--%p\n",l);
//                     Print(indent(indentation));
//                     Print(n->path);
//                     Print(" ");
//                     Print(l->key);
//                     Print(" --> '");
                    
//                     ssize_t bytes_written = write(fd, l->value, l->size);
                    
//                     if (bytes_written == -1) {
//                         perror("write failed");
//                         return;
//                     }

//                     Print("'\n");
//                 }
//                 fsync(fd);
//             }
//         }
//     }


//     return;
// }

void print_tree(int fd, Tree* _root) {
    int8 indentation = 0;
    Node* n;
    Leaf* l, *last;

    for (n = (Node*)_root; n; n = n->west) {
        dprintf(fd, "%s%s\n", indent(indentation++), n->path);  // Print folder

        if (n->east) {
            last = find_last(n);
            if (last) {
                for (l = n->east; l != NULL; l = l->east) {
                    dprintf(fd, "%s > { %s = '%s' }\n", indent(indentation),l->key, l->value);
                }
                fsync(fd);
            }
        }
    }
    return;
}


//indent(2) ->string "    "

int8* indent(int8 n){

   if (n < 0 || n >= 120) return NULL;
   if(n<1) return (int8*)"";

    char *buf = (char*)malloc(n * 2 + 1);
    if (!buf) return NULL;

    memset(buf, ' ', n * 2);
    buf[n * 2] = '\0';  // Null-terminate the string

    return (int8*)buf;  // Caller must free() this

}

void zero(int8* str,int16 size){
    int8* p;
    int16 n;

    for(n=0,p=str; n < size ;p++,n++)
        *p=0;
    return ;

    //equilvalent to memset(str,0,size);

}

Node* create_node(Node* parent,int8* path){
    Node* n;
    int16 size;

    errno = NoError;

    assert(parent);  //Ensures parent is not null

    size = sizeof(struct s_node);
    n = (Node*)malloc((int)size);
    zero((int8*)n, size);

    parent->west = n;

    n->tag = TagNode;
    n->north = parent;
    strncpy((char*)n->path,(char*)path,255);

    //if malloc fails
    if(!n) return NULL;
    return n;
}

Node* find_node_linear(int8* path){
    Node* p,* ret;


    for( ret=(Node*)0 , p = (Node*)&root ; p ;p=p->west){
        if(!strcmp((char*)p->path,(char*)path)){
            ret=p;
            break;
        }
    }

    return ret;
}


Leaf* find_leaf_linear(int8* path,int8* key){

    Node* n;
    Leaf* l,*ret;

    n= find_node_linear(path);
    if(!n) return NULL; // OR (Leaf*)0

    for(ret=(Leaf*)0 , l=n->east; l!=NULL ; l=l->east){
        if(!strcmp((char*)l->key,(char*)key)){
            ret=l;
            break;
        }

    }

    return ret;

}


int8* lookup_linear(int8* path,int8* key){
    Leaf *p;


    p=find_leaf_linear(path,key);

    return (p) ? p->value : (int8*)0;
}


Leaf* find_last_linear(Node* parent){

    Leaf* l;

    errno =0;
    assert(parent);

    if(!parent || !parent-> east)
        return (Leaf*)0;
    
    for(l= parent->east; l->east; l=l->east);
    assert(l);

    return l;

}

Node* find_last_west(Node* parent) {
    
    if (!parent) return NULL;

    Node* current = parent;

    // Traverse to the last west node
    while (current->west) {
        current = current->west;
    }

    return current;  // Last node in the west direction
}


Leaf* find_leaf_with_key(Node* parent, int8* key) {
    if (!parent || !key) return NULL; // Validate input
    
    Leaf* l = parent->east; // Start from the first leaf of the parent
    while (l) {
        if (strcmp((char*)l->key, (char*)key) == 0) {
            return l; // Found the matching key
        }
        l = l->east; // Move to the next leaf
    }
    
    return NULL; // Key not found
}


Leaf* create_leaf(Node* parent,int8* key,int8* value,int16 count){
    Leaf* l,*new; 
    int16 size;

    assert(parent);


    l= find_last_linear(parent);

    size= sizeof(struct s_leaf);
    new =(Leaf*)malloc(size);
    if(!new){
        perror("malloc failed for leaf\n");
        return NULL;
    }

    if(!l)
        // directly connected 
        parent->east = new;

    else    
        //l is a leaf
        l->east = new;

    zero((int8*)new,size);
    new->tag = TagLeaf;
    // new->west = (!l) ? (Tree*)parent : (Tree*)l;
    new->west = (l) ? (Tree*)l : (Tree*)parent;

    strncpy((char*)new->key ,(char*)key,127);
    new->value = (int8*) malloc(count);
    zero(new->value,count);

    assert(new->value);
    strncpy((char*)new->value,(char*)value,count);

    new->size = count;

    printf("Creating leaf %s at %p with west -> %p\n", key, new, new->west);


    return new ;


}



int32 attach_node(Node* parent,Node* new){

    if(!parent || !new) return -1;

    else{
        new->north = parent;
        parent->west = new;

        return 1;
    }

    return -1;
}

/*int main(){

    // printf("HELLO WORLD\n");

    // printf("%p\n",(void *)&root);

    Node* n,*n2;
    Leaf* l1,*l2;
    int8 *key, *value;
    int16 size;



    // printf("int fun() {\n %s return 0; \n}\n",indent(1));
    // printf("int main() {\n %s return 0; \n}\n",indent(2));
    // return 0;

    n = create_node((Node*)&root,(int8*)"/home");
    assert(n);
    n2 = create_node(n,(int8*)"/home/user");
    assert(n2);

    // printf("Node2 -%p\n",n2);

    key=(int8*)"ABC";
    value=(int8*)"abc";
    size =(int16)strlen((char*)value);

    l1 = create_leaf(n2,key,value,size);
    assert(l1);

    // printf("%s\n",l1->value);

    key =(int8*)"XYZ";
    value =(int8*)"xyz";
    size = (int16)strlen((char*)value);

    l2 = create_leaf(n2,key,value,size);
    assert(l2);


    // printf("%s\n",l2->value);


    print_tree(1,(Tree*)&root);

    // printf("%p %p\n",n,n2);



    free(n2);
    free(n);

    return 0;
}

int8 *example_path(int8 path){
    static int8 buf[256];
    int8 c;
    int32 x = 0;

    zero(buf, 256);


    for(c='a'; c<=path;c++){
        x = (int32)strlen((char*)buf);
        *(buf + x++) = '/';
        *(buf +x) = c;
    }

    return buf;

}

int8* example_duplicate(int8* str){
    static int8 buf[256]; 
    int16 n, x;

    zero(buf,256);
    strncpy((char*)buf, (char*)str,255);

    n = (int16)strlen((char*)buf);
    x = (n*2);

    if(x> 254) return buf;
    else 
        strncpy((char*)buf+n,strdup((char*)buf), 255);
    

    return buf;
}

int32 example_leaves(){
    int fd;
    int8 c;
    int32 x,y;
    int8 buf[256];
    int8* path,* val;
    Leaf* l;
    Node *n;

    fd = open(ExampleFile, O_RDONLY);
    assert(fd);

    y=0;
    zero(buf,256);
    while(x = read(fd,buf,255) >0){
        // abcd\n
        // 01234
        // 5
        *(buf+x-1) =0;
        path = example_path(*buf);

        n =  find_node(path);

        if(!n){

            zero(buf, 256);
            continue;
        }
        val=example_duplicate(buf);
        l = create_leaf(n, buf, val,(int16)strlen((char*)val));
        y++;
        
    }
    close(fd);

    return y;
}*/

// sample tree design
// Tree* example_tree(){

//     int8 c;
//     Node* n,*p;
//     int8 path[256];
//     int32 x;

//     zero(path,256);
//     // *path ='/';  // in beginning
//     x=0;

//     for(n=(Node*)&root, c='a';c<='z'; c++){
//         x = (int32)strlen((char*)path);
//         *(path + x++) ='/';
//         *(path + x) = c;

//         p=n;
//         n = create_node(p,path);
//     }

//     return (Tree*)&root;
// }



// int main(){

//     // Tree *example;

//     // example = example_tree();


//     // print_tree(1, example);



//     return 0;
// }