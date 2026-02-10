/* Copy pasting source code directly for this file because it allows for easier compilation for a
 * small script*/
#include "./my_utils.c"

#include <assert.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool recursive_search_for_file(const char *filename, SimplePath *curr_path, char *curr_dir_name) {
    int t_name_len;
    t_name_len = get_fname_len(filename);

    // a buffer of 1024 is enough for a minimum of 3 subdirectories
    char dir_path[1024];
    copy_path_to_buffer(curr_path, dir_path);

    DIR *curr_dir = opendir(dir_path);
    if (curr_dir == NULL) {
        printf("Could not open directory: %s\n", dir_path);
        exit(EXIT_FAILURE);
    }

    struct dirent *dir_entry;
    while ((dir_entry = readdir(curr_dir)) != NULL) {

        // case of finding a directory that is not "." or ".."
        if (dir_entry->d_type == DT_DIR) {
            bool is_cur = are_arrays_strict_equal(".\0", dir_entry->d_name, 0, 0, 2);
            bool is_root = are_arrays_strict_equal("..\0", dir_entry->d_name, 0, 0, 3);

            // only search directory if it is a sub directory
            if (is_root == false && is_cur == false) {
                path_add_segment(curr_path, dir_entry->d_name);
                bool has_found_file =
                    recursive_search_for_file(filename, curr_path, dir_entry->d_name);

                if (has_found_file == true) {
                    return true;
                }
            }

        } else if (are_arrays_strict_equal(filename, dir_entry->d_name, 0, 0, t_name_len)) {
            // case of finding the correct file
            path_add_segment(curr_path, dir_entry->d_name);
            return true;
        }
    }

    // Case where there is no more subdirectory and file was not found
    // In that case we remove the parent directory name
    path_rm_last_seg(curr_path);
    return false;
}

bool find_file_path_in_dir(const char *filename, SimplePath *path, char *init_dir) {
    bool has_found_file = false;
    if (path->num_segments > 0) {
        printf("Provided path must be empty for src/ search but got %ud segments\n",
               path->num_segments);
        return has_found_file;
    }
    path_add_segment(path, init_dir);

    has_found_file = recursive_search_for_file(filename, path, init_dir);
    return has_found_file;
}

bool does_test_exist(char *test_name) {
    bool test_exist = false;

    int t_name_len = get_fname_len(test_name);
    const char *dirname = "tests";

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        printf("Could not open directory: %s", dirname);
        exit(EXIT_FAILURE);
    }

    // iterate over directory entries to check if test exists
    struct dirent *direntry;
    while ((direntry = readdir(dir)) != NULL) {
        if (are_arrays_strict_equal(test_name, direntry->d_name, 0, 0, t_name_len)) {
            test_exist = true;
        }
    }

    // close directory before returning
    closedir(dir);
    return test_exist;
}

/**
 * Strips a filename of its last file extension
 * @param file: pointer to the filename. Expected to be null terminated.
 * @param name: pointer to a char array in which the name will be copied. Will copy with null
 * termination
 * */
void strip_filename_extension(const char *file, char *name) {
    int counter = get_fname_len(file);
    assert(counter > 0);

    while (counter > 0 && file[counter - 1] != '.') {
        counter--;
    }

    name = memset(name, 0, counter);

    if (counter == 0) {
        return;
    }

    memcpy(name, file, counter - 1);
}

/**
 * Replace the filename extension of a filename with the given extension
 * @param char *file: pointer to the filename. Expected to be null terminated and to have enough
 * place for the extension
 * @param char *extension: characters that make up the extension ("txt" for example). Expected to
 * be null terminated
 * */
void replace_filename_extension(char *file, char *extension) {
    int counter = get_fname_len(file);
    assert(counter > 0);

    while (counter > 0 && file[counter - 1] != '.') {
        counter--;
    }

    if (counter == 0) {
        printf("WARNING: Could not find a file extension for file: %s. Not replacing.\n", file);
        return;
    }

    int len_ext = get_fname_len(extension);
    memcpy(file + counter, extension, len_ext);
    file[counter + len_ext] = 0;
}

/**
 * Remove the 'test_' text from test_filename and copy it to new_filename*/
void clean_test_name(const char *test_filename, char *new_filename) {
    int test_f_len = get_fname_len(test_filename);
    memset(new_filename, 0, test_f_len);
    memcpy(new_filename, test_filename + 5, test_f_len - 5);
}

int main(int argc, char *argv[]) {
    // check that we do have a name associated with the test to run
    if (argc != 2) {
        printf("Expected 1 argument (test to run) but got %i arguments\n", argc - 1);
        printf("Assumptions for running the tests are:\n");
        printf(
            "    - Your tests are in a tests/ folder with name format: test_<file_to_test>.c\n");
        printf("    - Your source code is in a src/ folder (can be in sub folder) with name: "
               "<file_to_test>.c\n");
        printf("    - Your headers is in a src/ folder (can be in sub folder) with name: "
               "<file_to_test>.h\n");
        printf("    - You run the program with: './path_to_exec test_<file_to_test>.c'\n");
        return EXIT_FAILURE;
    }

    char *test_file_name = argv[1];
    if (does_test_exist(test_file_name) == false) {
        printf("Could not find test <%s> in the `tests` folder\n", test_file_name);
        return EXIT_FAILURE;
    }

    // find source file that we want to test
    char src_filename[255];
    char src_c_file[1024];
    char src_object_file[1024];
    char test_c_file[1024];
    char test_exec_file[1024];
    int cmd_res;

    // find test path
    SimplePath *test_file_path = init_path(false);
    bool has_found_test = find_file_path_in_dir(test_file_name, test_file_path, "tests");
    if (has_found_test == false) {
        printf("Could not find <%s> in directory 'tests'. Exiting.\n", test_file_name);
    }
    copy_path_to_buffer(test_file_path, test_c_file);
    clean_test_name(test_file_name, src_filename);

    // find source file
    SimplePath *src_file_path = init_path(false);
    bool has_found_src = find_file_path_in_dir(src_filename, src_file_path, "src");
    if (has_found_src == false) {
        printf("Could not find <%s> in directory 'src'. Exiting.\n", src_filename);
    }
    copy_path_to_buffer(src_file_path, src_c_file);

    // object file path wanted
    SimplePath *src_obj_path = init_path(false);
    path_add_segment(src_obj_path, "build");
    append_path_to_base(src_obj_path, src_file_path);
    path_rm_last_seg(src_obj_path);
    copy_path_to_buffer(src_obj_path, src_object_file);

    // build directory->need to mkdir -p it
    Cmd *cmd_mkdir_build = init_cmd();
    cmd_append_many(cmd_mkdir_build, "mkdir", "-p", src_object_file);
    cmd_res = run_cmd(cmd_mkdir_build);
    if (cmd_res == 0) {
        printf("Following command did not suceed: ");
        print_cmd(cmd_mkdir_build);
        exit(EXIT_FAILURE);
    }

    // finish the set-up for building object file
    replace_filename_extension(src_filename, "o");
    path_add_segment(src_obj_path, src_filename);
    copy_path_to_buffer(src_obj_path, src_object_file);

    // compile the src for the test as an object file
    Cmd *cmd_compile_src = init_cmd();
    cmd_append_many(cmd_compile_src, "clang", "-Wall", "-g", "-c", src_c_file, "-o",
                    src_object_file);
    cmd_res = run_cmd(cmd_compile_src);
    if (cmd_res == 0) {
        printf("Following command did not suceed: ");
        print_cmd(cmd_compile_src);
        exit(EXIT_FAILURE);
    }

    // Find correct path for the executable and mkdir directory
    Cmd *mkdir_test_exec = init_cmd();
    SimplePath *test_exec_path = init_path(true);
    path_add_segment(test_exec_path, "build");
    path_add_segment(test_exec_path, "tests");
    copy_path_to_buffer(test_exec_path, test_exec_file);

    cmd_append_many(mkdir_test_exec, "mkdir", "-p", test_exec_file);
    cmd_res = run_cmd(mkdir_test_exec);
    if (cmd_res == 0) {
        printf("Following command did not suceed: ");
        print_cmd(mkdir_test_exec);
        exit(EXIT_FAILURE);
    }
    char test_exec_name[255];
    strip_filename_extension(test_file_name, test_exec_name);
    path_add_segment(test_exec_path, test_exec_name);
    copy_path_to_buffer(test_exec_path, test_exec_file);

    // compile the test with the correctly compiled object file
    path_rm_last_seg(src_file_path);
    copy_path_to_buffer(src_file_path, src_c_file);
    Cmd *cmd_compile_test = init_cmd();
    cmd_append_many(cmd_compile_test, "clang", "-Wall", "-g", "-I", src_c_file, test_c_file,
                    src_object_file, "-o", test_exec_file);
    cmd_res = run_cmd(cmd_compile_test);
    if (cmd_res == 0) {
        printf("Following command did not suceed: ");
        print_cmd(cmd_compile_test);
        exit(EXIT_FAILURE);
    }

    // run the actual test
    Cmd *run_test = init_cmd();
    cmd_append(run_test, test_exec_file);
    cmd_res = run_cmd(run_test);
    if (cmd_res == 0) {
        printf("Following command did not suceed: ");
        print_cmd(run_test);
        exit(EXIT_FAILURE);
    }

    free_path(src_file_path);
    free_path(src_obj_path);
    free_path(test_exec_path);
    free_path(test_file_path);

    free_cmd(mkdir_test_exec);
    free_cmd(cmd_mkdir_build);
    free_cmd(cmd_compile_src);
    free_cmd(cmd_compile_test);
    free_cmd(run_test);
    return EXIT_SUCCESS;
}
