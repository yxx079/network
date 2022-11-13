#include <pthread.h>

// the Read/Write lock of the root
pthread_rwlock_t root_lock = PTHREAD_RWLOCK_INITIALIZER;

/*
Every 2 seconds the server has a downtime. During a downtime, the server transforms its BST into a balanced BST using the balanceTree() function.
The reason for doing this during a downtime is to ensure that no nodes are added while setting up the tree, causing it to become unbalanced again.
Note that different BST modifications during uptime causes imbalances in the tree. The main() function spawns a thread to perform the downtime
operation every downtime.
*/
void *downtime()
{
    for (int i = 0; i < 3; i++)
    {
        sleep(2);
        // write lock the root
        pthread_rwlock_wrlock(&root_lock);
        freeSubtree(root_balanced);
        root_balanced = balanceTree(root);
        freeSubtree(root);
        root = balanceTree(root_balanced);
        pthread_rwlock_unlock(&root_lock);
    }
    return NULL;
}

/*
The server serves multiple clients concurrently. For each client, the server calls void* ServeClient(char *clientCommands) in a concurrent thread
and passes the name of the command-file using the string pointer clientCommands.
*/
void *ServeClient(char *clientCommands)
{
    FILE *fp = fopen(clientCommands, "r");
    char command[40];
    int value;
    while (1)
    {
        // read a line from fp to command
        if (fgets(command, 40, fp) == NULL)
        {
            break;
        }
        // if the command is "addNode <some unsigned int value>", then read the value and add it to the BST
        if (strncmp(command, "addNode", 7) == 0)
        {
            sscanf(command, "addNode %d", &value);
            pthread_rwlock_wrlock(&root_lock);
            root = addNode(root, value);
            pthread_rwlock_unlock(&root_lock);
            // The server also prints a log using printf() in the format of
            // "[ClientName]insertNode <SomeNumber>\n"
            printf("[%s]insertNode %d\n", clientCommands, value);
        }
        // if the command is "removeNode <some unsigned int value>", then read the value and remove it to the BST
        else if (strncmp(command, "removeNode", 10) == 0)
        {
            sscanf(command, "removeNode %d", &value);
            pthread_rwlock_wrlock(&root_lock);
            root = removeNode(root, value);
            pthread_rwlock_unlock(&root_lock);
            // The server also prints a log using printf() in the format of
            // “[ClientName]deleteNode <SomeNumber>\n”
            printf("[%s]deleteNode %d\n", clientCommands, value);
        }
        // if the command is "countNodes", the server will count the current number of nodes in the BST and print the number on the display.
        // Note that this operation does not modify the BST.
        else if (strncmp(command, "countNodes", 10) == 0)
        {
            pthread_rwlock_rdlock(&root_lock);
            int count = countNodes(root);
            pthread_rwlock_unlock(&root_lock);
            // The server also prints a log using printf() in the format of
            // “[ClientName]countNodes = <SomeNumber>\n”
            printf("[%s]countNodes = %d\n", clientCommands, count);
        }
        // if the command is "avgSubtree", The server will count the current number of nodes in the BST and print the number on the display.
        // Note that this operation does not modify the BST.
        else if (strncmp(command, "avgSubtree", 10) == 0)
        {
            pthread_rwlock_rdlock(&root_lock);
            float avg = avgSubtree(root);
            pthread_rwlock_unlock(&root_lock);
            // The server also prints a log using printf() in the format of
            // “[ClientName]avgSubtree = <SomeNumber>\n”
            printf("[%s]avgSubtree = %f\n", clientCommands, avg);
        }
    }
    return NULL;
}
