#!/usr/bin/env python3
import atexit
import os
import signal
import subprocess
import time
from Xlib import display, X, Xatom

def xinit():
    global dpy, root
    dpy = display.Display()
    root = dpy.screen().root
    root.change_attributes(event_mask=X.PropertyChangeMask)

atoms = {}
def atom(name):
    if name not in atoms:
        atoms[name] = dpy.intern_atom(name)
    return atoms[name]

def xpropget(key):
    ret = root.get_full_property(atom(key), Xatom.STRING)
    return ret.value if ret else ''

last_set_value = {}
def xpropset(key, value):
    if last_set_value.get(key, None) != value:
        cmd = ['xprop', '-root', '-f', key, '8s', '-set', key, value]
        subprocess.check_call(cmd)
        last_set_value[key] = value

def xpropwait(keys):
    atoms = [atom(key) for key in keys]
    while True:
        event = dpy.next_event()
        if event.type == X.PropertyNotify and event.atom in atoms:
            return

########################################################################

# Clean up the child processes when terminated.
parent, kids = os.getpid(), []
def cleanup():
    if os.getpid() == parent:
        for kid in kids:
            os.kill(kid, signal.SIGKILL)
atexit.register(cleanup)
signal.signal(signal.SIGTERM, lambda sig, stack: exit(0))

def fork(f):
    pid = os.fork()
    if pid == 0:
        while True:
            try:
                f()
            except KeyboardInterrupt:
                pass
            except Exception as e:
                print(f)
                print(e)
                time.sleep(5)
        exit(0)
    kids.append(pid)

def check_output(*args):
    return subprocess.check_output(args).decode('utf8').rstrip('\n')

def command(cmd, interval):
    propname = '_STATUS_' + cmd.upper()
    xpropset(propname, '...')
    def func():
        xpropset(propname, check_output(cmd))
        time.sleep(interval)
    func.__name__ = cmd
    return func

def main():
    xinit()
    # Load local settings, if they exist.
    path = os.path.join(os.getenv('HOME'), 'bin/statuslocal.py')
    if os.path.exists(path):
        exec(open(path).read(), globals())
    # Start the main program loop.
    names = ['_STATUS_' + s.__name__.upper() for s in functions]
    for f in functions:
        fork(f)
    try:
        while True:
            entries = [xpropget(name) for name in names]
            entries = [value for value in entries if len(value) > 0]
            entries = [entry.decode('utf8') for entry in entries]
            xpropset('WM_NAME', combine(entries))
            xpropwait(names)
    except KeyboardInterrupt:
        pass

########################################################################

# Clock.
clock_format = '%a %Y %b %d %I:%M:%S %p %Z'
def clock():
    xpropset('_STATUS_CLOCK', time.strftime(clock_format))
    time.sleep(0.5)

# Disk space warning.
def diskspace():
    status = ''
    for line in check_output('df').split('\n')[1:]:
        line = line.split()
        path, used = line[5], int(line[4].rstrip('%'))
        if used >= 95:
            if used > 98:
                status += '\x03'
            status += '%s %d%%\x01, ' % (path, used)
    xpropset('_STATUS_DISKSPACE', status.rstrip('\x01, '))
    time.sleep(10)

# Volume.
volume = command('volume', interval=2)

# Default list of entries to appear in the status bar.
functions = [diskspace, volume, clock]

def combine(entries):
    return '\x01 : '.join(entries)

if __name__ == '__main__':
    main()
