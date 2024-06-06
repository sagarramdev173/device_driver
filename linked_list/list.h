#ifndef __MY_LIST__
#define __MY_LIST__

// To traverse the list
struct my_list_head{
    struct my_list_head *next;
    struct my_list_head *prev;
};

// My basic node
struct my_list{
    struct my_list_head list;  
    int data;     
};

// initialize the node in the list
#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr);\
    (ptr)->prev = (ptr);\      
} while (0)

#define LIST_ADD_TAIL(list, ptr) do { \
    (ptr)->next = (list);\
    (ptr)->prev = (list)->prev;\
    (list)->prev->next = (ptr);\
    (list)->prev = (ptr);\ 
} while (0)






#endif // __LIST__