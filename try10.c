#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define Max_child 3
#define Msg_buf_size 100

int num_child = 0;
pid_t child_pids[Max_child];
int my_pipe[Max_child][2];

void sigint_handler(int sig) {
    printf("\nInterruption MESSAGE. Exit Program.\n");
    exit(0);
}

void parent_process() {
    sleep(1);
    fflush(stdin);
    while (1) { // infinite loop
        for (int i = 0; i < Max_child; i++) {
            printf("\nEnter Any Message sending to child %d: ", i+1);
            char message[Msg_buf_size];
            if (fgets(message, sizeof(message), stdin) == NULL) {
                printf("\nError reading input.");
                exit(1);
            }
            printf("Parent WRITING to child %d\n", i+1);
            write(my_pipe[i][1], message, sizeof(message));
            sleep(1);
        }
    }
}

void child_process(int id) {
    printf("Child %d READ from pipe\n", id+1);

    char message[Msg_buf_size];
    read(my_pipe[id][0], message, sizeof(message));
    printf("Child %d RECEIVED Message: %s", id+1, message);
    sleep(1);
}

int main() {
    signal(SIGINT, sigint_handler);

    for (int i = 0; i < Max_child; i++) {
        if (pipe(my_pipe[i]) == -1) {
            printf("Error creating pipe for child %d\n", i+1);
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            printf("Error forking child %d\n", i+1);
            exit(1);
        } else if (pid == 0) {
            child_process(i);
            exit(0);
        } else {
            child_pids[i] = pid;
            num_child++;
            if (i == Max_child-1) {
                parent_process();
            }
        }
    }
    return 0;
}
