// 统计文件夹内所有文件中有内容的行数

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int main()
{
    const char* folderPath = ".\\TestSplit"; // 指定文件夹路径
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*.*", folderPath);

    hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("无法打开文件夹: %s\n", folderPath);
        return 1;
    }

    int totalLineCount = 0;

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 忽略子目录
            continue;
        }

        char filePath[MAX_PATH];
        snprintf(filePath, MAX_PATH, "%s\\%s", folderPath, findFileData.cFileName);

        FILE* file = fopen(filePath, "r");
        if (!file) {
            printf("无法打开文件: %s\n", filePath);
            continue;
        }

        int lineCount = 0;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file)) {
            // 检查是否为非空行
            int isNonEmptyLine = 0;
            for (char* p = buffer; *p != '\0'; p++) {
                if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
                    isNonEmptyLine = 1;
                    break;
                }
            }
            if (isNonEmptyLine) {
                lineCount++;
            }
        }

        fclose(file);

        printf("文件: %s, 有内容的行数: %d\n", findFileData.cFileName, lineCount);
        totalLineCount += lineCount;

    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    printf("总有内容的行数: %d\n", totalLineCount);

    return 0;
}