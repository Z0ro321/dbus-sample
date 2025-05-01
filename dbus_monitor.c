#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    DBusError err;
    DBusConnection *conn;
    
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "D-Bus error: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    
    // Запрос интроспекции
    DBusMessage *msg = dbus_message_new_method_call(
        "org.freedesktop.DBus",
        "/org/freedesktop/DBus",
        "org.freedesktop.DBus.Introspectable",
        "Introspect"
    );
    
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
    if (!reply) {
        fprintf(stderr, "Error: %s\n", err.message);
        dbus_error_free(&err);
        return 1;
    }
    
    // Извлечение XML-данных
    char *xml_data;
    dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &xml_data, DBUS_TYPE_INVALID);
    printf("D-Bus Introspection Data:\n%s\n", xml_data);
    
    // Очистка
    dbus_message_unref(msg);
    dbus_message_unref(reply);
    dbus_connection_unref(conn);
    return 0;
}
