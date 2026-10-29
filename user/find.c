#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"

// 路径拼接：把 child 拼到 parent 后面
void join_path(char *buf, char *parent, char *child) {
    strcpy(buf, parent);
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] != '/') {
        buf[len] = '/';
        buf[len + 1] = '\0';
    }
    strcpy(buf + strlen(buf), child);
}

// 获取路径的最后一部分（文件名）
char* get_basename(char *path) {
    char *p = path;
    char *last_slash = 0;
    while (*p) {
        if (*p == '/') last_slash = p;
        p++;
    }
    return last_slash ? last_slash + 1 : path;
}

void find(char *path, char *target) {
    char buf[512];
    int fd;
    struct stat st;
    struct dirent de;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            // 如果是普通文件，检查名字是否匹配
            if (strcmp(get_basename(path), target) == 0) {
                printf("%s\n", path);
            }
            break;

        case T_DIR:
            // 如果是目录，读取里面的目录项
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue; // 空槽位
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue; // 跳过特殊目录
                
                join_path(buf, path, de.name);
                find(buf, target); // 递归调用
            }
            break;
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
