#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

void log_event(const char *event) {
    syslog(LOG_INFO, "D-Bus Event: %s", event);
    printf("Event: %s\n", event);
}

int main() {
    DBusError err;
    DBusConnection *conn;
    
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection error: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    
    // Подписка на сигналы NameOwnerChanged
    dbus_bus_add_match(conn, 
        "type='signal',interface='org.freedesktop.DBus',member='NameOwnerChanged'",
        &err);
    
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Match error: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    
    openlog("dbus_monitor", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "D-Bus monitor started");
    
    // Основной цикл
    while (1) {
        dbus_connection_read_write(conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(conn);
        
        if (msg != NULL) {
            if (dbus_message_is_signal(msg, "org.freedesktop.DBus", "NameOwnerChanged")) {
                char *name, *old_owner, *new_owner;
                dbus_message_get_args(msg, &err,
                    DBUS_TYPE_STRING, &name,
                    DBUS_TYPE_STRING, &old_owner,
                    DBUS_TYPE_STRING, &new_owner,
                    DBUS_TYPE_INVALID);
                
                char event[256];
                snprintf(event, sizeof(event), 
                    "NameOwnerChanged: %s (Old: %s, New: %s)", 
                    name, old_owner, new_owner);
                log_event(event);
            }
            dbus_message_unref(msg);
        }
        sleep(1);
    }
    
    closelog();
    dbus_connection_unref(conn);
    return 0;
}
