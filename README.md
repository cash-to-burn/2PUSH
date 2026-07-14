# 2PUSH

**2PUSH** is a Peer-to-Peer (P2P) utility designed for syncing and sending files/directories between clients.

> ⚠️ **DISCLAIMER:** This project is still under active development. It is **NOT** recommended for use in production or on real projects yet!

---

## How to Build

Make sure you have `cmake` and a C++ compiler installed, then run:
```bash
git clone https://github.com/cash-to-burn/2PUSH.git
cd 2PUSH/
mkdir build && cmake -S . -B build
cmake --build build/
```

- The Final Binary is in build/2PUSH

---

# USING

```bash

$ 2PUSH --help


 2PUSH help menu: 


 -ip --> IP Target to Send Files 
 -port --> PORT Target to Send Files 
 -sync --> Sync Directories Files 
 -send_all --> Send All Files in The Directories 
 -receive --> Endable Receiver Mode 


 Written by Unc Frank 
```

- Receiveing

```bash
$ 2PUSH -receive path/to/fold -port PORT
```


- Syncing

```bash
$ 2PUSH -sync path/to/fold -ip IP -port PORT
```



- Sending

```bash
$ 2PUSH -send_all path/to/fold -ip IP -port PORT
``` 

