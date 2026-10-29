#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// 辅助函数：判断字符是不是分隔符
int is_separator(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
           c == '-' || c == '.' || c == '/' || c == ',';
}

void process_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "sixfive: cannot open %s\n", filename);
        return;
    }

    char c;
    long long num = 0;
    int in_num = 0;

    while (read(fd, &c, 1) == 1) {
        if (c >= '0' && c <= '9') {
            // 是数字，累加
            num = num * 10 + (c - '0');
            in_num = 1;
        } else if (is_separator(c)) {
            // 是分隔符，且刚才在拼数字，就结算
            if (in_num) {
                if (num % 5 == 0 || num % 6 == 0) {
                    printf("%lld\n", num);
                }
                num = 0;
                in_num = 0;
            }
        } else {
            // 是其他字符（字母等），说明数字中断了，丢弃
            num = 0;
            in_num = 0;
        }
    }

    // 文件末尾的结算（如果文件最后是个数字，没有空格）
    if (in_num) {
        if (num % 5 == 0 || num % 6 == 0) {
            printf("%lld\n", num);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: sixfive [file...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        process_file(argv[i]);
    }

    exit(0);
}
