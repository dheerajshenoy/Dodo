# Dodo
A minimalistic PDF Viewer written in C++ using `Qt` and `Poppler` that uses `VIM` keybindings (mostly)

# Why ?

> Wanted to put my C++ skills to the test, and also to create a decent PDF viewer for my daily use.

# Dependencies
	
1. Qt6
2. Poppler-Qt6
3. Linux :)

### For anyone one ArchLinux
	
```
sudo pacman -S qt6-base poppler-qt6

```

# Installation

```
git clone https://github.com/dheerajshenoy/Dodo.git
cd Dodo
qmake6 .
make

// make command, outputs a binary file to the bin directory, which can be opened.

 sudo make install
// command can be used to install the binary to /bin directory

// TODO: creation of .desktop file
```

# Features

1. PDF Viewing (*obviously*)
2. Unobtrusive UI
3. Check out the [TODO](Todo.md) Sections for the other features

# Preview

[](Screenshots/pic1.png)
[](Screenshots/pic2.png)
[](Screenshots/pic3.png)
