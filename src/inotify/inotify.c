//
// Created by Renato Böhler on 23/10/18.
//
// To compile: gcc inotify.c -lrt
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))
#define WATCH_ROOT_DIR "/home/pi/.wildradio/config"
#define PRINCIPAL 1
#define ALTERNATIVA 0

typedef struct camera_config
{
  int ativa;
  int temporizador;
  int presenca;
  int horizontal;
  int vertical;
  int fotoConfirmacao;
};

void monitor(char *);
int evnt_mon(char *);

int *debouncing;

void debounceDiffConfigs(struct camera_config old, struct camera_config new) {
  int local_debouncing = ++(*debouncing);
  if (fork() == 0) {
    sleep(2);
    if (local_debouncing == *debouncing) {
      *debouncing = 0;
      diffConfigs(old, new);
    }
    exit(0);
  }
}

void diffConfigs(struct camera_config old, struct camera_config new) {
  printf("\n\nLast configuration:\n\n");
  printConfig(old);
  printf("\n\n---\n\nNew configuration:\n\n");
  printConfig(new);

  old = new;
}

char* readFile(char *file_name) {
  // Configuration file won't be larger than 20 chars
  int max_buffer_length = 20;
  char *source = malloc(max_buffer_length);

  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    return;
  }

  size_t newLen = fread(source, sizeof(char), max_buffer_length, fp);
  if (ferror(fp) != 0) {
    return;
  }

  fclose(fp);

  return source;
}

struct camera_config loadConfigFile(char *file_name) {
  struct camera_config configuration;

  char* file_content = readFile(file_name);

  // First line of the file (0 or 1)
  configuration.ativa = file_content[0] - '0';

  // Second line of the file (0 or 1)
  configuration.temporizador = file_content[2] - '0';

  // Third line of the file (0 or 1)
  configuration.presenca = file_content[4] - '0';

  // Fourth line of the file (-60 to 60)
  char row[3];
  row[0] = file_content[6]; // -   or 0-9
  row[1] = file_content[7]; // 0-9 or \n
  row[2] = file_content[8]; // 0-9 or \n

  int multiplier = row[0] == '-' ? -1 : 1;
  char firstDigit = row[0] == '-' ? row[1] : row[0];
  char secondDigit = row[0] == '-' ? row[2] : row[1];

  int firstDigitInt = firstDigit - '0';
  int secondDigitInt = secondDigit - '0';
  if (secondDigit != '\n') {
    firstDigitInt *= 10;
  } else {
    secondDigitInt = 0;
  }

  configuration.horizontal = multiplier * (firstDigitInt + secondDigitInt);

  // Fifth line of the file (-60 to 60)
  int cursor = 10;
  if (row[1] == '\n') {
    cursor = 8;
  } else if (row[2] == '\n') {
    cursor = 9;
  }

  row[0] = file_content[cursor];     // -   or 0-9
  row[1] = file_content[cursor + 1]; // 0-9 or \n
  row[2] = file_content[cursor + 2]; // 0-9 or \n

  multiplier = row[0] == '-' ? -1 : 1;
  firstDigit = row[0] == '-' ? row[1] : row[0];
  secondDigit = row[0] == '-' ? row[2] : row[1];

  firstDigitInt = firstDigit - '0';
  secondDigitInt = secondDigit - '0';
  if (secondDigit != '\n') {
    firstDigitInt *= 10;
  } else {
    secondDigitInt = 0;
  }

  configuration.vertical = multiplier * (firstDigitInt + secondDigitInt);

  // Sixth line of the file (0 or 1)
  if (row[1] == '\n') {
    cursor = cursor + 2;
  } else if (row[2] == '\n') {
    cursor = cursor + 3;
  } else {
    cursor = cursor + 4;
  }

  configuration.fotoConfirmacao = file_content[cursor] - '0';

  return configuration;
}

void printConfig(struct camera_config c) {
  printf("Ativa: %d\n", c.ativa);
  printf("Temporizador: %d\n", c.temporizador);
  printf("Presenca: %d\n", c.presenca);
  printf("Horizontal: %d\n", c.horizontal);
  printf("Vertical: %d\n", c.vertical);
  printf("Foto confirmacao: %d\n", c.fotoConfirmacao);
}

void main() {
  // Shared memory to perform function call debouncing
  char *shm_name = "/debounce.diff.configs";
  int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0644);
  ftruncate(fd, sizeof(int));
  debouncing = (int *) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  // Watch the root directory on a new process
  if (fork() == 0) {
    evnt_mon(WATCH_ROOT_DIR);
  }

  // Recursively watch subdirectories
  monitor(WATCH_ROOT_DIR);

  while (1);
}

void monitor(char *rt_dir) {
  // Setup
  struct stat st;
  DIR *dirp;
  struct dirent *dp;
  char str[100][100] = {};
  char temp[100];
  char str1[500] = " ";
  int i = 0, j = 0, src_ret = 9, src_ret1 = 9;
  strcpy(str1, rt_dir);

  // Open the directory
  dirp = opendir(str1);
  if (dirp == NULL) {
    perror("opendir");
    return;
  }

  // Reads all the subdirectories
  while (1) {
    dp = readdir(dirp);
    
    // No more subdirectories
    if (dp == NULL) {
      break;
    }

    // "." and ".."
    if ((strcmp(dp->d_name, ".\0") == 0) || (strcmp(dp->d_name, "..") == 0)) {
      continue;
    }

    // Valid directory: watch it on another process
    if ((dp->d_type == DT_DIR) &&
        ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))) {
      strcat(str[i], str1);
      strcat(str[i], "/");
      strcat(str[i], dp->d_name);
      if (fork() == 0) {
        evnt_mon(str[i]);
      }
      i++;
    }
  }

  // Closes the directory and recursively add watches to its subdirectories
  closedir(dirp);
  if (i > 0) {
    for (j = 0; j < i; j++) {
      monitor(str[j]);
    }
  }
}

int evnt_mon(char *argv) {
  // Initial file contents
  struct camera_config last_configurations[2];

  char main_camera_path[100] = "";
  strcat(main_camera_path, argv);
  strcat(main_camera_path, "/principal");
  last_configurations[PRINCIPAL] = loadConfigFile(main_camera_path);

  char alternative_camera_path[100] = "";
  strcat(alternative_camera_path, argv);
  strcat(alternative_camera_path, "/alternativa");
  last_configurations[ALTERNATIVA] = loadConfigFile(alternative_camera_path); 

  // Setup
  int length, i = 0, wd;
  int fd;
  char buffer[BUF_LEN];

  // inotify initialization
  fd = inotify_init();
  if (fd < 0) {
    perror("Couldn't initialize inotify");
  }

  wd = inotify_add_watch(fd, argv, IN_CREATE | IN_MODIFY);

  if (wd == -1) {
    printf("Couldn't add watch to %s\n", argv);
  } else {
    printf("Watching directory: %s\n", argv);
  }

  while (1) {
    // Wait for events
    i = 0;
    length = read(fd, buffer, BUF_LEN);

    if (length < 0) {
      perror("read");
    }

    while (i < length) {
      struct inotify_event *event = (struct inotify_event *)&buffer[i];
      if (event->len) {
        if (event->mask & IN_CREATE) {
          if (event->mask & IN_ISDIR) {
            // New directory, watch it aswell
            if (fork() == 0) {
              char p[100] = " ";
              strcpy(p, argv);
              strcat(p, "/");
              strcat(p, event->name);
              evnt_mon(p);
            }
          }
        }

        if (event->mask & IN_MODIFY && !(event->mask & IN_ISDIR)) {
          char full_path[100] = "";
          strcat(full_path, argv);
          strcat(full_path, "/");
          strcat(full_path, event->name);
          printf("File modified: %s\n", full_path);

          int camera_type = full_path[strlen(full_path) - 1] == 'l' ? PRINCIPAL : ALTERNATIVA;
          struct camera_config new_configuration = loadConfigFile(full_path);

          debounceDiffConfigs(last_configurations[camera_type], new_configuration);
        }

        i += EVENT_SIZE + event->len;
      }
    }
  }

  // Cleanup
  inotify_rm_watch(fd, wd);
  close(fd);

  return 0;
}