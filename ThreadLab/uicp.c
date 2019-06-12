#include "includes.h"
#define _GNU_SOURCE

struct path {
  char *fileInput;
  char *fileOutput;
};

int bytecount = 0;
int byteTotal = 0;
int filecount = 0;
int fileTotal = 0;
int cancel = 0;

int isDirectory(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0)
    return 0;
  return S_ISDIR(statbuf.st_mode);
}

int isFile(const char *path) {
  struct stat statbuf;
  stat(path, &statbuf);
  return S_ISREG(statbuf.st_mode);
}

void countFile(char *arg1, char *arg2) {
  DIR *d;
  struct dirent *dir;
  struct stat s;
  stat(arg1, &s);
  char *path1 = malloc(sizeof(char) * 500);
  char *path2 = malloc(sizeof(char) * 500);
  d = opendir(arg1);

  mkdir(arg2, 0777);

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      printf("\n#####\n");
      printf("%s\n", dir->d_name);
      printf("#####\n");

      if (dir->d_type == DT_DIR) {
        if (dir->d_name[0] != '.') {
          printf("É DIRETORIO\n");
          sprintf(path1, "%s/%s", arg1, dir->d_name);
          sprintf(path2, "%s/%s", arg2, dir->d_name);
          printf("PATH2: %s\n", path2);
          countFile(path1,path2);
        }
      } else {
        fileTotal++;
      }
    }
    closedir(d);
  }
}

void copyPaste(char *arg1, char *arg2) {
  DIR *d;
  struct dirent *dir;
  struct stat s;
  stat(arg1, &s);
  char *path1 = malloc(sizeof(char) * 500);
  char *path2 = malloc(sizeof(char) * 500);
  d = opendir(arg1);

  mkdir(arg2, 0777);

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      printf("\n#####\n");
      printf("%s\n", dir->d_name);
      printf("#####\n");

      if (dir->d_type == DT_DIR) {
        if (dir->d_name[0] != '.') {
          printf("É DIRETORIO\n");
          sprintf(path1, "%s/%s", arg1, dir->d_name);
          sprintf(path2, "%s/%s", arg2, dir->d_name);
          printf("PATH2: %s\n", path2);
          copyPaste(path1, path2);
        }
      } else {
        sprintf(path1, "%s/%s", arg1, dir->d_name);
        sprintf(path2, "%s/%s", arg2, dir->d_name);
        printf("É ARQUIVO\n");
        copyFile(path1, path2);
      }
    }
    closedir(d);
  }
}

int removeAll(char arg[1024]) {
  DIR *direc = opendir(arg);
  size_t len = strlen(arg);
  int read = -1;

  if (direc) {
    struct dirent *p;
    read = 0;
    
    while (!read && (p = readdir(direc))) {
        int r2 = -1;
        char *buf;
        size_t len2;

        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            continue;

        len2 = len + strlen(p->d_name) + 2;
        buf = malloc(len2);

        if (buf) {
            struct stat statbuf;

            snprintf(buf, len2, "%s/%s", arg, p->d_name);

            if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode)) {
                    r2 = removeAll(buf);
                } else {
                    r2 = unlink(buf);
                }
            }
            free(buf);
        }
        read = r2;
    }
    closedir(direc);
  }

  if (!read)
    read = rmdir(arg);

  return read;
}

int remove_directory(char path[1024]) {
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int r = -1;

  if (d) {
    struct dirent *p;
    r = 0;
    while (!r && (p = readdir(d))) {
      int r2 = -1;
      char *buf;
      size_t len;

      // Cortar fora os dots e dot-dots
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
        continue;
      }

      len = path_len + strlen(p->d_name) + 2;
      buf = malloc(len);

      if (buf) {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf)) {
          if (S_ISDIR(statbuf.st_mode)) {
            // Recursivo
            r2 = remove_directory(buf);
          } else {
            r2 = unlink(buf);
          }
        }

        free(buf);
      }

      r = r2;
    }

    closedir(d);
  }

  if (!r) {
    r = rmdir(path);
  }

  return r;
}

void copyFile(char *arg1, char *arg2) {
  struct stat s;
  stat(arg1, &s);
  byteTotal = s.st_size;    
  bytecount = 0;
  int file1, file2;
  char buffer[256];
  char buffer2[256];

  sprintf(buffer, "%s", arg1);
  sprintf(buffer2, "%s", arg2);

  printf("A1: %s\n", arg1);
  printf("A2: %s\n", arg2);
  if (access((arg1), F_OK) != -1) {
    file1 = open(buffer, O_RDONLY, 0666);
    char *buf[1];
    file2 = open(buffer2, O_CREAT | O_RDWR, 0666);
    while (read(file1, buf, 1) > 0 && !cancel) {
      write(file2, buf, 1);
      bytecount++;
    }
  }

  filecount ++;

  close(file1);
  close(file2);
}

void draw(char *Outputpath) {
  int screenWidth = 800;
  int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "raylib [core] example - input mouse wheel");
  int boxPositionY = screenHeight / 4;
  int boxPositionX = 20;
  int cancelX = screenWidth - 108;
  char bytesBuffer[200];
  char fileBuffer[200];
  char testBuffer[200];
  int cancelY = screenHeight - 50;

  SetTargetFPS(60);
  // Main game loop
  while (!WindowShouldClose()) {
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    float progressByte = (float) bytecount / byteTotal;
    float progressFile = (float) filecount / fileTotal;
    BeginDrawing();
    // Detect window close button or ESC key
    ClearBackground(RAYWHITE);
    
    sprintf(fileBuffer, "Arquivos: %d / %d files", filecount, fileTotal);
    DrawText(fileBuffer, 15, boxPositionY + 78, 20, GRAY);
    DrawRectangle(20, boxPositionY + 100, (screenWidth-20)*progressFile - 100, 50, GREEN);
    
    sprintf(bytesBuffer, "Tamanho Arquivo: %d / %d bytes", bytecount, byteTotal);
    DrawText(bytesBuffer, 15, (boxPositionY - 22), 20, GRAY);
    DrawRectangle(20, (boxPositionY), (screenWidth-20)*progressByte - 100, 50, GREEN);

    DrawRectangle(cancelX, cancelY, 100, 40, RED);
    DrawText("Cancelar!", screenWidth - 110 + 5, screenHeight - 50 + 10, 20, WHITE);

    if(mouseX >= cancelX && mouseX <= (cancelX + 100)){
        if(mouseY >= cancelY && mouseY <= (cancelY + 40)){
            if(IsMouseButtonPressed(0)){
                sprintf(testBuffer,"Pos: %d X %d Y", mouseX, mouseY);
                DrawText(testBuffer,10,10,20,GREEN);
                removeAll(Outputpath);
                //remove_directory(Outputpath);
                cancel = 1;
            }
        }
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();
  // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
  return 0;
}

void *cp_thread(void *args) {
  printf("\nCopy thread..\n");
  struct path *p = args;

  printf("%s\n", p->fileInput);
  printf("%s\n", p->fileOutput);
  
  if (isFile(p->fileInput)) {
    fileTotal = 1;
    copyFile(p->fileInput, p->fileOutput);
  } else {
    if (strchr(p->fileOutput,'.') != NULL){
        printf("Erro pasta para arquivo!\n");
        exit(-1);
    }
    else{
        countFile(p->fileInput, p->fileOutput);
        copyPaste(p->fileInput, p->fileOutput);
    }
  }
}

int main(int argc, char const *argv[]) {
  pthread_t copyT;
  struct path p;
  cancel = 0;
  char *a1 = argv[1];
  char *a2 = argv[2];
  printf("%s", a1);
  p.fileInput = a1;
  p.fileOutput = a2;

  int errorCopy = pthread_create(&copyT, NULL, cp_thread, &p);

  draw(p.fileOutput);

  pthread_join(copyT, NULL);

  return 0;
}
