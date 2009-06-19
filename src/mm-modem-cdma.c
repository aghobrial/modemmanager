/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <string.h>
#include <dbus/dbus-glib.h>
#include "mm-modem-cdma.h"
#include "mm-errors.h"
#include "mm-callback-info.h"

static void impl_modem_cdma_get_signal_quality (MMModemCdma *modem, DBusGMethodInvocation *context);
static void impl_modem_cdma_get_esn (MMModemCdma *modem, DBusGMethodInvocation *context);
static void impl_modem_cdma_get_serving_system (MMModemCdma *modem, DBusGMethodInvocation *context);

#include "mm-modem-cdma-glue.h"

enum {
    SIGNAL_QUALITY,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

/*****************************************************************************/

static void
str_call_done (MMModem *modem, const char *result, GError *error, gpointer user_data)
{
    DBusGMethodInvocation *context = (DBusGMethodInvocation *) user_data;

    if (error)
        dbus_g_method_return_error (context, error);
    else
        dbus_g_method_return (context, result);
}

static void
str_call_not_supported (MMModemCdma *self,
                        MMModemStringFn callback,
                        gpointer user_data)
{
    MMCallbackInfo *info;

    info = mm_callback_info_string_new (MM_MODEM (self), callback, user_data);
    info->error = g_error_new_literal (MM_MODEM_ERROR, MM_MODEM_ERROR_OPERATION_NOT_SUPPORTED,
                                       "Operation not supported");

    mm_callback_info_schedule (info);
}

static void
uint_op_not_supported (MMModem *self,
                       MMModemUIntFn callback,
                       gpointer user_data)
{
    MMCallbackInfo *info;

    info = mm_callback_info_uint_new (self, callback, user_data);
    info->error = g_error_new_literal (MM_MODEM_ERROR, MM_MODEM_ERROR_OPERATION_NOT_SUPPORTED,
                                       "Operation not supported");
    mm_callback_info_schedule (info);
}

static void
uint_call_done (MMModem *modem, guint32 result, GError *error, gpointer user_data)
{
    DBusGMethodInvocation *context = (DBusGMethodInvocation *) user_data;

    if (error)
        dbus_g_method_return_error (context, error);
    else
        dbus_g_method_return (context, result);
}

static void
serving_system_call_done (MMModemCdma *self,
                          guint32 class,
                          char band,
                          guint32 sid,
                          GError *error,
                          gpointer user_data)
{
    DBusGMethodInvocation *context = (DBusGMethodInvocation *) user_data;

    if (error)
        dbus_g_method_return_error (context, error);
    else {
        GValueArray *array;
        GValue value = { 0, };
        char band_str[2] = { 0, 0 };

        array = g_value_array_new (3);

        /* Band Class */
        g_value_init (&value, G_TYPE_UINT);
        g_value_set_uint (&value, class);
        g_value_array_append (array, &value);
        g_value_unset (&value);

        /* Band */
        g_value_init (&value, G_TYPE_STRING);
        band_str[0] = band;
        g_value_set_string (&value, band_str);
        g_value_array_append (array, &value);
        g_value_unset (&value);

        /* SID */
        g_value_init (&value, G_TYPE_UINT);
        g_value_set_uint (&value, sid);
        g_value_array_append (array, &value);
        g_value_unset (&value);

        dbus_g_method_return (context, array);
    }
}

static void
serving_system_invoke (MMCallbackInfo *info)
{
    MMModemCdmaServingSystemFn callback = (MMModemCdmaServingSystemFn) info->callback;

    callback (MM_MODEM_CDMA (info->modem), 0, 0, 0, info->error, info->user_data);
}

static void
serving_system_call_not_supported (MMModemCdma *self,
                                   MMModemCdmaServingSystemFn callback,
                                   gpointer user_data)
{
    MMCallbackInfo *info;

    info = mm_callback_info_new_full (MM_MODEM (self), serving_system_invoke, G_CALLBACK (callback), user_data);
    info->error = g_error_new_literal (MM_MODEM_ERROR, MM_MODEM_ERROR_OPERATION_NOT_SUPPORTED,
                                       "Operation not supported");

    mm_callback_info_schedule (info);
}

void
mm_modem_cdma_get_serving_system (MMModemCdma *self,
                                  MMModemCdmaServingSystemFn callback,
                                  gpointer user_data)
{
    g_return_if_fail (MM_IS_MODEM_CDMA (self));
    g_return_if_fail (callback != NULL);

    if (MM_MODEM_CDMA_GET_INTERFACE (self)->get_serving_system)
        MM_MODEM_CDMA_GET_INTERFACE (self)->get_serving_system (self, callback, user_data);
    else
        serving_system_call_not_supported (self, callback, user_data);
}

static void
impl_modem_cdma_get_serving_system (MMModemCdma *modem,
                                    DBusGMethodInvocation *context)
{
    mm_modem_cdma_get_serving_system (modem, serving_system_call_done, context);
}

void
mm_modem_cdma_get_esn (MMModemCdma *self,
                       MMModemStringFn callback,
                       gpointer user_data)
{
    g_return_if_fail (MM_IS_MODEM_CDMA (self));
    g_return_if_fail (callback != NULL);

    if (MM_MODEM_CDMA_GET_INTERFACE (self)->get_esn)
        MM_MODEM_CDMA_GET_INTERFACE (self)->get_esn (self, callback, user_data);
    else
        str_call_not_supported (self, callback, user_data);
}

static void
impl_modem_cdma_get_esn (MMModemCdma *modem,
                         DBusGMethodInvocation *context)
{
    mm_modem_cdma_get_esn (modem, str_call_done, context);
}

void
mm_modem_cdma_get_signal_quality (MMModemCdma *self,
                                  MMModemUIntFn callback,
                                  gpointer user_data)
{
    g_return_if_fail (MM_IS_MODEM_CDMA (self));
    g_return_if_fail (callback != NULL);

    if (MM_MODEM_CDMA_GET_INTERFACE (self)->get_signal_quality)
        MM_MODEM_CDMA_GET_INTERFACE (self)->get_signal_quality (self, callback, user_data);
    else
        uint_op_not_supported (MM_MODEM (self), callback, user_data);
}

static void
impl_modem_cdma_get_signal_quality (MMModemCdma *modem, DBusGMethodInvocation *context)
{
    mm_modem_cdma_get_signal_quality (modem, uint_call_done, context);
}

void
mm_modem_cdma_signal_quality (MMModemCdma *self,
                              guint32 quality)
{
    g_return_if_fail (MM_IS_MODEM_CDMA (self));

    g_signal_emit (self, signals[SIGNAL_QUALITY], 0, quality);
}

/*****************************************************************************/

static void
mm_modem_cdma_init (gpointer g_iface)
{
    GType iface_type = G_TYPE_FROM_INTERFACE (g_iface);
    static gboolean initialized = FALSE;

    if (initialized)
        return;

    /* Signals */
    signals[SIGNAL_QUALITY] =
        g_signal_new ("signal-quality",
                      iface_type,
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (MMModemCdma, signal_quality),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__UINT,
                      G_TYPE_NONE, 1,
                      G_TYPE_UINT);

    initialized = TRUE;
}

GType
mm_modem_cdma_get_type (void)
{
    static GType modem_type = 0;

    if (!G_UNLIKELY (modem_type)) {
        const GTypeInfo modem_info = {
            sizeof (MMModemCdma), /* class_size */
            mm_modem_cdma_init,   /* base_init */
            NULL,       /* base_finalize */
            NULL,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            0,
            0,              /* n_preallocs */
            NULL
        };

        modem_type = g_type_register_static (G_TYPE_INTERFACE,
                                             "MMModemCdma",
                                             &modem_info, 0);

        g_type_interface_add_prerequisite (modem_type, MM_TYPE_MODEM);

        dbus_g_object_type_install_info (modem_type, &dbus_glib_mm_modem_cdma_object_info);
    }

    return modem_type;
}
