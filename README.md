Hibbert is a very simple power monitor, which (at the moment) supports Linux.
It has no dependencies aside from a Linux system with the
/sys/class/power_supply interface. Hibbert periodically checks the battery
status, and if the battery is below a certain percentage (default: 5) and not
charging, then it tries to execute ~/.hibbertt as a shell script, or
/etc/hibbertt should ~/.hibbertt not exist. Shibbert is also built, which is
just a simpler hibbert which doesn't act as a daemon and is just meant to be
run from cron if that's more your thing.

Tips:
You can have more than one entry for a user/group in sudoers, if you define
things like pm-hibernate as NOPASSWD for yourself then you can hibernate or
suspend as your trigger. There may also be a way to do this from logind or
something.
