# Building pylorcon2 #

Install necessary software to build lorcon2 and pylorcon2.

```
$ sudo apt-get install libpcap-dev libnl-dev python-dev
...
```

Donwload lorcon, build it and install it.

```
$ git clone https://code.google.com/p/lorcon/
...
$ cd lorcon
$ ./configure --libdir=/usr/lib
...
$ make
...
$ sudo make install
...
```

Donwload !pylorcon2, build it, install it and test it.

```
$ svn co http://pylorcon2.googlecode.com/svn/trunk pylorcon2
...
$ cd pylorcon2
...
$ python setup.py build
...
$ sudo python setup.py install
...
```

Now we can test it.

```
$ python
Python 2.7.3 (default, Sep 26 2012, 21:53:58) 
[GCC 4.7.2] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>>
>>> import PyLorcon2
>>>
>>> PyLorcon2.get_version()
20091101
>>> for driver in PyLorcon2.list_drivers():
...   print driver
...
('madwifing', 'Linux madwifi-ng drivers, deprecated by ath5k and ath9k')
('tuntap', 'Linux tuntap virtual interface drivers')
('mac80211', 'Linux mac80211 kernel drivers, includes all in-kernel drivers on modern systems')
>>>
```