#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int MAX_THREAD_NUM = 50;


// F(0) = 0, F(1) = 1, F(n) = F(n-1) + F(n-2)
class Thread_Args
{
public:
    int n;
    int result;
    int threadID;
};

void *fibonacci(void *arg) {
    // 把参数转换为Thread_Args类型
    Thread_Args *args = (Thread_Args *)arg;


    if (args->n == 0 || args->n == 1){
        args->result = args->n;
        return NULL;
    }
    
    // 检查是否超过最大线程数
    if (args->threadID >= MAX_THREAD_NUM) {
        // 用循环代替递归
        int a = 0, b = 1;
        for (int i = 2; i <= args->n; i++) {
            int temp = a + b;
            a = b;
            b = temp;
        }
        args->result = b;
        return NULL;
    }

    // 创建两个线程
    // 这里ID的计算方式是父线程ID * 2 和 父线程ID * 2 + 1 有一点tricky的地方
    // 初始化的时候，父线程ID是1，所以两个子线程的ID分别是2和3

    // 然后2的子线程ID是4和5，3的子线程ID是6和7
    // 以此类推，每个线程的子线程ID都是父线程ID * 2 和 父线程ID * 2 + 1
    Thread_Args args1, args2;
    args1.n = args->n - 1;
    args1.threadID = args->threadID * 2;
    args2.n = args->n - 2;
    args2.threadID = args->threadID * 2 + 1;

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, fibonacci, &args1);
    pthread_create(&thread2, NULL, fibonacci, &args2);

    // 等待两个线程结束
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 计算结果
    args->result = args1.result + args2.result;
    return NULL;
}



int main() {
    Thread_Args argInit;
    argInit.n = 30;
    argInit.threadID = 1;
    fibonacci(&argInit);
    cout << "The 30th fibonacci number is " << argInit.result << endl;
    return 0;
}
