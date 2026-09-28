#include <stdio.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc > 2) {
    fprintf(stderr, "Error: too many arguments.\nUsage: %s [filename]\n",
            argv[0]);
    return 2;
  }

  FILE *out = stdout;
  const char *filename = NULL;

  if (argc == 2) {
    filename = argv[1];

    if (access(filename, F_OK) == 0) {
      fprintf(stderr, "Warning: file '%s' exists, appending data.\n", filename);
    }

    out = fopen(filename, "a");
    if (!out) {
      perror("fopen");
      return 1;
    }
  }

  struct utsname sys_info;
  if (uname(&sys_info) < 0) {
    perror("uname");
    if (out != stdout)
      fclose(out);
    return 1;
  }

  time_t time_epoch = time(NULL);
  struct tm *time_info = localtime(&time_epoch);
  char time_string[40];

  if (time_info != NULL) {
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);
  } else {
    snprintf(time_string, sizeof(time_string), "N/A");
  }

  fprintf(out, "========================================\n");
  fprintf(out, " Hostname:  %s\n", sys_info.nodename);
  fprintf(out, " Time:      %s\n", time_string);
  fprintf(out, " OS:        %s %s\n", sys_info.sysname, sys_info.release);
  fprintf(out, " Hardware:  %s\n", sys_info.machine);
  fprintf(out, "========================================\n");

  if (out != stdout) {
    fclose(out);
  }

  return 0;
}