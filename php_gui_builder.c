/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: No Name                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */


#include <stdio.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlIO.h>


#include "php.h"
#include "php_ini.h"
#include "zend_interfaces.h"
#include "ext/standard/info.h"

#include "ext/gtk/php_gobject/object.h"
//#include "ext/gtk/php_gtk.h"
#include "ext/gtk/php_gtk/widget.h"
#include "ext/gtk/php_gtk/window.h"

#include "php_gui_builder.h"

#include "php_gui_parser.h"


static php_gtk_widget *gtkml_window_new(char *attrs[]);



static GArray *factories = NULL;// GArray<FactoryEntry>

static FactoryEntry gtkml_entry_window = {"window", gtkml_window_new, TRUE};


static GArray*
php_gui_get_factories() {
    if (factories!=NULL) {
        return factories;
    }
    factories = g_array_new(0, 0, sizeof(FactoryEntry));

    // DO it alpha ordered
    g_array_append_val(factories, gtkml_entry_window);

    //g_array_sort(factories, (GCompareFunc)cb);
    return factories;
}

static void
php_gui_binary_search_factory(GArray *factories, char *name) {
    return NULL;
}

gtkml_element_factory
php_gui_get_element_factory(char *element_name) {
    GArray *table = php_gui_get_factories();

    //gtkml_element_factory factory = php_gui_binary_search_factory (table, element_name);
    guint i;
    for(i=0; i<table->len; i++) {
        FactoryEntry *entry = &g_array_index(table, FactoryEntry, i);
        if( 0==g_strcmp0(element_name, entry->element_name) ) {
            //gtkml_element_factory factory = entry;
            return entry->factory;
        }
    }

    return NULL;
}


/* {{{ gtkml_window_new
 * @attrs an c array null terminated
 */
static php_gtk_widget*
gtkml_window_new(char *attrs[])
{
    zend_object *zobj = zend_objects_new(php_gtk_window_class_entry);
    //zend_object *zobj = php_gtk_widget_new(php_gtk_window_class_entry);
    //php_gtk_widget *gobj = php_gtk_widget_from_obj(zobj);
    php_gobject_object *gobj = ZOBJ_TO_PHP_GOBJECT_OBJECT(zobj);

    // read from attribute <window type="TOLEVEL">
    GtkWindowType type = GTK_WINDOW_TOPLEVEL;
    char *title = NULL;//"GTK+PHP 0.99.9";
    zend_bool resizable = TRUE;
    char *str_resizable = NULL;

    // FIXME: loop all attrs and feed a struct options
    char *str_attr;
    char *str_value;
    if (attrs!=NULL) {
        int i;
        for(i=0; attrs[i]!=NULL; i+=2) {
            //g_print("attr: %s\n", atts[i]);// <input  disabled /> use libxml2 module HTMLParser
            //g_print("%s=\"%s\"", attrs[i], attrs[i+1]);
            str_attr = g_utf8_casefold (attrs[i], -1);
            if (0==g_strcmp0(str_attr, "type")) {
                str_value = g_utf8_casefold (attrs[i+1], -1);
                if (0==g_strcmp0(str_value, "popup")) {
                    type = GTK_WINDOW_POPUP;
                }
                break;
            } else if(0==g_strcmp0(str_attr, "title")) {
                title = attrs[i+1];
            } else if(0==g_strcmp0(str_attr, "resizable")) {
                str_resizable = g_utf8_casefold (attrs[i+1], -1);
                if (0==g_strcmp0(str_resizable, "true")) {
                    resizable = TRUE;
                } else if(0==g_strcmp0(str_resizable, "false")) {
                    resizable = FALSE;
                } else {
                    // unrocogizer
                }
            } else {
                //printf("\e[1;31mUnknown Attribute: '%s'\e[0m\n", attrs[i]);
                fprintf( stderr, "[Unknown Attribute: '%s']\n", attrs[i]);
            }
        }
    }


    zval obj;
    zval arg;
    zval retval;
    ZVAL_OBJ(&obj, zobj);
    ZVAL_LONG(&arg, type);
    zend_call_method_with_1_params(&obj, Z_OBJCE_P(&obj), NULL,
                                   "__construct", &retval, &arg);
    if (title) {
        gtk_window_set_title(GTK_WINDOW(gobj->ptr), title);
    }
    if (str_resizable) {
        gtk_window_set_resizable(GTK_WINDOW(gobj->ptr), resizable);
    }

    gtk_widget_show(GTK_WINDOW(gobj->ptr));
    g_signal_connect(GTK_WINDOW(gobj->ptr), "destroy", gtk_main_quit, NULL);
    // loop attrs
    // gtk_widget_set_property(gobj, "attribute-name", value);

    return gobj;
}/*}}} */

