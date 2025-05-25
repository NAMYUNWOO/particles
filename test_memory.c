#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static volatile int keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <command>\n", argv[0]);
        return 1;
    }
    
    signal(SIGINT, handle_sigint);
    
    // Start the program
    FILE *fp = popen(argv[1], "r");
    if (!fp) {
        perror("popen");
        return 1;
    }
    
    // Run for 3 seconds
    sleep(3);
    
    // Send SIGTERM to close the window gracefully
    system("pkill -TERM game");
    
    // Wait a bit for cleanup
    sleep(1);
    
    pclose(fp);
    return 0;
}