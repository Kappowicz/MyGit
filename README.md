# MyGit
My own simple git implementation
Plan for now:
- make 'mygit init' create folder .mygit //DONE 
- make 'mygit status' display the status of current folders repo
- make 'mygit --help' display info about whole command
- make 'mygit add {}' add some files to the "index", 
basically it will copy the file to .mygit/objects, 
and rename the file to hash of this file,
(theoretically git also compresses the file 
and puts the file into subfolder with name of hashes first two letters
[for example if file hello.txt hash a hash qwert1 the file is put into 
.git/objects/qw/ert1 and it makes the /objects folder cleaner, but for 
now I'll put everything into one folder, time for improvements will come later])
- make my own very simple implementation of some kind of hashing //Partially done
(accidentally remade djb2), might improve
- make some fancy tests in github ci/cd 

