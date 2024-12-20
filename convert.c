#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

int is_comment_or_empty(const char *line) {
    while (isspace(*line)) line++;
    return *line == '\0' || *line == '@' || *line == ':';
}

void trim_newline(char *line) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
}

void escape_quotes(char *line) {
    char *src = line;
    char *dst = line;
    while (*src) {
        if (*src == '"') {
            *dst++ = '\\';
        }
        *dst++ = *src++;
    }
    *dst = '\0';
}

void convert_command_to_c(const char *command, FILE *output_file) {
    const char *start = command;
    while (isspace(*start)) start++;
    
    if (is_comment_or_empty(start)) return;

    char command_copy[MAX_LINE_LENGTH];
    strncpy(command_copy, start, MAX_LINE_LENGTH - 1);
    command_copy[MAX_LINE_LENGTH - 1] = '\0';
    trim_newline(command_copy);
    escape_quotes(command_copy);
    
    if (strncmp(command_copy, "echo", 4) == 0) {
        fprintf(output_file, "    printf(\"%s\\n\");\n", command_copy + 4);
    } else {
        fprintf(output_file, "    system(\"%s\");\n", command_copy);
    }
}

void convert_batch_to_c(const char *batch_file_path, const char *c_file_path) {
    FILE *batch_file = fopen(batch_file_path, "r");
    FILE *c_file = fopen(c_file_path, "w");
    
    if (!batch_file || !c_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(c_file, "#include <stdio.h>\n#include <stdlib.h>\n\n");
    fprintf(c_file, "int main() {\n");
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, batch_file)) {
        convert_command_to_c(line, c_file);
    }
    
    fprintf(c_file, "    return 0;\n}\n");
    
    fclose(batch_file);
    fclose(c_file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    const char *batch_file_path = argv[1];
    char c_file_path[1024];
    char exe_file_path[1024];
    
    snprintf(c_file_path, sizeof(c_file_path), "%s-converted.c", batch_file_path);
    snprintf(exe_file_path, sizeof(exe_file_path), "%s-converted", batch_file_path);
    
    convert_batch_to_c(batch_file_path, c_file_path);
    printf("Conversion completed. Result saved in %s\n", c_file_path);
    
    printf("Compiling with GCC\n");
    
    char compile_command[2048];
    snprintf(compile_command, sizeof(compile_command), "gcc/bin/gcc %s -o %s", c_file_path, exe_file_path);
    int compile_result = system(compile_command);
    
    if (compile_result == 0) {
        printf("Compilation successful. Executable saved in %s\n", exe_file_path);
    } else {
        printf("Compilation failed\n");
    }
    
    return 0;
}
