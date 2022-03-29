#include "my_exec.h"
 
#define MAXLINE 1024
#define MAXARGS 128


char theme[] = "Here to control my file system";

void eval(char *cmdline);
int parseline(const char *cmdline, char **argv);

void print_version() {
  char load[] = "....................";
  char ch[20];
  printf("Welcome to my file system\n\n");
  for (int i = 0; i < 19; i++) {
    memset(ch, 0, sizeof(ch));
    strncpy(ch, load, i + 1);
    printf("loading %s\n", ch);
    fflush(stdout);
    usleep(100000);
    printf("\033[1A\033[K");
  }
  printf("loading ....................\n\n");
}

// print usage info
void print_usage(void) {
  printf("Usage: ./myfs [options]\n");
  printf("Options:\n");
  printf("\t-h: print this massges\n");
  printf("\t-p: hide the prompt\n");
  printf("\t-d: use developer mode\n");
  printf("\tdefault: start shell\n");
}

int Start_Shell(int argc, char **argv) {
  int ret;
  char ch;
  // command line 
  char cmdline[MAXLINE];
  // whether print a prompt, default yes
  int emit_prompt = 1;

  // parse the command line 
  while ((ch = getopt(argc, argv, "hpd")) != EOF) {
    switch(ch) {
    case 'h':
      print_usage();
      exit(0);
    case 'p':
      emit_prompt = 0;
      break;
    case 'd':
      developer = 0;
      break;
    default:
      print_usage();
      exit(1);
    }
  }

  // login 
  print_version();
  while (login() != FS_LOGIN) {}
  ret = load_super_block();
  if (ret == FS_RD_ERROR)
    return FS_RD_ERROR;

  printf("Welcome to my shell!\n");
  printf("%s\n", theme);
  printf("\n");

  // excute the shell's read/eval loop 
  while (1) {

    // print prompt 
    if (emit_prompt) {
      if (developer == 0)
        printf("\e[1;31m[Developer mode] \e[0m");
      printf("\e[1;32m%s \e[0m", path);
      fflush(stdout);
    }
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      printf("fgets error\n");
      exit(1);
    }
    if (feof(stdin)) {
      printf("[EXIT] Parser reached end-of-file. Terminated!\n");
      fflush(stdout);
      exit(0);
    }

    // evaluate the command line 
    eval(cmdline);
    fflush(stdout);
  }

  return 0;
}

// evaluate the command line whether user requset a built-in command
void eval(char *cmdline) {
  char *argv[MAXARGS];
  char buf[MAXLINE];
  int argc;

  // parse command 
  strcpy(buf, cmdline);
  argc = parseline(buf, argv);

  if (argv[0] == NULL)
    return;
  if (!developer_cmd(argc, argv)) {
    if (!builtin_cmd(argv)) {
      if (!main_execute(argv[0], argc, argv))
        printf("sh: command not found: %s\n", argv[0]);
      // else
        // printf("[INFO] Operation finished.\n\n");
    }
  }
}

// parse the command line and build the argv array.
int parseline(const char *cmdline, char **argv) {
  // holds local copy of command line 
  static char array[MAXLINE];
  char *buf = array;
  char *delim;
  int argc;
  
  strcpy(buf, cmdline);
  buf[strlen(buf) - 1] = ' ';
  // ignore leading spaces 
  while (*buf && (*buf == ' '))
    buf++;

  // build the argv list 
  argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
  }
  else {
    delim = strchr(buf, ' ');
  }

  while(delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' '))
      buf++;
    
    if (*buf == '\'') {
      buf++;
      delim = strchr(buf, '\'');
    }
    else {
      delim = strchr(buf, ' ');
    }
  }
  argv[argc] = NULL;
  return argc;
}
