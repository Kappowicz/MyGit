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
- make my own very simple implementation of some kind of hashing (accidentally remade djb2), might improve
- make some fancy tests in github ci/cd 

