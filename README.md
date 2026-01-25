# MyGit

My own simple git implementation  
I use cpp26 (btw)

### Available commands:

start or delete a working area:

- 'mygit init' initialize mygit folder structure or reinitialize existing one
- 'mygit _erase' delete entire mygit repository (debug)

work on the current change:

- 'mygit add <fileName>' add file to index

examine the history and state:

- 'mygit log' show commits history
- 'mygit status' display status of files in current directory
- 'mygit diff' show differences in file content
- 'mygit hash-object <fileName>' display hash of a file

grow and tweak your history:

- 'mygit branch' display all branches
- 'mygit branch <branchName>' create branch
- 'mygit commit -m <commitName>' create commit
- 'mygit checkout <commitName>' (doesn't support switching branches yet) change current commit

### mygit folder structure with sample txt file commited:

```text
hello.txt
.mygit/
├── objects/
│   ├── 17240584371599139008
│   └── 2096893839203276767
├── index
├── refs/
│   ├── HEAD
│   └── main
```

### Planned features:

- [X] make 'mygit init' create folder .mygit
- [X] make 'mygit status' display the status of current folders repo
- [X] make 'mygit --help' display info about whole command
- [X] make 'mygit add {}' add some files to the "index",
  basically it will copy the file to .mygit/objects,
  and rename the file to hash of this file,
  (theoretically git also compresses the file
  and puts the file into subfolder with name of hashes first two letters
  [for example if file hello.txt hash a hash qwert1 the file is put into
  .git/objects/qw/ert1 and it makes the /objects folder cleaner, but for
  now I'll put everything into one folder, time for improvements will come later]),
  I will also have to make ./mygit/index update with all commands
- [X] make my own very simple implementation of some kind of hashing (accidentally remade djb2), now I plan to implement
  SHA-256 later
- [X] make 'mygit diff's to check differences between some commit and current directory
- [ ] make some fancy tests in github ci/cd
- [ ] make .mygitignore
- [ ] make tree object type (for now there are only files and commits, no directories)
- [ ] make mygit merge
