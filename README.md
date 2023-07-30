# iSpy
## About
iSpy is a small IPv4 scanner that allows you to scan for open ports. Features include pretty much everything you'd expect from an IP address scanner, including multithreading support with the ability to scan around a hundred ports at once, although here are some features that are planned to be added:
* Proxy support & TOR support. Some people may want to scan IP addresses secretly, hence the name iSpy. Therefore, it is planned that iSpy will have proxy & TOR support in the future.
## Building
You'll need a UNIX-like operating system, a C compiler, and a libc that supports TCP sockets. Most likely, you have the last two of the requirements. Oh and yes, you'll also need BSD make or GNU make to be able to execute the makefile. Once all the requirements are met, go ahead and run the command `make` in the root directory of the `iSpy` directory.
## Deploying
After building just execute the `ispy` command. You'll want to supply some arguments to also get it working:
```
./ispy -T1 -A192.168.0.0:25565 -B192.168.255.255:25577
```
The `-T` parameter sets iSpy's timeout. If it's set to zero, iSpy will use the operating system's default timeout. It is recommended to set it to 1 or 2 depending on the speed of your internet. Then the `-A` and `-B` arguments define the range of IPv4 addresses and ports that should be scanned. In the command above, we're scanning for Minecraft servers in our local network. This can be changed to scan for anything else though. Let's say we want to scan for HTTP(S) servers. We can change the ports from 25565 - 25577 to 80 - 80:
```
./ispy -T1 -A1.0.0.0:80 -B127.0.0.0:80

```
Here, we just scan for HTTP(S) servers from the range `1.0.0.0` to `127.0.0.0`.
Now, what if you want to increase the amount of threads that iSpy uses? Just set the `-t` parameter! Here's an example:
```
./ispy -T1 -t100 -A1.0.0.0:80 -B126.255.255.255:80
```
There we scan just for the port 80 from the IPv4 address range `1.0.0.0` to `126.255.255.255`!