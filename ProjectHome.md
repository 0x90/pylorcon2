**PyLorcon2** is a Python wrapper for the Lorcon2 library.

Lorcon2 (Loss Of Radio CONnectivity) is a generic library for injecting 802.11 frames, capable of injection via multiple driver frameworks, without forcing modification of the application code for each platform/driver.

Here is a little example.

```
>>> import PyLorcon2
>>>
>>> context = PyLorcon2.Context("wlan0")
>>> context.open_injmon()
>>>
>>> context.send_bytes("A" * 30)
```



See instructions on building and installing the wrapper [here](BuildingPyLorcon2.md)

http://corelabs.coresecurity.com