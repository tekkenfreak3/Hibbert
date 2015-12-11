Hibbert is a very simple power monitor, which (at the moment) supports Linux.
It has no dependencies aside from a Linux system with the
/sys/class/power_supply interface. Hibbert periodically checks the battery
status, and if the battery is below a certain percentage (default: 5) and not
charging, then it tries to execute ~/.hibbertt as a shell script, or
/etc/hibbertt should ~/.hibbertt not exist. Shibbert is also built, which is
just a simpler hibbert which doesn't act as a daemon and is just meant to be
run from cron if that's more your thing.
