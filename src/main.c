#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int compiler(char *argv[])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("compiler fork error");
        return -1;
    }
    else if (pid == 0)
    {
        return execvp("gcc", argv);
    }
    else
    {
        return waitpid(pid, NULL, 0);
    }
}

int run(const char *filename, char *argv[])
{
    return execv(filename, argv);
}

char *get_rand_name(int len)
{
    srand((unsigned)time(NULL));
    char *s = (char *)malloc(len * sizeof(char) + strlen("/tmp/") + 1);
    char *letter = "abcdefghijklmnopqrstuvwxyz";
    char *prefix = "/tmp/";
    int prefix_len = strlen("/tmp/");
    int i = 0;
    for (i = 0; i < len + prefix_len; i++)
    {
        if (i < prefix_len)
        {
            s[i] = prefix[i];
        }
        else
        {
            s[i] = letter[rand() % 26];
        }
    }
    s[i] = '\0';
    return s;
}

int main(int argc, char *argv[])
{
    int build_argv_len = 0;
    int exec_argv_len = 0;
    int is_c_files = 0;
    for (int i = 1; i < argc; i++)
    {
        if (is_c_files != 1)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                printf("goc can't use -o build flags\n");
                return -1;
            }
            if (strstr(argv[i], ".c") != NULL)
            {
                is_c_files = 1;
                i = i - 1;
            }
        }
        else
        {
            if (strstr(argv[i], ".c") == NULL)
            {
                i = i - 1;
                build_argv_len = i;
                exec_argv_len = argc - 1 - i;
                break;
            }
            if ((i + 1) >= argc)
            {
                build_argv_len = i;
                exec_argv_len = argc - 1 - i;
                break;
            }
        }
    }
    if (is_c_files != 1)
    {
        printf("args error can't find c files\n");
        return -1;
    }
    char *build_argv[build_argv_len + 4];
    char *exec_argv[exec_argv_len + 2];
    char *exe = get_rand_name(12);
    build_argv[0] = "gcc";
    build_argv[build_argv_len + 1] = "-o";
    build_argv[build_argv_len + 2] = exe;
    build_argv[build_argv_len + 3] = NULL;
    exec_argv[0] = exe;
    exec_argv[exec_argv_len + 1] = NULL;
    for (int i = 1; i < argc; i++)
    {
        if (i <= build_argv_len)
        {
            build_argv[i] = argv[i];
        }
        else
        {
            exec_argv[i - build_argv_len] = argv[i];
        }
    }
    // printf("%d build argc %d exec argc %d\n", argc, build_argv_len, exec_argv_len);
    // printf("build argv:\n");
    // for (int i = 0; i < build_argv_len + 4; i++)
    // {
    //     if (build_argv[i] != NULL)
    //         printf("%s\n", build_argv[i]);
    // }
    // printf("exec argv:\n");
    // for (int i = 0; i < exec_argv_len + 2; i++)
    // {
    //     if (exec_argv[i] != NULL)
    //         printf("%s\n", exec_argv[i]);
    // }
    if (compiler(build_argv) < 0)
    {
        printf("compiler error\n");
        return -1;
    }
    if (run(exe, exec_argv) < 0)
    {
        printf("run error");
        return -1;
    }
    if (unlink(exe) < 0)
    {
        printf("unlink error");
        return -1;
    }
    return 0;
}