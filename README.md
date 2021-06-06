# Terminal based File Explorer
- The implemented file explorer works in two modes:
    - `Normal mode (default mode)`: used to explore the current directory and navigate the file system 
    - `Command mode`: used to perform operations using shell commands
![fileExplorer](https://drive.google.com/uc?export=view&id=1Nzc3VHFa25qmOBuvbvY85wVmd5Hnw3qQ)
- The root of the application is the directory where the application is started.

### Normal Mode
- In normal mode, a list of files and directories in the current folder is displayed
- Scroll using `k` and `l` in case the list overflows
- Navigate up and down with the corresponding `arrow keys`
- To open a file or enter a directory press `Enter` key
- Traversal:
    - `Left arrow`: Go back to previously visited directory
    - `Right arrow`: Go forward to the next directory
    - `Backspace`: Up one level (parent directory)
    - `h`: Go to home folder (where the application was started)

### Command Mode
- To go to command mode press `:`
- Following commands can be used (the directory path can be absolute or relative):
    - `copy`: copy <source_file(s)> <destination_directory>
    - `move`: move <source_file(s)> <destination_directory>
    - `rename`: rename <old_filename> <new_filename>
    - `create_file`: create_file <file_name> <destination_path>
    - `create_dir`: create_dir <dir_name> <destination_path>
    - `delete_file`: delete_file <file_path>
    - `delete_dir`: delete_dir <dir_path>
    - `goto`: goto <directory_path> (absolute path)
    - `search`: search <file_name> or search <directory_name>
    - `ESC`: Return to Normal Mode
- Sample Commands:
    - copy foo.txt bar.txt baz.mp4 ~/foobar
    - move foo.txt bar.txt baz.mp4 ~/foobar
    - rename foo.txt bar.txt
    - create_file foo.txt ~/foobar
    - create_file foo.txt .
    - create_dir foo ~/foobar

### Build Command
```
g++ main.cpp normal.cpp listdir.cpp terminal.cpp header.h -o fileExplorer
```

### Run Command
```
./fileExplorer
```
