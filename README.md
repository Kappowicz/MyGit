# MyGit

My own simple git implementation

Available commands:

- 'mygit init'
- 'mygit status'
- 'mygit add {fileName}'
- 'mygit commit -m {commitName}'
- 'mygit _erase'
- 'mygit log'
- 'mygit hash-object {fileName}'
- 'mygit diff'

mygit folder structure with sample txt file commited:

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

Initially planned features:

- make 'mygit init' create folder .mygit
- make 'mygit status' display the status of current folders repo
- make 'mygit --help' display info about whole command
- make 'mygit add {}' add some files to the "index",
  basically it will copy the file to .mygit/objects,
  and rename the file to hash of this file,
  (theoretically git also compresses the file
  and puts the file into subfolder with name of hashes first two letters
  [for example if file hello.txt hash a hash qwert1 the file is put into
  .git/objects/qw/ert1 and it makes the /objects folder cleaner, but for
  now I'll put everything into one folder, time for improvements will come later]),
  I will also have to make ./mygit/index update with all commands
- make my own very simple implementation of some kind of hashing (accidentally remade djb2), now I plan to implement
  SHA-256 later
- make 'mygit diff's to check differences between some commit and current directory
- make some fancy tests in github ci/cd 

