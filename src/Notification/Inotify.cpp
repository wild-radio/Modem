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
#include "Inotify.hpp"
#include "EventMonitor.hpp"

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define WATCH_ROOT_DIR "/home/clemente/.wildradio/config"
#define MAX_FILE_PATH_LENGTH 100
#define MAX_CONFIG_FILE_LENGTH 20

char* Inotify::readFile(char *file_name) {
  char *source = new char[MAX_CONFIG_FILE_LENGTH];

  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    return NULL;
  }

  size_t newLen = fread(source, sizeof(char), MAX_CONFIG_FILE_LENGTH, fp);
  if (ferror(fp) != 0) {
    return NULL;
  }

  fclose(fp);

  return source;
}

char* Inotify::getSharedMemoryName(char *full_path) {
  char path[MAX_FILE_PATH_LENGTH];
  strcpy(path, full_path);

  char *shm_name = strstr(path, "/config/");
  shm_name += strlen("/config");

  int i;
  for(i = strlen(shm_name) - 1; i--; i > 0) {
    if (shm_name[i] == '/') {
      shm_name[i] = '.';
      break;
    }
  }

  return shm_name;
}

void Inotify::storeConfig(char *full_path, struct camera_config new_config) {

  char *shm_name = getSharedMemoryName(full_path);

  int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0644);
  ftruncate(fd, sizeof(struct camera_config));
  struct camera_config *old = (struct camera_config *) mmap(NULL, sizeof(struct camera_config), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  old->ativa = new_config.ativa;
  old->temporizador = new_config.temporizador;
  old->presenca = new_config.presenca;
  old->horizontal = new_config.horizontal;
  old->vertical = new_config.vertical;
  old->fotoConfirmacao = new_config.fotoConfirmacao;
}

struct camera_config Inotify::getConfig(char *full_path) {
  char *shm_name = getSharedMemoryName(full_path);

  int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0644);
  ftruncate(fd, sizeof(struct camera_config));
  struct camera_config *shm_config = (struct camera_config *) mmap(NULL, sizeof(struct camera_config), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

  struct camera_config config;

  config.ativa = shm_config->ativa;
  config.temporizador = shm_config->temporizador;
  config.presenca = shm_config->presenca;
  config.horizontal = shm_config->horizontal;
  config.vertical = shm_config->vertical;
  config.fotoConfirmacao = shm_config->fotoConfirmacao;

  return config;
}

void Inotify::diffConfigs(struct camera_config new_config, char* full_path) {
  struct camera_config old = getConfig(full_path);
	auto camera_id = this->getCameraId(full_path);

  if (old.ativa != new_config.ativa) {
    if (new_config.ativa) {
      printf("%s\tCâmera ativada\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::ACTIVATE, camera_id));
    } else {
      printf("%s\tCâmera desativada\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::INACTIVATE, camera_id));
    }
  }

  if (old.temporizador != new_config.temporizador) {
    if (new_config.temporizador) {
      printf("%s\tTemporizador ativado\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::ENABLE_TIMER, camera_id));
    } else {
      printf("%s\tTemporizador desativado\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::DISABLE_TIMER, camera_id));
    }
  }

  if (old.presenca != new_config.presenca) {
    if (new_config.presenca) {
      printf("%s\tPresenca ativada\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::ENABLE_SENSOR, camera_id));
    } else {
      printf("%s\tPresenca desativada\n", full_path);
	    this->notifications_queue->post(Notification(NotificationType::DISABLE_SENSOR, camera_id));
    }
  }

  if (old.horizontal != new_config.horizontal || old.vertical != new_config.vertical) {
    printf("%s\tAngulo alterado para (%d, %d)\n", full_path, new_config.horizontal, new_config.vertical);
	  this->notifications_queue->post(Notification(NotificationType::NEW_ANGLE, new_config.horizontal, new_config.vertical, camera_id));
  }

  if (new_config.fotoConfirmacao) {
    printf("%s\tFoto de confirmacao requisitada\n", full_path);
	  std::cout << this->notifications_queue << std::endl;
	  this->notifications_queue->post(Notification(NotificationType::REQUEST_CAPTURE, camera_id));
  }

  storeConfig(full_path, new_config);
}

void Inotify::debounceDiffConfigs(struct camera_config new_config, char* full_path) {
  int local_debouncing = ++(*debouncing);

  if (fork() == 0) {
    sleep(2);
    if (local_debouncing == *debouncing) {
      *debouncing = 0;
      diffConfigs(new_config, full_path);
    }
    exit(0);
  }
}


void Inotify::run() {
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
  EventMonitor monitor;
	monitor.monitor(WATCH_ROOT_DIR, std::__cxx11::string());

  while (1);
}

int Inotify::evnt_mon(char *argv) {
}

int Inotify::getCameraId(char *string) {
	return string[strlen(string) - 1] != 'L';
}
