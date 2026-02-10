#include <assert.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO: right now the memory allocation is done outside of the Cmd and SimplePath structs for the
// actual chars stored in the char ** Will need to switch that to pre-allocation of some space for
// each pointer and copying data for manipulations (like appending)
// Main reasons:
//  - Want each Cmd/Path to be independent
//  - Want no risk of modifying read-only memory
//  - Easier management of who needs to free which pointer

typedef struct Cmd {
    const char **cmd;
    unsigned int num_items;
    unsigned int allocated;
} Cmd;

typedef struct SimplePath {
    const char **segments;
    unsigned int num_segments;
    unsigned int allocated;
} SimplePath;

typedef enum bool { false, true } bool;

bool are_arrays_strict_equal(const char *arr_1, const char *arr_2, int start_1, int start_2,
                             int len) {
    bool arrays_equal = true;

    for (int i = 0; i < len; i++) {
        if (arr_1[start_1 + i] != arr_2[start_2 + i]) {
            arrays_equal = false;
            return arrays_equal;
        }
    }

    return arrays_equal;
}

int get_fname_len(const char *filename) {
    int len = 0;
    bool search_finished = false;
    while (search_finished == false) {
        len++;

        if (filename[len] == 0) {
            search_finished = true;
        }
        if (len > MAXNAMLEN && search_finished == false) {
            printf("The name of the current file overstepped the defined filename boundaries\n");
            exit(EXIT_FAILURE);
        }
    }
    return len;
}

/**
 * Initializes a cmd struct with space for 100 arguments in the command.
 */
Cmd *init_cmd() {
    Cmd *init_cmd = malloc(sizeof(*init_cmd));
    if (init_cmd == NULL) {
        printf("Could not allocate memory for cmd struct\n");
        exit(EXIT_FAILURE);
    }

    init_cmd->cmd = calloc(100, sizeof(*(init_cmd->cmd)));
    if (init_cmd->cmd == NULL) {
        printf("Could not allocate memory for cmd \n");
        exit(EXIT_FAILURE);
    }

    init_cmd->num_items = 0;
    init_cmd->allocated = 100;
    return init_cmd;
}

void cmd_append(Cmd *command, const char *arg) {
    if (command->num_items >= command->allocated) {
        printf("More than %i arguments is not supported. Not appending.\n", command->allocated);
        return;
    }

    command->cmd[command->num_items] = arg;
    command->num_items++;
}

#define cmd_append_many(cmd, ...) _cmd_append_many(cmd, __VA_ARGS__, NULL)
void _cmd_append_many(Cmd *cmd, ...) {
    va_list args;
    va_start(args, cmd);
    for (;;) {
        const char *argument = va_arg(args, const char *);
        if (argument == NULL) {
            break;
        }
        cmd_append(cmd, argument);
    }
}

void cmd_add_null_terminator(Cmd *command) { cmd_append(command, NULL); }

void free_cmd(Cmd *command) {
    free(command->cmd);
    free(command);
}

void print_cmd(Cmd *cmd) {
    for (int i = 0; i < cmd->num_items - 1; i++) {
        printf("%s ", cmd->cmd[i]);
    }
    printf("%s", cmd->cmd[cmd->num_items - 1]);
    printf("\n");
}

int run_cmd(Cmd *cmd_2_run) {
    int sucess = 0;
    printf("Running command: ");
    print_cmd(cmd_2_run);
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        cmd_add_null_terminator(cmd_2_run);
        if (execvp(cmd_2_run->cmd[0], cmd_2_run->cmd) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        };
    } else if (pid > 0) {
        // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            sucess = 1;
        }
    } else {
        // failure
        printf("Failure of cmd: ");
        print_cmd(cmd_2_run);
        sucess = 0;
    }
    return sucess;
}

void path_add_segment(SimplePath *path, const char *seg) {
    if (path->num_segments >= path->allocated) {
        printf("More than %i arguments is not supported. Not appending.\n", path->allocated);
        return;
    }

    path->segments[path->num_segments] = seg;
    path->num_segments++;
}

void path_rm_last_seg(SimplePath *path) {
    path->segments[path->num_segments - 1] = NULL;
    path->num_segments = path->num_segments - 1;
}

void append_path_to_base(SimplePath *base_path, SimplePath *path_to_append) {
    for (int i = 0; i < path_to_append->num_segments; i++) {

        const char *seg = path_to_append->segments[i];
        path_add_segment(base_path, seg);
    }
}

/**
 * Copy a path to an already allocated buffer. The copied path is null terminated
 * @param SimplePath *path: path to be copied.
 * @param char *buffer: buffer in which to copy the path. It is assumed that the buffer is large
 * enough to hold the path.
 * */
void copy_path_to_buffer(SimplePath *path, char *buffer) {
    unsigned int prev_offset = 0;
    for (int i = 0; i < path->num_segments - 1; i++) {
        int len = get_fname_len(path->segments[i]);
        memcpy(buffer + prev_offset, path->segments[i], len);

        buffer[prev_offset + len] = '/';
        prev_offset += len + 1;
    }

    int len = get_fname_len(path->segments[path->num_segments - 1]);
    memcpy(buffer + prev_offset, path->segments[path->num_segments - 1], len);

    prev_offset += len;
    buffer[prev_offset] = 0;
}

/**
 * Get the num of characters needed to hold all characters in the path. This also accounts for the
 * null terminator at end of all chars.
 * @param SimplePath *path: Path we want to count characters of.
 * @return int: number of characters needed to hold the complete path.
 * */
int path_get_num_chars(SimplePath *path) {
    int num_chars = 0;
    for (int i = 0; i < path->num_segments; i++) {
        const char *segment = path->segments[i];
        num_chars += get_fname_len(segment) + 1;
    }
    return num_chars + 1;
}

/**
 * Initializes a Path struct with space for  segments in the path.
 *
 * @param bool dot_init: wether to start the path with '.' for executables
 */
SimplePath *init_path(bool dot_init) {
    SimplePath *init_path = malloc(sizeof(*init_path));
    if (init_path == NULL) {
        printf("Could not allocate memory for cmd struct\n");
        exit(EXIT_FAILURE);
    }

    init_path->segments = calloc(100, sizeof(*(init_path->segments)));
    if (init_path->segments == NULL) {
        printf("Could not allocate memory for segments \n");
        exit(EXIT_FAILURE);
    }

    init_path->allocated = 100;
    init_path->num_segments = 0;

    if (dot_init == true) {
        path_add_segment(init_path, ".");
    }
    return init_path;
}

void print_path(SimplePath *path) {
    for (int i = 0; i < path->num_segments - 1; i++) {
        printf("%s/", path->segments[i]);
    }
    printf("%s\n", path->segments[path->num_segments - 1]);
}

void free_path(SimplePath *path) {
    free(path->segments);
    free(path);
}
