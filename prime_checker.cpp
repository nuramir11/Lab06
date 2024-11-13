#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstring>

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

void checker(int pipe_fd[2], int max_number) {
    close(pipe_fd[0]);
    for (int i = 2; i < max_number; i++) {
        if (is_prime(i)) {
            write(pipe_fd[1], &i, sizeof(i));
        }
    }
    close(pipe_fd[1]);
    std::cout << "checker: I will now finish" << std::endl;
    _exit(0);
}

void printer(int pipe_fd[2]) {
    close(pipe_fd[1]);
    int number;
    while (read(pipe_fd[0], &number, sizeof(number)) > 0) {
        std::cout << "printer: " << number << std::endl;
    }
    close(pipe_fd[0]);
    std::cout << "printer: I will now finish" << std::endl;
    _exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <max_number>" << std::endl;
        return 1;
    }

    int max_number = std::stoi(argv[1]);
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t checker_pid = fork();
    if (checker_pid == 0) {
        checker(pipe_fd, max_number);
    }

    pid_t printer_pid = fork();
    if (printer_pid == 0) {
        printer(pipe_fd);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    waitpid(checker_pid, nullptr, 0);
    std::cout << "parent: checker finished" << std::endl;

    waitpid(printer_pid, nullptr, 0);
    std::cout << "parent: printer finished" << std::endl;

    return 0;
}
