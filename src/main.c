#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h> /* Definition of SYS_* constants */
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern void *func1(void *);
extern void *func2(void *);
extern void *func3(void *);
extern int main();
void * my_get_physical_addresses(void* vaddr)
{
    return syscall(351, vaddr);
}

struct data_
{
    int id;
    char name[16];
};
typedef struct data_ sdata;
static __thread sdata tx; // thread local variable

// bss
int c;
int k;

// heap
int *p;
int *p2;

int a = 123; // global variable

void hello(int pid)
{
    // local varialbe
    int b = 10;
    b = b + pid;

    // global variable
    printf("In thread %d \na = %d, logical address = %p, ", pid, a, &a);
    printf("physical address = %p\n", my_get_physical_addresses(&a));
    // local variable
    printf("b = %d, logical address = %p, ", b, &b);
    printf("physical address = %p\n", my_get_physical_addresses(&b));
    // thread local variable
    printf("tx: logical address = %p, ", &tx);
    printf("physical address = %p\n", my_get_physical_addresses(&tx));
    // heap
    printf("heap p: local address = %p, ", p);
    printf("physical address = %p\n", my_get_physical_addresses(p));
    printf("heap &p: local address = %p, ", &p);
    printf("physical address = %p\n", my_get_physical_addresses(&p));
    printf("heap p2: local address = %p, ", p2);
    printf("physical address = %p\n", my_get_physical_addresses(p2));
    printf("heap &p2: local address = %p, ", &p2);
    printf("physical address = %p\n", my_get_physical_addresses(&p2));
    // bss
    printf("bss c: local address = %p, ", &c);
    printf("physical address = %p\n", my_get_physical_addresses(&c));
    printf("bss k: local address = %p, ", &k);
    printf("physical address = %p\n", my_get_physical_addresses(&k));
    // function
    printf("hello: logical address = %p, ", hello);
    printf("physical address = %p\n", my_get_physical_addresses(hello));
    printf("func1: logical address = %p, ", func1);
    printf("physical address = %p\n", my_get_physical_addresses(func1));
    printf("func2: logical address = %p, ", func2);
    printf("physical address = %p\n", my_get_physical_addresses(func2));
    printf("func3: logical address = %p, ", func3);
    printf("physical address = %p\n", my_get_physical_addresses(func3));
    printf("main: logical address = %p, ", main);
    printf("physical address = %p\n", my_get_physical_addresses(main));
    // library function
    printf("printf: logical address = %p, ", printf);
    printf("physical address = %p\n", my_get_physical_addresses(printf));
    printf("====================================================================================================================\n");
}

void *func1(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);

    pthread_mutex_lock(&mutex);
    printf("I am thread with ID %d executing func1().\n", pid);
    hello(pid);
    pthread_mutex_unlock(&mutex);

    while (1)
    {
        // printf("(%d)(%s)\n",tx.id,tx.name) ;
        sleep(1);
    }
}

void *func2(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);

    pthread_mutex_lock(&mutex);
    printf("I am thread with ID %d executing func2().\n", pid);
    hello(pid);
    pthread_mutex_unlock(&mutex);

    while (1)
    {
        // printf("(%d)(%s)\n",tx.id,tx.name) ;
        sleep(2);
    }
}

void *func3(void *arg)
{
    char *p = (char *)arg;
    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, p);

    pthread_mutex_lock(&mutex);
    printf("I am thread with ID %d executing func3().\n", pid);
    hello(pid);
    pthread_mutex_unlock(&mutex);

    while (1)
    {
        // printf("(%d)(%s)\n",tx.id,tx.name) ;
        sleep(3);
    }
}

int main()
{
    p = malloc(10 * sizeof(int));
    p2 = malloc(20 * sizeof(int));

    pthread_t id[3];
    char p[3][16];
    strcpy(p[0], "Thread1");
    pthread_create(&id[0], NULL, func1, (void *)p[0]);
    strcpy(p[1], "Thread2");
    pthread_create(&id[1], NULL, func2, (void *)p[1]);
    strcpy(p[3], "Thread3");
    pthread_create(&id[2], NULL, func3, (void *)p[2]);

    int pid;
    pid = syscall(__NR_gettid);
    tx.id = pid;
    strcpy(tx.name, "MAIN");
    
    pthread_mutex_lock(&mutex);
    printf("I am main thread with ID %d.\n", pid);
    hello(pid);
    pthread_mutex_unlock(&mutex);

    while (1)
    {
        // printf("(%d)(%s)\n",tx.id,tx.name) ;
        sleep(5);
    }

    free(p);
    free(p2);
}