#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<wait.h>
#include<linux/limits.h>



// Function to handle built-in commands like cd and exit 
int handling_BuiltInCommands(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            return 0;
        } else if (strcmp(args[1],"~") == 0) {
            // Change to home directory
            chdir(getenv("HOME"));
            return 1; // Indicate that a builtin command was handled
        }
        else if(args[1] != NULL){
            // Change to specified directory
            if (chdir(args[1]) != 0) {
                printf("NO such file or directory: %s\n", args[1]);
            }
            return 1; // Indicate that a builtin command was handled
        }
        return 1; // Indicate that a builtin command was handled
    } 
    return 0; // Not a built-in command
}

void bg_function(char *input){
char *args[5];
int count = 0;
char *split = strtok(input, "&");
while(split != NULL){
    // Trim whitespace from command
    while(*split == ' '){
        split++;
    }
    char *end = split + strlen(split) - 1;
    while(end > split && *end == ' '){
        end--; } 
    
    *(end + 1) = '\0'; // removes the remaining spaces
    args[count++] = split;
    split = strtok(NULL, "&");  
}
    args[count] = NULL; 
    
    if (count < 1 || count > 5) {
        printf("write appropriate number of commands.\n");
        return;
    }

    for(int i = 0; i < count; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            return;
        } else if (pid == 0) {
            // Child process
            setpgid(0,0); // Set process group ID for background process
            sleep(2);
            char *arguments[] = {args[i], NULL}; // Preparing arguments for execvp
            execvp(arguments[0], arguments);
            exit(1); // Exit if execvp fails
        } else {
            // Parent process
            printf("Background process started for %s\n", args[i]);
            } 
}
        return;

}
void pipe_function(char *input){
    char *commands[5]; // up to 5 commands can be handled
    int count = 0; // Count of commands
    char *split = strtok(input, "|");
    while(split != NULL){
        // Trim whitespace from command
        while(*split == ' ') {
            split++;}
        char *end = split + strlen(split) - 1;
        while(end > split && *end == ' '){
            end--;}
        *(end + 1) = '\0'; // Null-terminate the trimmed string
        commands[count++] = split;
        split = strtok(NULL, "|");    
}    

   if (count < 2 || count > 5) {
        printf("Support is for 1 to 4 pipes (2 to 5 commands).\n");
        return;
    }
 
    //create pipes based on the number of commands
    int p1[2], p2[2], p3[2], p4[2];
    if (count > 1){
        pipe(p1);}
    if (count > 2){ 
        pipe(p2);}
    if (count > 3){ 
        pipe(p3);}
    if (count > 4){
         pipe(p4);}

    for (int i = 0; i < count; i++) {
        int pid = fork();
        if (pid == 0) {
            // Parse arguments
            char *args[6];
            int j = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL && j < 5) {
                args[j++] = arg;
                arg = strtok(NULL, " ");
            }
            args[j] = NULL;

            
            if (i == 0) {
                //writing to pipe p1
                dup2(p1[1], 1);
            } else if (i == 1) {
                //reading from pipe p1, writing to p2
                dup2(p1[0], 0);
                if (count > 2){
                    dup2(p2[1], 1);
                }
            } else if (i == 2) {
                // reading from pipe p2, writing to p3
                dup2(p2[0], 0);
                if (count > 3){ 
                dup2(p3[1], 1);
                }
            } else if (i == 3) {
                // reading from pipe p3, writing to p4
                dup2(p3[0], 0);
                if (count > 4) dup2(p4[1], 1);
            } else if (i == 4) {
                // fifth command: stdin from p4
                dup2(p4[0], 0);
            }

            // Close all pipes in child
            if (count > 1){
              close(p1[0]);
              close(p1[1]);
            }
            if (count > 2){ 
                close(p2[0]);
                close(p2[1]);
             }
            if (count > 3) { 
                close(p3[0]);
                close(p3[1]);
            }
            if (count > 4) {
                close(p4[0]);
                close(p4[1]);
            }

            execvp(args[0], args);
            printf("Error executing command: %s\n", args[0]);
            exit(1);
        }
    }    

    // Parent closes all pipes
    if (count > 1){
         close(p1[0]);
        close(p1[1]);
     }
    if (count > 2){ 
        close(p2[0]); 
        close(p2[1]); 
    }
    if (count > 3){ 
        close(p3[0]); 
        close(p3[1]); 
    }
    if (count > 4){ 
        close(p4[0]); 
        close(p4[1]); 
    }

    // Parent waits for all children
    for (int i = 0; i < count; i++) {
        wait(NULL);
    }


}

void reverse_pipe_function(char *input){
    char *commands[5];
    int count = 0;
    char *split = strtok(input, "~");
    while(split != NULL){
        while(*split == ' ') split++;
        char *end = split + strlen(split) - 1;
        while(end > split && *end == ' ') end--;
        *(end + 1) = '\0';
        commands[count++] = split;
        split = strtok(NULL, "~");
    }

    if (count < 2 || count > 5) {
        printf("Support is for 1 to 4 reverse pipes (2 to 5 commands).\n");
        return;
    }

    //creating pipes based on the number of commands
    int p1[2], p2[2], p3[2], p4[2];
    if (count > 1){
        pipe(p1);}
    if (count > 2){ 
        pipe(p2);}
    if (count > 3){ 
        pipe(p3);}
    if (count > 4){
         pipe(p4);}


    for (int i = count - 1; i >= 0; i--) {
        int pid = fork();
        if (pid == 0) {
            char *args[6];
            int j = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL && j < 5) {
                args[j++] = arg;
                arg = strtok(NULL, " ");
            }
            args[j] = NULL;

            // Reverse pipe logic
            if (count > 1) {
                if (i == count - 1) {
                    // Last command: write to p1
                    dup2(p1[1], 1);
                } else if (i == count - 2) {
                    // Read from p1, write to p2
                    dup2(p1[0], 0);
                    if (count > 2) dup2(p2[1], 1);
                } else if (i == count - 3) {
                    // Read from p2, write to p3
                    dup2(p2[0], 0);
                    if (count > 3) dup2(p3[1], 1);
                } else if (i == count - 4) {
                    // Read from p3, write to p4
                    dup2(p3[0], 0);
                    if (count > 4) dup2(p4[1], 1);
                } else if (i == count - 5) {
                    // Fifth command: read from p4
                    dup2(p4[0], 0);
                }
            }

            // Close all pipes in child
            if (count > 1){
              close(p1[0]);
              close(p1[1]);
            }
            if (count > 2){ 
                close(p2[0]);
                close(p2[1]);
             }
            if (count > 3) { 
                close(p3[0]);
                close(p3[1]);
            }
            if (count > 4) {
                close(p4[0]);
                close(p4[1]);
            }
            execvp(args[0], args);
            printf("Error executing command: %s\n", args[0]);
            exit(1);
        }
    }

     // Parent closes all pipes
    if (count > 1){
         close(p1[0]);
        close(p1[1]);
     }
    if (count > 2){ 
        close(p2[0]); 
        close(p2[1]); 
    }
    if (count > 3){ 
        close(p3[0]); 
        close(p3[1]); 
    }
    if (count > 4){ 
        close(p4[0]); 
        close(p4[1]); 
    }
    // Parent waits for all children
    for (int i = 0; i < count; i++) {
        wait(NULL);
    }

}

// Function to append contents of two files into each other
void append_function(char* input){
   
    //getting the file names from input
    char *files[2];
    int count = 0;
    char *split = strtok(input, "+");
    while(split !=NULL){
        // Trim whitespace from filename
        while(*split == ' ') {
        split++;
    }
        char *end = split + strlen(split) - 1;
        while(end > split && *end == ' '){
         end--;
        }
       *(end + 1) = '\0';// Null-terminate the trimmed string
    files[count++] = split;
    split = strtok(NULL, "+");  
    }

    if(files[0]!=NULL && files[1]!=NULL && count == 2){
    char buffer1[PATH_MAX];
    int fd1 =  open(files[0], O_RDWR | O_APPEND);
    if(fd1 < 0){
        printf("Error opening file: %s\n", files[0]);
        return;
    }

    int n = read(fd1, buffer1, sizeof(buffer1) - 1);
    int fd2 = open(files[1], O_RDWR | O_APPEND);
    if(fd2 < 0){
        printf("Error opening file: %s\n", files[1]);
        return;
    }
    char buffer2[PATH_MAX];
    int m = read(fd2, buffer2, sizeof(buffer2) - 1);
    
    write(fd1,buffer2,m);// Append the content of the second file to the first file
    write(fd2,buffer1,n);// Append the content of the first file to the second file
    close(fd1);
    close(fd2);

     return;
}
    printf("Not enough files provided for appending.\n");
    return;

}
//function to output number of words in upto 4 files
void word_count(char *input){
    char *files[4];
    int count = 0;

    char *start = input;
    if(*start == '#') {
        start++; // Skip the '#' character
        while(*start == ' ') {
            start++; // Skip leading spaces
        }
    
    }
    //splitting the input to get files
    char *split = strtok(start, " ");
    while(split!=NULL){
        
        files[count++] = split;
        
        split = strtok(NULL, " ");
    }
    
    if(count == 0) {
        printf("No files provided for word count.\n");
        return;
    }
    if(count > 4){ // Limiting to 4 files
        printf("Word count of upto 4 files are allowed.\n");
        return;
    }

    for(int i = 0; i < count; i++) {
        int fd = open(files[i], O_RDONLY);
        if(fd < 0) {
            printf("Error opening file: %s\n", files[i]);
            continue;
        }
        
        char buffer[PATH_MAX];
        int n = read(fd, buffer, sizeof(buffer) - 1);
        buffer[n] = '\0'; // Null-terminate the buffer to 
        
        int word_count = 0;
        char *token = strtok(buffer, " \n\t");
        while(token != NULL) {
            word_count++; //its here that means we have a word
            token = strtok(NULL, " \t\n");// Split by whitespace characters and newlines and tabs
        }
        printf("%d\n",word_count);        
        close(fd);
    }
}



//function to concatenate files and print it to stdout
void concat_function(char *input){

 int count = 0;
 char* total_files[6];//
 //splitting the input to get files
 char *split = strtok(input,"++");
 while(split!=NULL){

    // Trim whitespace from filename
    while(*split == ' ') {
        split++;
    }
    char *end = split + strlen(split) - 1;
    while(end > split && *end == ' '){
         end--;
    }
    *(end + 1) = '\0';// this will cut all the remaining spaces

    total_files[count++] = split;
    split = strtok(NULL, "++");
 } 

 if (count < 2 || count > 5) {
        printf("write appropriate number of commands.\n");
        return;
    }
 
 total_files[count] = NULL; // Null-terminate the array
 char buffer[PATH_MAX];
 for(int i=0;i<count;i++){
    int fd = open(total_files[i],O_RDONLY);
    if(fd<0){
        printf("Error opening file: %s\n", total_files[i]);
    }
    else{
    int n = read(fd, buffer, sizeof(buffer) - 1);
    buffer[n] = '\0'; // Null-terminate the buffer
    write(1, buffer, n); // Write to stdout
    close(fd);
    }}
 return;
}

void redirect_function(char *input){
    char *args[2];
    int count =0;
    if(strstr(input," > ")){
        char *split = strtok(input, " > ");
        while(split !=NULL){
        args[count++] = split;
        split = strtok(NULL, " > ");
        }
        char *command1 = args[0];
        // char *file = args[1];

        char *trimmed_commands = strtok(command1, " ");
        char *command_args[6];
        int i = 0;
        while (trimmed_commands != NULL) {
            command_args[i++] = trimmed_commands;
            trimmed_commands = strtok(NULL, " ");
        }
        command_args[i] = NULL; //
        
       
        int fd = open(args[1],O_WRONLY);
        dup2(fd,1); // Redirect stdout to the file
        execvp(command_args[0], command_args);
        close(fd); // Close the file descriptor
        return;
    }
    //redirecting output with appendind it
    else if(strstr(input," >> ")){
        char *split = strtok(input, " >> ");
        while(split !=NULL){

    
        args[count++] = split;
        split = strtok(NULL, " >> ");
        }
        char *command1 = args[0];
        // char *file = args[1];

        char *trimmed_commands = strtok(command1, " ");
        char *command_args[6];
        int i = 0;
        while (trimmed_commands != NULL) {
            command_args[i++] = trimmed_commands;
            trimmed_commands = strtok(NULL, " ");
        }
        command_args[i] = NULL; 
        
       
        int fd = open(args[1],O_WRONLY | O_APPEND);
        dup2(fd,1); // Redirect stdout to the file
        execvp(command_args[0], command_args);
        close(fd); // Close the file descriptor
        return;
    }
    else if(strstr(input,"<")){
        char *split = strtok(input, "<");
        while(split !=NULL){
        args[count++] = split;
        split = strtok(NULL, "<");
        }
        char *command1 = args[0];
        // char *file = args[1];

        char *trimmed_commands = strtok(command1, " ");
        char *command_args[6];
        int i = 0;
        while (trimmed_commands != NULL) {
            command_args[i++] = trimmed_commands;
            trimmed_commands = strtok(NULL, " ");
        }
        command_args[i] = NULL; 
        
        int pid = fork();
        if (pid < 0) {
            printf("Fork failed\n");
            return;
        } else if (pid == 0) {
        int fd = open(args[1], O_RDONLY); //opening our file
        if(fd < 0) {
            printf("Error opening file");
        }
        dup2(fd, 0); // Redirect stdin to the file
        execvp(command_args[0], command_args);
        close(fd); // Close the file descriptor
        return;}
        else {
            wait(NULL); // Wait for child process to finish
        }    
    }
    else{
        printf("Invalid redirection syntax.\n");
        return; 
    }
}
// Function to handle sequential commands separated by semicolons
void sequential_commands(char *input){
    char *commands[4]; // utpo 4 commands can be handled
    int count = 0;
    char *split = strtok(input, ";");   
    while(split!=NULL){
        while(*split == ' ') {
        split++; // Trim leading spaces
        }
        char *end = split + strlen(split) - 1;
        while(end > split && *end == ' ') {
         end--; // Trim trailing spaces
        }
        *(end + 1) = '\0'; // Null-terminate the trimmed string     

        commands[count++] = split;
        split = strtok(NULL, ";");
    }
    
    if (count < 2 || count > 5) {
        printf("write appropriate number of commands.\n");
        return;
    }
    commands[count] = NULL; // Null-terminate the array
    
 int i = 0;
 while(i<count){ // loop through each command
    char *single_command = commands[i];
    char *args[6];
    int command_count = 0;
    char *p = strtok(single_command, " ");
    while (p != NULL) {
        args[command_count++] = p; // Store each argument in the args array
        p = strtok(NULL, " ");  
 }
    args[command_count] = NULL; 

    int pid = fork(); 
    if (pid < 0) {
        printf("Fork failed\n");
        return;
    } else if (pid == 0) { 
        // Child process
        execvp(args[0], args);
        exit(1); // Exit if execvp fails
    } else {
        // Parent process
        wait(NULL); // Wait for child process to finish
    }
    i++;

}   
    return;
}

void conditional_function(char *input) {
    char *executables[5];
    char *operators[4];
    int exec_count = 0, op_count = 0;

    // Parse input into executables and operators
    char *ptr = input;
    while (*ptr) {
        
        char *next_and = strstr(ptr, "&&");
        char *next_or = strstr(ptr, "||");
        char *next_op = NULL;
        int is_and = 0;
        if (next_and && (!next_or || next_and < next_or)) {
            next_op = next_and;
            is_and = 1;
        } else if (next_or) {
            next_op = next_or;
            is_and = 0;
        }

        if (next_op) {
            int len = next_op - ptr;
            char *cmd = malloc(len + 1);
            strncpy(cmd, ptr, len);
            cmd[len] = '\0';
            // Remove leading spaces
            char *start = cmd;
            while (*start == ' '){
                 start++;}
            // Remove trailing spaces
            char *end = start + strlen(start) - 1;
            while (end > start && *end == ' '){
                 end--;}
            *(end + 1) = '\0';
            // If we skipped leading spaces, copy to a new buffer
            if (start != cmd) {
                char *trimmed = strdup(start);
                free(cmd);
                executables[exec_count++] = trimmed;
            } else {
                executables[exec_count++] = cmd;
}
            operators[op_count++] = is_and ? "&&" : "||";
            ptr = next_op + 2;
        } else {
            char *cmd = strdup(ptr);// Get the remaining command
            while (*cmd == ' ') {
                cmd++;}
            char *end = cmd + strlen(cmd) - 1;
            while (end > cmd && *end == ' ') {
                end--;}
            *(end + 1) = '\0';
            executables[exec_count++] = cmd;
            break;
        }
    }

    int last_status = 0;
    for (int i = 0; i < exec_count; i++) {
        // Split command into args
        char *args[6];
        int arg_count = 0;
        char *p = strtok(executables[i], " ");
        while (p != NULL && arg_count < 5) {
            args[arg_count++] = p;
            p = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        // Decide if we should run this command
        if (i == 0 ||
            (i > 0 && strcmp(operators[i-1], "&&") == 0 && last_status == 0) ||
            (i > 0 && strcmp(operators[i-1], "||") == 0 && last_status != 0)) {
            int pid = fork();
            if (pid < 0) {
                printf("Fork failed\n");
                return;
            } else if (pid == 0) {
                execvp(args[0], args);
                exit(1);
            } else {
                int status;
                waitpid(pid, &status, 0);
                last_status = WEXITSTATUS(status);
            }
        }
       
    }
    for (int i = 0; i < exec_count; i++){
        free(executables[i]);
    }
    // Free allocated memory for operators
    for (int i = 0; i < op_count; i++) {
        free(operators[i]);
    }
    return;
}


int main() {
    // Allocate memory dynamically
    char *input = malloc(PATH_MAX);
    if (input == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    while (1) {
        printf("mbash25$ ");
        fflush(stdout);
        
        if (fgets(input, PATH_MAX, stdin) == NULL) {
            break;
        }
        // Remove newline
        input[strcspn(input, "\n")] = '\0';

        //MAKING A COPY FOR PARSING
       char* input_copy = malloc(strlen(input) + 1);
       strcpy(input_copy, input);
       
       if(strstr(input_copy,"&&") || strstr(input_copy,"||")){
        conditional_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling conditional command
       }

       if(strstr(input_copy,"&")){
        bg_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling background command
       }
       if(strstr(input_copy,"|")){
        pipe_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling pipe command
       }
       if(strstr(input_copy,"~")){
        reverse_pipe_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling reverse pipe command
       }
       if(strstr(input_copy,"++")){
        concat_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling concat command
       }
       if(strstr(input_copy,"+")){
        append_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling append command
       }
       if(strstr(input_copy,"#")){
        word_count(input_copy);
        free(input_copy); // Free the copy after use
        continue; // Continue to the next iteration after handling word count command
       }
       if(strstr(input_copy,">")|| strstr(input_copy,"<")||strstr(input_copy,">>")){    
        redirect_function(input_copy);
        free(input_copy); // Free the copy after use
        continue; 
       }
       if(strstr(input_copy,";")){
        sequential_commands(input_copy);
        free(input_copy); // Free the copy after use
        continue;
       }
        //Storing input into a array of strings
        char *p = strtok(input, " ");
        char *args[6];
        int i = 0;
        while ( p!= NULL) {
        args[i++] = p; 
        p = strtok(NULL, " ");
       }
    
       if (i <1 || i > 5) {
           printf("write appropriate number of commands.\n");
           continue; // Skip to the next iteration if no valid command is entered
       }
       args[i] = NULL; // Null-terminate the array
       if(args[0] == NULL) {
           continue; // No command is eneterd,
       }
      
       if(handling_BuiltInCommands(args)){
        continue; //if true then no need to fork
       }
      
       int pid = fork();
         if (pid < 0) {
            printf("Fork failed\n");
            free(input);
            return 1;
        } else if (pid == 0) {
            // Child process
            execvp(args[0],args);
            exit(1); // Exit if execvp fails
        } else {
            // Parent process
            wait(NULL); // Wait for child process to finish
        }
        
        
        if (strcmp(input, "killterm") == 0) {
            printf("Terminating the mbash25 shell...\n");
            break;
        }
    }   
    // Free allocated memory
    free(input);
    return 0;
}
