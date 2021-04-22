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


//#include <libxml/parser.h>
//#include <libxml/parserInternals.h>
//#include <libxml/tree.h>
//#include <libxml/uri.h>
//#include <libxml/xmlerror.h>
//#include <libxml/xinclude.h>
//#include <libxml/hash.h>
//#include <libxml/c14n.h>
//#if defined(LIBXML_HTML_ENABLED)
//#include <libxml/HTMLparser.h>
//#include <libxml/HTMLtree.h>
//#endif


#include <stdio.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlIO.h>

//zend_file_handle fh;
//fh.handle.fp;

//#define trace fprintf(stderr, "%s\n", __FUNCTION__);
//#define trace g_print("%s\n", __FUNCTION__);
//#define trace

#include "php.h"
#include "php_ini.h"
#include "zend_interfaces.h"
#include "ext/standard/info.h"

#include "ext/gtk/php_gobject/object.h"
#include "ext/gtk/php_gtk/widget.h"

#include "php_gui_parser.h"

#include "php_gui_builder.h"


static char *___message;

#define trace(msg, user_data) ___message = g_strdup_printf("%s\n", msg); fputs(___message, user_data->fd_debug);





static FILE *desc;



static GtkWidget*
php_gui_parser_get_document (void *ctx)
{
    //trace
#if 0
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;

    if (parser->document==NULL) {
        //DomDocument *doc = dom_document_new("1.0", "utf-8");
        GError *error = NULL;
        gchar *name = BAD_CAST(parser->ctxt->intSubName);//svg
        gchar *ExternalID = BAD_CAST(parser->ctxt->extSubSystem);//-//W3C//DTD SVG 1.1//EN
        gchar *SystemID = BAD_CAST(parser->ctxt->extSubURI);//http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd
        gchar *NameSpace = NULL;//"http://w3c.org/2009/svg";
        DomImplementation *implementation = dom_implementation_new();// xml_implementation_new() @see php_gui_parser_internal_subset()
        DomDocumentType *doctype = dom_implementation_create_document_type(implementation, name, ExternalID, SystemID);
        //doctype = NULL;
        DomDocument *document = dom_implementation_create_document(implementation, NameSpace, name, doctype);

        // if g_object_unref(parser->document)
        parser->document = document;
        parser->element = DOM_NODE(document);//element;
    }
    if (parser->document->priv->implementation==NULL) {

    }

    return parser->document;
#endif
    return 0;
}


/**
 * resolveEntitySAXFunc:
 * @ctx:  the user data (XML parser context)
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * Callback:
 * The entity loader, to control the loading of external entities,
 * the application can either:
 *    - override this resolveEntity() callback in the SAX block
 *    - or better use the xmlSetExternalEntityLoader() function to
 *      set up it's own entity resolution routine
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behaviour.
 */
static xmlParserInputPtr
php_gui_parser_resolve_entity (void *ctx,
                const xmlChar *publicId,
                const xmlChar *systemId)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

    return 0;
}


/**
 * isStandaloneSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Is this document tagged standalone?
 *
 * Returns 1 if true
 */
static int
php_gui_parser_is_standalone (void *ctx)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

#if 0
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;

    xmlParserCtxt *ctxt = parser->ctxt;
    g_print("Is standalone : %d\n", ctxt->input->standalone);

    return ctxt->input->standalone;
#endif
    return 0;
}

/**
 * internalSubsetSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on internal subset declaration.
 */
static void
php_gui_parser_internal_subset (void *ctx,
                const xmlChar *name,
                const xmlChar *ExternalID,
                const xmlChar *SystemID)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

}

/**
 * elementDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the element name
 * @type:  the element type
 * @content: the element value tree
 *
 * An element definition has been parsed.
 */
static void
php_gui_parser_element_decl(void *ctx,
                const xmlChar *name,
                int type,
                xmlElementContentPtr content)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

}


/**
 * entityDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the entity name
 * @type:  the entity type
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @content: the entity value (without processing).
 *
 * An entity definition has been parsed.
 */
static void
php_gui_parser_entity_decl (void *ctx,
                const xmlChar *name,
                int type,
                const xmlChar *publicId,
                const xmlChar *systemId,
                xmlChar *content)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

}

static void
php_gui_parser_attribute_decl(void *ctx,
                const xmlChar *elem,
                const xmlChar *fullname,
                int type,
                int def,
                const xmlChar *defaultValue,
                xmlEnumerationPtr tree)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)


}


static void
php_gui_parser_start_document(void *ctx)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)



    user_data->window = 0;
    user_data->container = 0;
    user_data->deep = 0;
    user_data->is_end = 0;

#if 0
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);

    // clear context
    //DomDocument *document = dom_document_new("UTF-8", "1.0");

#if 0
    state->state = PARSER_START;

    state->unknown_depth = 0;
    state->prev_state = PARSER_UNKNOWN;

    state->widget_depth = 0;
    state->content = g_string_sized_new(128);

    state->interface = g_new0(GladeInterface, 1);
    state->interface->names = g_hash_table_new(g_str_hash, g_str_equal);
    state->interface->strings = g_hash_table_new_full(g_str_hash,
                              g_str_equal,
                              (GDestroyNotify)g_free,
                              NULL);
    state->widget = NULL;

    state->prop_type = PROP_NONE;
    state->prop_name = NULL;
    state->prop_agent = NULL;
    state->translate_prop = FALSE;
    state->props = NULL;

    state->signals = NULL;
    state->accels = NULL;
#endif
#endif
}

static void
php_gui_parser_end_document(PhpGuiParserContext *ctx)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

    user_data->deep = 0;

#if 0
    g_string_free(state->content, TRUE);

    if (state->unknown_depth != 0)
    g_warning("unknown_depth != 0 (%d)", state->unknown_depth);
    if (state->widget_depth != 0)
    g_warning("widget_depth != 0 (%d)", state->widget_depth);
#endif
}

#define SVG_URI "http://w3c.org/2009/svg"

#if 0
typedef php_gtk_widget *(*gtkml_element_factory)(char **attrs);
typedef struct {char *element_name; gtkml_element_factory factory; int internal;} FactoryEntry;


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

#endif


static void
php_gui_parser_start_element (void *ctx,
                              const xmlChar *name,
                              const xmlChar **atts)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;
    user_data->deep++;

    trace(__FUNCTION__, user_data)

    trace(name, user_data)

    // TODO, map tab, function
    // GTKMLDocument::registerDomClass
    // GTKMLDocument::registerGtkClass
    // GTKMLWindowElement::registerGtkClass
    // TODO map element to function

    /*
    entry *table = get_map();
    entry = map_find(table, name);
    entry = map_set(table, name);
    */

    // if element root is not window, but label(par ex) alors crer une window
    gtkml_element_factory factory = php_gui_get_element_factory(name);
    //g_print("factory=%p\n", factory);

    // DTD specification:
    /*if (0==g_strcmp0(name, "application")) {

    } else */if (0==g_strcmp0(name, "window")) {
        if (user_data->window==NULL && user_data->deep==1) {
            php_gtk_widget *gobj = factory(atts);// gtkml_window_new(atts)

            user_data->window = gobj;
            user_data->container = gobj;
        }
    } else {
        if (user_data->window==NULL) {
            // create a window
            /*
            php_gtk_widget *gobj = gtkml_window_new(atts);

            user_data->window = gobj;
            user_data->container = gobj;
            */
        }


    }

    // switch()
    // case top_application
    // case top_window
    // case sub_window
    // case sub_widget



#if 0
    GError *error = NULL;
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;
    DomDocument *document = php_gui_parser_get_document(parser);
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "\t%s", BAD_CAST name);


    g_assert(document!=NULL);

    // TODO: parse name=ns:name

    DomElement *element = dom_document_create_element_ns(document, SVG_URI, (DomString *)name, &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
        return;
    }
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "\t@%p", element);
    // FIXME: use dom_node_parser_append_child
    dom_node_append_child(parser->element, DOM_NODE(element), &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
        g_clear_object(&element);
        return;
    }
    parser->element = DOM_NODE(element);

    if (atts!=NULL) {
        int i;
        for(i=0; atts[i]!=NULL; i+=2) {
            //g_print("attr: %s\n", atts[i]);// <input  disabled /> use libxml2 module HTMLParser
            g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "\t%s=\"%s\"", atts[i], atts[i+1]);

//            DomAttribute *attr = dom_document_create_attribute_ns(document, SVG_URI, BAD_CAST atts[i], &error);
//            GValue val = G_VALUE_INIT;
//            g_value_init(&val, G_TYPE_STRING);
//            g_value_set_string(&val, BAD_CAST atts[i+1]);
//            g_object_set_property(G_OBJECT(attr), "value", &val);

            //dom_element_set_attribute_node_ns(element, DOM_ATTRIBUTE(attr), &error);
            DomString *attr_name = (DomString*)atts[i];//dom_attribute_get_name(attr);
            DomString *attr_value = (DomString*)atts[i+1];//dom_attribute_get_value(attr);
            dom_element_set_attribute(element, attr_name, attr_value, &error);
        }
    }
#endif

}

static void
php_gui_parser_end_element (void *ctx,
                const xmlChar *name)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;
    user_data->deep--;

    trace(__FUNCTION__, user_data)

    trace(name, user_data)

    if(user_data->deep==0) {
        user_data->is_end = 1;
    }

#if 0
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;

    parser->element = dom_node_get_parent_node(parser->element);
#endif
}

static void
php_gui_parser_characters (void* ctx,
                       const xmlChar* ch,
                       int len)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

    char *str = g_strndup(ch, len);
    GtkWidget *label = gtk_label_new(str);
    gtk_label_set_markup(GTK_LABEL(label), str);
    g_free(str);
    gtk_container_add(GTK_WINDOW(user_data->window->ptr), label);
    gtk_widget_show(label);


#if 0
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);

    PhpGuiParserContext *parser = (PhpGuiParserContext *)user_data;
    DomDocument *document = php_gui_parser_get_document(parser);

    //g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", g_strndup(ch, len));

    GError *error = NULL;
    DomString *data = g_strndup(ch, len);
    DomText *element = dom_document_create_text_node(document, data, NULL);
    g_free(data);
    dom_node_append_child(parser->element, DOM_NODE(element), &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
    }


    //DOM_IS_COMMENT(parser->element)
    //DOM_IS_STYLE(parser->element)
    //DOM_IS_TEXT(parser->element)
    /*
    DomString *node_name = dom_node_get_node_name(DOM_NODE(parser->element));
    //DomNodeCode node_code = dom_node_get_node_code(DOM_NODE(parser->element));
    if (g_str_equal(node_name, "style")) {
        //svg_element_style_set_content(parser->element, g_strndup(ch, len));
        DomNode *text_node = g_strndup(ch, len);//dom_text_node;
        dom_node_append_child(DOM_NODE(parser->element), text_node, NULL);
    }
    */
#endif
}

static void
php_gui_parser_cdata_section (void *ctx, const xmlChar *value, int len)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

#if 0
    GError *error = NULL;
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);

    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;

#if 1
    DomCDataSection *cdata = dom_document_create_cdata_section(parser->document, value, &error);
    dom_node_append_child(parser->element, DOM_NODE(cdata), &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
    } else {
        //parser->element = DOM_NODE(comment);
    }

#endif

    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", g_strndup(value, len));

#endif
}

/**
 * commentSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @value:  the comment content
 *
 * A comment has been parsed.
 */
static void
php_gui_parser_comment (void *ctx,
                    const xmlChar *value)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

#if 0
    GError *error = NULL;
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", value);

#if 1
    DomComment *comment = dom_document_create_comment(parser->document, value/*, &error*/);
    // TODO : if document == NULL; then use parser->virtual_node==new_node
    DomNode *new_node = dom_node_append_child(parser->element, DOM_NODE(comment), &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
    } else {
        //parser->element = DOM_NODE(comment);
    }

#endif

    //parser->element = dom_node_get_parent_node(parser->element);
    // document->create_element();
    // append
#endif
}



/**
 * processingInstructionSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @target:  the target name
 * @data: the PI data's
 *
 * A processing instruction has been parsed.
 */
static void
php_gui_parser_processing_instruction (void *ctx, const xmlChar *target, const xmlChar *data)
{
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)

#if 0
    GError *error = NULL;
    PhpGuiParserContext *parser = (PhpGuiParserContext *)ctx;

    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "%s", G_STRFUNC);
    g_log(DOM_PARSER_LOG_DOMAIN, G_LOG_LEVEL_INFO, "\t%s ; %s", target, data);
    // xml-stylesheet ; href="rects.css" type="text/css"
    // xml-stylesheet ; href="docum.xsl" type="text/xsl"

    //parser->element

    //doc = parser->document
    // doc->pi(XML_STYLE_SHEET);
#if 1
    DomNode *pi = dom_document_create_processing_instruction(NULL, target, data, &error);

    parser->processing_instruction_list = g_list_append(parser->processing_instruction_list, pi);

#endif

#if 0
    dom_node_append_child(parser->element, DOM_NODE(comment), &error);
    if (error) {
        g_print("ERROR:             %s\n", error->message);
    } else {
        //parser->element = DOM_NODE(comment);
    }
#endif

#endif
}

#include <stdarg.h>
#include <stdio.h>
static void
php_gui_parser_warning(void *ctx, const char *msg, ...) {
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)


    va_list varlist;
    va_start(varlist, msg);
    char *str = va_arg(varlist, char*);

    char *message = g_strdup_printf(msg, str);
    trace(message, user_data)

    va_end(varlist);
}

static void
php_gui_parser_error(void *ctx, const char *msg, ...) {

    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)


    va_list varlist;
    va_start(varlist, msg);
    char *str = va_arg(varlist, char*);

    char *message = g_strdup_printf(msg, str);
    trace(message, user_data)

    va_end(varlist);

#if 0
    PhpGuiParserContext *parser = (PhpGuiParserContext *)context;
    va_list varlist;
    va_start(varlist, msg);
    char *str = va_arg(varlist, char*);

    g_print("%s\n", "php_gui_parser_error");

    if (parser->last_error!=NULL) {
        g_print("php_gui_parser_error: %s\n", parser->last_error->message);
        g_clear_error(&parser->last_error);
    }
    g_set_error (&parser->last_error,
                 DOM_EXCEPTION_ERROR,                 // error domain
                 NOT_FOUND_ERR,            // error code
                 msg, // error message format string
                 str);
    va_end(varlist);
#endif
}

static void
php_gui_parser_fatal_error(void *ctx, const char *msg, ...) {
    xmlParserCtxtPtr context = (xmlParserCtxtPtr)ctx;
    PhpGuiParserContext *user_data = context->_private;

    trace(__FUNCTION__, user_data)


    va_list varlist;
    va_start(varlist, msg);
    char *str = va_arg(varlist, char*);

    char *message = g_strdup_printf(msg, str);
    trace(message, user_data)

    va_end(varlist);
}

#if 0
struct _xmlSAXHandler {
    internalSubsetSAXFunc internalSubset;
    isStandaloneSAXFunc isStandalone;
    hasInternalSubsetSAXFunc hasInternalSubset;
    hasExternalSubsetSAXFunc hasExternalSubset;
    resolveEntitySAXFunc resolveEntity;
    getEntitySAXFunc getEntity;
    entityDeclSAXFunc entityDecl;
    notationDeclSAXFunc notationDecl;
    attributeDeclSAXFunc attributeDecl;
    elementDeclSAXFunc elementDecl;
    unparsedEntityDeclSAXFunc unparsedEntityDecl;
    setDocumentLocatorSAXFunc setDocumentLocator;
    startDocumentSAXFunc startDocument;
    endDocumentSAXFunc endDocument;
    startElementSAXFunc startElement;
    endElementSAXFunc endElement;
    referenceSAXFunc reference;
    charactersSAXFunc characters;
    ignorableWhitespaceSAXFunc ignorableWhitespace;
    processingInstructionSAXFunc processingInstruction;
    commentSAXFunc comment;
    warningSAXFunc warning;
    errorSAXFunc error;
    fatalErrorSAXFunc fatalError; /* unused error() get all the errors */
    getParameterEntitySAXFunc getParameterEntity;
    cdataBlockSAXFunc cdataBlock;
    externalSubsetSAXFunc externalSubset;
    unsigned int initialized;
    /* The following fields are extensions available only on version 2 */
    void *_private;
    startElementNsSAX2Func startElementNs;
    endElementNsSAX2Func endElementNs;
    xmlStructuredErrorFunc serror;
};
#endif

#if 0
int
dom_document_load(const char *uri, GtkWindow *document, GError **error)
{

    LIBXML_TEST_VERSION

    xmlSAXHandler sh = {0, };
    //sh.isStandalone   = (isStandaloneSAXFunc) php_gui_parser_is_standalone;
    sh.internalSubset = (internalSubsetSAXFunc) php_gui_parser_internal_subset;
    sh.resolveEntity  = (resolveEntitySAXFunc) php_gui_parser_resolve_entity;
    sh.entityDecl     = (entityDeclSAXFunc) php_gui_parser_entity_decl;// <!ENTITY
    sh.attributeDecl  = (attributeDeclSAXFunc) php_gui_parser_attribute_decl;
    sh.startDocument  = (startDocumentSAXFunc) php_gui_parser_start_document;
    sh.endDocument    = (endDocumentSAXFunc) php_gui_parser_end_document;
    sh.startElement   = (startElementSAXFunc) php_gui_parser_start_element;
    /// TODO:
    //sh.notationDecl <!NOTATION
    sh.characters     = (charactersSAXFunc) php_gui_parser_characters;
    sh.cdataBlock     = (cdataBlockSAXFunc) php_gui_parser_cdata_section;
    sh.comment        = (commentSAXFunc) php_gui_parser_comment;
    sh.endElement     = (endElementSAXFunc) php_gui_parser_end_element;
    sh.processingInstruction = (processingInstructionSAXFunc) php_gui_parser_processing_instruction;
    sh.warning        = (warningSAXFunc) php_gui_parser_warning;
    sh.error          = (errorSAXFunc) php_gui_parser_error;
    sh.fatalError     = (fatalErrorSAXFunc) php_gui_parser_fatal_error;

    xmlParserCtxtPtr ctxt  = xmlNewParserCtxt();
    if (ctxt == NULL) {
        fprintf(stderr, "Failed to allocate parser context\n");
        return FALSE;
    }
    ctxt->sax = &sh;
    xmlParserInputPtr *input = xmlNewInputFromFile(ctxt, uri);
    if (input==NULL) {
        g_print("No such file or directory\n");
        return FALSE;
    }
    ctxt->input = input;

    DomParserContext parser;
    parser.status = 0;
    parser.last_error = NULL;
    parser.processing_instruction_list = NULL;
    parser.doctype = NULL;//dom_document_get_doctype(parser.document);
    parser.document = *document;
    parser.element = DOM_NODE(parser.document);
    parser.ctxt = ctxt;
    ctxt->userData = &parser;

    // Analyse
    xmlParseDocument(ctxt);

    if (ctxt->wellFormed==0) {
        fprintf(stderr, "Document XML mal formé\n");
        ctxt->sax = 0;
        xmlFreeParserCtxt(ctxt);
        return FALSE;
    }

    *document = ((DomParserContext*)ctxt->userData)->document;// my DOM
    (*document)->priv->xml_standalone = (ctxt->input->standalone == 1);
    (*document)->priv->document_uri = g_strdup(uri);//ctxt->input->filename);

    // Libération de la mémoire
    ctxt->sax = 0;
    xmlFreeParserCtxt(ctxt);

    return TRUE;
}
#endif

#if 0
/**
 * section: Parsing
 * synopsis: Parse an XML document chunk by chunk to a tree and free it
 * purpose: Demonstrate the use of xmlCreatePushParserCtxt() and
 *          xmlParseChunk() to read an XML file progressively
 *          into a tree and xmlFreeDoc() to free the resulting tree
 * usage: parse4 test3.xml
 * test: parse4 test3.xml
 * author: Daniel Veillard
 * copy: see Copyright for the status of this software.
 */




/**
 * readPacket:
 * @mem: array to store the packet
 * @size: the packet size
 *
 * read at most @size bytes from the document and store it in @mem
 *
 * Returns the number of bytes read
 */
static int
readPacket(char *mem, int size) {
    int res;

    res = fread(mem, 1, size, desc);
    return(res);
}
#endif

static int ioread(void * context, char * buffer, int len)
{
#if 0
    // Documentation show this example, but it don't work in my case:
    int fd = *((int *) context);
    return read(fd, buffer, len);
#else

    /**
SYNOPSIS
       #include <stdio.h>
       size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

RETURN VALUE
       On  success,  fread()  and  fwrite() return the number of items read or
       written.  This number equals the number of bytes transferred only  when
       size  is 1.  If an error occurs, or the end of the file is reached, the
       return value is a short item count (or zero).

       fread() does not distinguish between end-of-file and error, and callers
       must use feof(3) and ferror(3) to determine which occurred.
     */
    static int loop = 0;

    //trace

    //FILE *fd = (FILE *)context;
    int nread = 0;
    size_t item_size = 1;
    //size_t ret = fread(buffer, (size_t)len, item_size, desc);
    size_t ret = fread(buffer, len, item_size, context);
        //g_print("%p\n", ret);
        //buffer[len]='\0';
        //g_print("%s\n", buffer);

        if (nread==1) {
            nread = len;
        } else if (feof(context)) {
            nread = strlen(buffer);
            //g_print("%d\n", nread);
            //g_print("%s\n", buffer);
        } else {
            ferror(context);
            nread = -1;
        }

    return nread;// Return the number of bytes read or -1 in case of error
#endif
}

static int ioclose(void * context)
{
    //trace
    /*
    if (*desc > 2) {
        return fclose(desc);
    }
    */
    return 0;
}

#if 0
/**
 * example4Func:
 * @filename: a filename or an URL
 *
 * Parse the resource and free the resulting tree
 */
static void
example4Func(const char *filename) {
    xmlParserCtxtPtr ctxt;
    char chars[4];
    xmlDocPtr doc; /* the resulting document tree */
    int res;

    PhpGuiParserContext user_data;
    user_data.status = 6;


    xmlSAXHandler sh = {0};
    //sh.isStandalone   = (isStandaloneSAXFunc) php_gui_parser_is_standalone;
    sh.internalSubset = (internalSubsetSAXFunc) php_gui_parser_internal_subset;
    sh.resolveEntity  = (resolveEntitySAXFunc) php_gui_parser_resolve_entity;
    sh.entityDecl     = (entityDeclSAXFunc) php_gui_parser_entity_decl;// <!ENTITY
    sh.attributeDecl  = (attributeDeclSAXFunc) php_gui_parser_attribute_decl;
    sh.startDocument  = (startDocumentSAXFunc) php_gui_parser_start_document;
    sh.endDocument    = (endDocumentSAXFunc) php_gui_parser_end_document;
    sh.startElement   = (startElementSAXFunc) php_gui_parser_start_element;
    /// TODO:
    //sh.notationDecl <!NOTATION
    sh.characters     = (charactersSAXFunc) php_gui_parser_characters;
    sh.cdataBlock     = (cdataBlockSAXFunc) php_gui_parser_cdata_section;
    sh.comment        = (commentSAXFunc) php_gui_parser_comment;
    sh.endElement     = (endElementSAXFunc) php_gui_parser_end_element;
    sh.processingInstruction = (processingInstructionSAXFunc) php_gui_parser_processing_instruction;
    sh.warning        = (warningSAXFunc) php_gui_parser_warning;
    sh.error          = (errorSAXFunc) php_gui_parser_error;
    sh.fatalError     = (fatalErrorSAXFunc) php_gui_parser_fatal_error;


#if 1



    //void *ioctx = desc;
    void *ioctx = stdout;

    //fprintf(stdout, "<doc>%s</doc>", "Hello");
    //xmlCtxtReadIO(f;
    xmlCharEncoding enc = XML_CHAR_ENCODING_UTF8;
    if ((ctxt = xmlCreateIOParserCtxt(&sh, NULL, ioread, ioclose, ioctx, enc)) == NULL) {
        printf("Erreur lors de la création du contexte\n");// stderr
        return EXIT_FAILURE;
    }
    //xmlParseDocument(ctxt);
    //xmlParseChunk(ctxt, chars, 0, 1);

    char buffer[1000]="<doc>Hello</doc>    ";
    xmlParseChunk(ctxt, buffer, 16, 0);
    /*
     * there is no more input, indicate the parsing is finished.
     */
    //xmlParseChunk(ctxt, chars, 0, 1);


#else
    /*
     * Read a few first byte to check the input used for the
     * encoding detection at the parser level.
     */
    res = readPacket(chars, 4);
    if (res <= 0) {
        fprintf(stderr, "Failed to parse %s\n", filename);
        return;
    }

    /*
     * Create a progressive parsing context, the 2 first arguments
     * are not used since we want to build a tree and not use a SAX
     * parsing interface. We also pass the first bytes of the document
     * to allow encoding detection when creating the parser but this
     * is optional.
     */
    ctxt = xmlCreatePushParserCtxt(&sh, NULL, chars, res, filename);
    //ctxt->userData // used for bui DOM but not used in our case;
    ctxt->_private = &user_data;// using _private is more flexible

    if (ctxt == NULL) {
        fprintf(stderr, "Failed to create parser context !\n");
    return;
    }

    /*
     * loop on the input getting the document data, of course 4 bytes
     * at a time is not realistic but allows to verify testing on small
     * documents.
     */
    while ((res = readPacket(chars, 4)) > 0) {
        xmlParseChunk(ctxt, chars, res, 0);
    }

    /*
     * there is no more input, indicate the parsing is finished.
     */
    xmlParseChunk(ctxt, chars, 0, 1);
#endif

    /*
     * collect the document back and if it was wellformed
     * and destroy the parser context.
     */
    doc = ctxt->myDoc;
    res = ctxt->wellFormed;
    xmlFreeParserCtxt(ctxt);

    if (!res) {
        fprintf(stderr, "Failed to parse %s\n", filename);
    }

    /*
     * since we don't use the document, destroy it now.
     */
    xmlFreeDoc(doc);
}

int php_gui_load(char *argv) {

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION
    /*
     * simulate a progressive parsing using the input file.
     */
    desc = fopen("/home/dev/Projects/php-ubuntu/ext/gtkml/gtkml.php", "rb");
    if (desc != NULL) {
        example4Func(argv);
//        char buffer[4000];
//        int len = 100;
//        size_t nread = fread(buffer, 12, 1, desc);
//        g_print("%p\n", nread);
//        g_print("%s\n", buffer);


        fclose(desc);
    } else {
        fprintf(stderr, "Failed to parse %s\n", argv);
    }

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    return(0);
}
#endif

static xmlSAXHandler sh = {0};
static PhpGuiParserContext user_data;

static void php_gui_parser_init() {
    // sh...
}

xmlParserCtxtPtr php_gui_parser_create(const char *filename) {
    xmlCharEncoding enc = XML_CHAR_ENCODING_UTF8;

    //sh.isStandalone   = (isStandaloneSAXFunc) php_gui_parser_is_standalone;
    sh.internalSubset = (internalSubsetSAXFunc) php_gui_parser_internal_subset;
    sh.resolveEntity  = (resolveEntitySAXFunc) php_gui_parser_resolve_entity;
    sh.entityDecl     = (entityDeclSAXFunc) php_gui_parser_entity_decl;// <!ENTITY
    sh.attributeDecl  = (attributeDeclSAXFunc) php_gui_parser_attribute_decl;
    sh.startDocument  = (startDocumentSAXFunc) php_gui_parser_start_document;
    sh.endDocument    = (endDocumentSAXFunc) php_gui_parser_end_document;
    sh.startElement   = (startElementSAXFunc) php_gui_parser_start_element;
    /// TODO:
    //sh.notationDecl <!NOTATION
    sh.characters     = (charactersSAXFunc) php_gui_parser_characters;
    sh.cdataBlock     = (cdataBlockSAXFunc) php_gui_parser_cdata_section;
    sh.comment        = (commentSAXFunc) php_gui_parser_comment;
    sh.endElement     = (endElementSAXFunc) php_gui_parser_end_element;
    sh.processingInstruction = (processingInstructionSAXFunc) php_gui_parser_processing_instruction;
    sh.warning        = (warningSAXFunc) php_gui_parser_warning;
    sh.error          = (errorSAXFunc) php_gui_parser_error;
    sh.fatalError     = (fatalErrorSAXFunc) php_gui_parser_fatal_error;

    //xmlParserCtxtPtr parser = xmlCreateIOParserCtxt(&sh, NULL, ioread, ioclose, ioctx, enc);
    char chars[1000]={0};
    int res=0;
    xmlParserCtxtPtr parser = xmlCreatePushParserCtxt(&sh, NULL, NULL, 0, NULL);
    parser->_private = &user_data;

    //user_data.deep = 0;
    //user_data.is_end = 0;
    user_data.status = 6;
    user_data.fd_debug = fopen("/home/dev/Documents/php_dom/debug.txt", "w");
    fputs("php_gui_parser_create---------------------------------------------\n", user_data.fd_debug);

    return parser;
}

