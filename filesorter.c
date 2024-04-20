#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH_LENGTH 260

void handleError(const char* message) {
    printf("Error: %s\n", message);
    printf("Error code: %lu\n", GetLastError());
}

BOOL createDirectory(const char* path) {
    if (!CreateDirectory(path, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            handleError("Failed to create directory");
            return FALSE;
        }
    }
    printf("Directory created: %s\n", path);
    return TRUE;
}

BOOL copyFile(const char* src, const char* dest) {
    if (!CopyFile(src, dest, FALSE)) {
        handleError("Failed to copy file");
        return FALSE;
    }
    printf("File copied: %s -> %s\n", src, dest);
    return TRUE;
}

int main() {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    const char* sourceDirectory = "C:\\path\\to\\source\\directory\\";
    const char* picsDirectory = "C:\\path\\to\\pics\\directory\\";
    const char* videoDirectory = "C:\\path\\to\\video\\directory\\";
    const char* otherDirectory = "C:\\path\\to\\other\\directory\\";

    const char* picExtensions[] = { "jpg", "jpeg", "gif", "png", "bmp" };
    const char* videoExtensions[] = { "avi", "mov", "mp4", "mkv", "wmv", "flv" };

    char searchPattern[MAX_PATH_LENGTH];
    sprintf(searchPattern, "%s*.*", sourceDirectory);
    hFind = FindFirstFile(searchPattern, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error searching directory\n");
        return 1;
    }

    do {
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char* extension = strrchr(findFileData.cFileName, '.');
            if (extension != NULL) {
                char destDirectory[MAX_PATH_LENGTH];
                const char* filename = findFileData.cFileName;
                BOOL found = FALSE;

                for (int i = 0; i < sizeof(picExtensions) / sizeof(picExtensions[0]); i++) {
                    if (strcmp(extension + 1, picExtensions[i]) == 0) {
                        sprintf(destDirectory, "%s", picsDirectory);
                        found = TRUE;
                        break;
                    }
                }

                if (!found) {
                    for (int i = 0; i < sizeof(videoExtensions) / sizeof(videoExtensions[0]); i++) {
                        if (strcmp(extension + 1, videoExtensions[i]) == 0) {
                            sprintf(destDirectory, "%s", videoDirectory);
                            found = TRUE;
                            break;
                        }
                    }
                }

                if (!found) {
                    sprintf(destDirectory, "%s", otherDirectory);
                }

                char sourcePath[MAX_PATH_LENGTH];
                char destPath[MAX_PATH_LENGTH];
                sprintf(sourcePath, "%s%s", sourceDirectory, filename);
                sprintf(destPath, "%s%s", destDirectory, filename);

                if (!copyFile(sourcePath, destPath)) {
                    return 1;
                }
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    return 0;
}
