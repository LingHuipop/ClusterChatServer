#include <sched.h>
#include <unistd.h>

int main(int argc, char **argv) {

    for (int i = 0; i < 100000; i++) {
        //usleep(0);
    sched_yield();
    }

    return 0;
}