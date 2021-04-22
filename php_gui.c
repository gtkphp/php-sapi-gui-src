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

//Declare : HAVE_CONFIG, GUARDIANE
//Declare : Includes, Macros, Constants, Types, Functions
//Define : Function, Entry point

/**
  +----------------------------------------------------------------------+
  | Includes declaration                                                 |
  +----------------------------------------------------------------------+
*/

/** external dependency -------------------------------------------------+
 */

#include <stdio.h>
#include <gtk/gtk.h>        // gtk_init, gtk_main
#include <libxml/parser.h> // xmlParserCtxtPtr

/** internal dependency -------------------------------------------------+
*/

#include "php.h"           // zend_object, ...
#include "php_main.h"
#include "php_ini.h"
#include "php_getopt.h"
#include "php_network.h"

#include "SAPI.h"

//#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h" // default_socket_timeout

/** self declaration ----------------------------------------------------+
*/
#include "gui.h"


/** extra dependency ----------------------------------------------------+
*/

#include "ext/gtk/php_gobject/object.h"

#include "php_gui_parser.h"


/**
  +----------------------------------------------------------------------+
  | Macros declaration                                                   |
  +----------------------------------------------------------------------+
*/

//#define trace printf("%s\n", __FUNCTION__);
#define trace

#ifndef PHP_WIN32
# define php_select(m, r, w, e, t)	select(m, r, w, e, t)
#else
# include "win32/select.h"
#endif

/**
  +----------------------------------------------------------------------+
  | Constants declaration                                                |
  +----------------------------------------------------------------------+
*/

#define PHP_MODE_STANDARD      1
#define PHP_MODE_PROCESS_STDIN 7
#define PHP_MODE_SHOW_INI_CONFIG        13


/**
  +----------------------------------------------------------------------+
  | Types declaration                                                    |
  +----------------------------------------------------------------------+
*/
/* The global structure */
typedef struct _PhpGuiContext PhpGuiContext;

/**
  +----------------------------------------------------------------------+
  | Functions declaration                                                |
  +----------------------------------------------------------------------+
*/

/** SAPI module handlers ------------------------------------------------+
*/
static int php_gui_startup(sapi_module_struct *sapi_module);
static int sapi_gui_deactivate(void);
static size_t sapi_gui_ub_write(const char *str, size_t str_length);
static void sapi_gui_flush(void *server_context);
static int sapi_gui_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s);
static int sapi_gui_send_headers(sapi_headers_struct *sapi_headers);
static void sapi_gui_send_header(sapi_header_struct *sapi_header, void *server_context);
static char* sapi_gui_read_cookies(void);
static void sapi_gui_register_variables(zval *track_vars_array);
static void sapi_gui_log_message(char *message, int syslog_type_int);

/**
  +----------------------------------------------------------------------+
  | Structures declaration                                               |
  +----------------------------------------------------------------------+
*/

/*{{{ PhpGuiContext
 * @foo : a int
 *
 * Structure
 */
struct _PhpGuiContext {
    int bar;// see https://developer.gnome.org/glib/stable/glib-Commandline-option-parser.html
    //FILE *fd_debug;
    xmlParserCtxtPtr parser;
    struct {
        int r;
    } options;// https://github.com/frida/glib/blob/master/gio/tests/gapplication-example-cmdline3.c
};/*}}} */

gui_shell_callbacks_t gui_shell_callbacks = { NULL, NULL, NULL };
PHP_GUI_API gui_shell_callbacks_t *php_gui_get_shell_callbacks()
{
    return &gui_shell_callbacks;
}

/**
  +----------------------------------------------------------------------+
  | Static variable declaration                                          |
  +----------------------------------------------------------------------+
*/


PhpGuiContext ctxt;


const char HARDCODED_INI[] =
    "html_errors=0\n"
    "register_argc_argv=1\n"
    "implicit_flush=1\n"
    "output_buffering=0\n"
    "max_execution_time=0\n"
    "max_input_time=-1\n\0";

const opt_struct OPTIONS[] = {
    {'a', 0, "interactive"},
    {'B', 1, "process-begin"},
    {'C', 0, "no-chdir"}, /* for compatibility with CGI (do not chdir to script directory) */
    {'c', 1, "php-ini"},
    {'d', 1, "define"},
    {'E', 1, "process-end"},
    {'e', 0, "profile-info"},
    {'F', 1, "process-file"},
    {'f', 1, "file"},
    {'h', 0, "help"},
    {'i', 0, "info"},
    {'l', 0, "syntax-check"},
    {'m', 0, "modules"},
    {'n', 0, "no-php-ini"},
    {'q', 0, "no-header"}, /* for compatibility with CGI (do not generate HTTP headers) */
    {'R', 1, "process-code"},
    {'H', 0, "hide-args"},
    {'r', 1, "run"},
    {'s', 0, "syntax-highlight"},
    {'s', 0, "syntax-highlighting"},
    {'S', 1, "server"},
    {'t', 1, "docroot"},
    {'w', 0, "strip"},
    {'?', 0, "usage"},/* help alias (both '?' and 'usage') */
    {'v', 0, "version"},
    {'z', 1, "zend-extension"},
    {10,  1, "rf"},
    {10,  1, "rfunction"},
    {11,  1, "rc"},
    {11,  1, "rclass"},
    {12,  1, "re"},
    {12,  1, "rextension"},
    {13,  1, "rz"},
    {13,  1, "rzendextension"},
    {14,  1, "ri"},
    {14,  1, "rextinfo"},
    {15,  0, "ini"},
    {'-', 0, NULL} /* end of args */
};

static const char *param_mode_conflict =
        "Either execute direct code, process stdin or use a file.\n";


/* {{{ sapi_module_struct gui_sapi_module
 */
static sapi_module_struct gui_sapi_module = {
    "gui",							/* name */
    "GTK+PHP Interface",            /* pretty name */

    php_gui_startup,				/* startup */
    php_module_shutdown_wrapper,	/* shutdown */

    NULL,							/* activate */
    sapi_gui_deactivate,			/* deactivate */

    sapi_gui_ub_write,		    	/* unbuffered write */
    sapi_gui_flush,				    /* flush */
    NULL,							/* get uid */
    NULL,							/* getenv */

    php_error,						/* error handler */

    sapi_gui_header_handler,		/* header handler */
    sapi_gui_send_headers,			/* send headers handler */
    sapi_gui_send_header,			/* send header handler */

    NULL,				            /* read POST data */
    sapi_gui_read_cookies,          /* read Cookies */

    sapi_gui_register_variables,	/* register server variables */
    sapi_gui_log_message,			/* Log message */
    NULL,							/* Get request time */
    NULL,							/* Child terminate */

    STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

static char *php_self = "";
static char *script_filename = "";

/**
  +----------------------------------------------------------------------+
  | Functions definition                                                 |
  +----------------------------------------------------------------------+
*/

static int sapi_gui_deactivate(void) /* {{{ */
{
    trace

    fflush(stdout);
    if(SG(request_info).argv0) {
        free(SG(request_info).argv0);
        SG(request_info).argv0 = NULL;
    }

    return SUCCESS;
}
/* }}} sapi_gui_deactivate */

// TODO sax
// au fur et a mesur qu'on parse, on met a jour Gtk; on et on event_pending entre chaque ub_write

static inline int sapi_gui_select(php_socket_t fd)
{
    fd_set wfd, dfd;
    struct timeval tv;
    int ret;

    FD_ZERO(&wfd);
    FD_ZERO(&dfd);

    PHP_SAFE_FD_SET(fd, &wfd);

    tv.tv_sec = (long)FG(default_socket_timeout);
    tv.tv_usec = 0;

    ret = php_select(fd+1, &dfd, &wfd, &dfd, &tv);

    return ret != -1;
}

PHP_GUI_API size_t sapi_gui_single_write(const char *str, size_t str_length) /* {{{ */
{
#ifdef PHP_WRITE_STDOUT
    zend_long ret;
#else
    size_t ret;
#endif

    if (gui_shell_callbacks.gui_shell_write) {
        gui_shell_callbacks.gui_shell_write(str, str_length);
    }




#ifdef PHP_WRITE_STDOUT
    do {
        ret = write(STDOUT_FILENO, str, str_length);
#if 1
        PhpGuiParserContext *parser = ctxt.parser->_private;

        // php_gui_parser_log();
        //gchar *msg = g_strdup_printf("<<<%s>>>%d\n", str, str_length);
        //fputs(msg, parser->fd_debug);

        xmlParseChunk(ctxt.parser, str, str_length, 0);
        // TODO:
        /*
         * there is no more input, indicate the parsing is finished.
         */
        //xmlParseChunk(ctxt, chars, 0, 1);
        if (parser->is_end) {
            xmlParseChunk(ctxt.parser, str, 0, 1);
        }



#endif
    } while (ret <= 0 && errno == EAGAIN && sapi_gui_select(STDOUT_FILENO));

    if (ret <= 0) {
        return 0;
    }

    return ret;
#else
    ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
    return ret;
#endif
}
/* }}} */


/* {{{ sapi_gui_ub_write
*/
static size_t sapi_gui_ub_write(const char *str, size_t str_length)
{
    trace

    const char *ptr = str;
    size_t remaining = str_length;
    size_t ret;

    if (!str_length) {
        return 0;
    }

    if (gui_shell_callbacks.gui_shell_ub_write) {
        size_t ub_wrote;
        ub_wrote = gui_shell_callbacks.gui_shell_ub_write(str, str_length);
        if (ub_wrote != (size_t) -1) {
            return ub_wrote;
        }
    }

    while (remaining > 0)
    {
        ret = sapi_gui_single_write(ptr, remaining);
        if (!ret) {
#if 0
#ifndef PHP_CLI_WIN32_NO_CONSOLE
            php_handle_aborted_connection();
#endif
#endif
            break;
        }
        ptr += ret;
        remaining -= ret;
    }
    return (ptr - str);
}
/* }}} */

static void sapi_gui_flush(void *server_context) /* {{{ */
{
    trace
    /* Ignore EBADF here, it's caused by the fact that STDIN/STDOUT/STDERR streams
     * are/could be closed before fflush() is called.
     */
    if (fflush(stdout)==EOF && errno!=EBADF) {
#ifndef PHP_CLI_WIN32_NO_CONSOLE
        php_handle_aborted_connection();
#endif
    }
}
/* }}} */

static int sapi_gui_header_handler(sapi_header_struct *h, sapi_header_op_enum op, sapi_headers_struct *s) /* {{{ */
{
    trace
    return 0;
}
/* }}} */

static int sapi_gui_send_headers(sapi_headers_struct *sapi_headers) /* {{{ */
{
    trace
    /* We do nothing here, this function is needed to prevent that the fallback
     * header handling is called. */
    return SAPI_HEADER_SENT_SUCCESSFULLY;
}
/* }}} */

static void sapi_gui_send_header(sapi_header_struct *sapi_header, void *server_context) /* {{{ */
{
    trace
}
/* }}} */

static char* sapi_gui_read_cookies(void) /* {{{ */
{
    trace
    return NULL;
}
/* }}} */

static void sapi_gui_register_variables(zval *track_vars_array) /* {{{ */
{
    trace
#if 0
    size_t len;
    char   *docroot = "";
    /* In CGI mode, we consider the environment to be a part of the server
     * variables
     */
    php_import_environment_variables(track_vars_array);
    /* Build the special-case PHP_SELF variable for the CLI version */
    len = strlen(php_self);
    if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &php_self, len, &len)) {
        php_register_variable("PHP_SELF", php_self, track_vars_array);
    }
#endif
#if 0
    if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_NAME", &php_self, len, &len)) {
        php_register_variable("SCRIPT_NAME", php_self, track_vars_array);
    }
    /* filenames are empty for stdin */
    len = strlen(script_filename);
    if (sapi_module.input_filter(PARSE_SERVER, "SCRIPT_FILENAME", &script_filename, len, &len)) {
        php_register_variable("SCRIPT_FILENAME", script_filename, track_vars_array);
    }
    if (sapi_module.input_filter(PARSE_SERVER, "PATH_TRANSLATED", &script_filename, len, &len)) {
        php_register_variable("PATH_TRANSLATED", script_filename, track_vars_array);
    }
    /* just make it available */
    len = 0U;
    if (sapi_module.input_filter(PARSE_SERVER, "DOCUMENT_ROOT", &docroot, len, &len)) {
        php_register_variable("DOCUMENT_ROOT", docroot, track_vars_array);
    }
#endif
}
/* }}} */

static void sapi_gui_log_message(char *message, int syslog_type_int) /* {{{ */
{
    trace
#if 0
    fprintf(stderr, "%s\n", message);
#ifdef PHP_WIN32
    fflush(stderr);
#endif
#endif
}
/* }}} */


/* {{{ php_gui_startup
*/
static int php_gui_startup(sapi_module_struct *sapi_module)
{
    trace
    if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
        return FAILURE;
    }
    return SUCCESS;
}
/* }}} php_gui_startup */



/* {{{ gui_seek_file_begin
 */
static int gui_seek_file_begin(zend_file_handle *file_handle, char *script_file, int *lineno)
{
    int c;

    *lineno = 1;

    file_handle->type = ZEND_HANDLE_FP;
    file_handle->opened_path = NULL;
    file_handle->free_filename = 0;
    if (!(file_handle->handle.fp = VCWD_FOPEN(script_file, "rb"))) {
        php_printf("Could not open input file: %s\n", script_file);
        return FAILURE;
    }
    file_handle->filename = script_file;

    /* #!php support */
    c = fgetc(file_handle->handle.fp);
    if (c == '#' && (c = fgetc(file_handle->handle.fp)) == '!') {
        while (c != '\n' && c != '\r' && c != EOF) {
            c = fgetc(file_handle->handle.fp);	/* skip to end of line */
        }
        /* handle situations where line is terminated by \r\n */
        if (c == '\r') {
            if (fgetc(file_handle->handle.fp) != '\n') {
                zend_long pos = zend_ftell(file_handle->handle.fp);
                zend_fseek(file_handle->handle.fp, pos - 1, SEEK_SET);
            }
        }
        *lineno = 2;
    } else {
        rewind(file_handle->handle.fp);
    }

    return SUCCESS;
}
/* }}} */


static void php_gui_usage(char *argv0)
{
    char *prog;

    prog = strrchr(argv0, '/');
    if (prog) {
        prog++;
    } else {
        prog = "php";
    }



#define UND "\e[4m"
#define RED "\e[1;31m"
#define BLU "\e[1;34m"
#define GRN "\e[1;32m"
#define RZT "\e[0m"

//
#if 1
    printf(
    "/*\n"
    "    +----------------------------------------------------------------------+\n"
    "    |                          GTK+PHP Version 0.1.0                 _ - X |\n"
    "    +----------------------------------------------------------------------+\n"
    "    | "UND"F"RZT"ile | "UND"V"RZT"iew | "UND"H"RZT"elp                                                   |\n"
    "    +----------------------------------------------------------------------+\n"
    "    | "GRN"Usage"RZT":                                                     | PHP   x |\n"
    "    |    php [options] <file> [options GTK] [--] [args...]       |         |\n"
    "    |                                                            |         |\n"
    "    | "GRN"Options"RZT":                                                   |         |\n"
    "    |    php --help options [GTK|GDK|G]                          |         |\n"
    "    |                                                            |_________|\n"
    "    | "GRN"Options GTK+"RZT":                                              | GTK+  x |\n"
    "    |                                                            |_________|\n"
    "    | "GRN"Options GDK"RZT":                                               | GDK   x |\n"
    "    |                                                            |_________|\n"
    "    | "GRN"Options G"RZT":                                                 | G     x |\n"
    "    |                                                            |         |\n"
    "    +----------------------------------------------------------------------+\n"
    "    | Author: DE VASCONCELOS Sergio <dev-5312910@php.net>                  |\n"
    "    +----------------------------------------------------------------------+\n"
    "*/\n"
    "/* $Id:$ echo \"<gtk:help />\" | php */\n"
    );




#else
#if 0
    #define MGT "\e[1;35m"
    #define BLU "\e[1;34m"
    #define YLW "\e[1;33m"
    #define GRN "\e[1;32m"
    #define RED "\e[1;31m"
    #define RZT "\e[0m"
#else
    #define MGT ""
    #define BLU ""
    #define YLW ""
    #define GRN ""
    #define RED ""
    #define RZT ""
#endif
    printf("Gtk+PHP "GRN"0.1.0"RZT" - Module Gtkml "YLW"0.1.0"RZT" (Gtk+: "YLW"3.22.24"RZT", PHP: 7.2)\n");
    printf( RED"Usage"RZT": %s <url>\n"
            "  %s [options] -r <code> [--] [args...]\n"
            "  %s [options] [-B <begin_code>] -R <code> [-E <end_code>] [--] [args...]\n"
            "  %s [options] [-B <begin_code>] -F <file> [-E <end_code>] [--] [args...]\n"
            "  %s [options] -S <addr>:<port> [-t docroot] [router]\n"
            "  %s [options] -- [args...]\n"
            "  %s [options] -a\n"
            "\n"
            RED"Options"RZT"\n"
            "  -a               Run interactively\n"
            "  -c <path>|<file> Look for php.ini file in this directory\n"
            "  -n               No configuration (ini) files will be used\n"
            "  -d foo[=bar]     Define INI entry foo with value 'bar'\n"
            "  -e               Generate extended information for debugger/profiler\n"
            "  -f <file>        Parse and execute <file>.\n"
            "  -h               This help\n"
            "  -i               PHP information\n"
            "  -l               Syntax check only (lint)\n"
            "  -m               Show compiled in modules\n"
            "  -r <code>        Run PHP <code> without using script tags <?..?>\n"
            "  -B <begin_code>  Run PHP <begin_code> before processing input lines\n"
            "  -R <code>        Run PHP <code> for every input line\n"
            "  -F <file>        Parse and execute <file> for every input line\n"
            "  -E <end_code>    Run PHP <end_code> after processing all input lines\n"
            "  -H               Hide any passed arguments from external tools.\n"
            "  -S <addr>:<port> Run with built-in web server.\n"
            "  -t <docroot>     Specify document root <docroot> for built-in web server.\n"
            "  -s               Output HTML syntax highlighted source.\n"
            "  -v               Version number\n"
            "  -w               Output source with stripped comments and whitespace.\n"
            "  -z <file>        Load Zend extension <file>.\n"
            "\n"
            "  args...          Arguments passed to script. Use -- args when first argument\n"
            "                   starts with - or script is read from stdin\n"
            "\n"
            "  --ini            Show configuration file names\n"
            "\n"
            "  --rf <name>      Show information about function <name>.\n"
            "  --rc <name>      Show information about class <name>.\n"
            "  --re <name>      Show information about extension <name>.\n"
            "  --rz <name>      Show information about Zend extension <name>.\n"
            "  --ri <name>      Show configuration for extension <name>.\n"
            "\n", prog, prog, prog, prog, prog, prog, prog);
#endif
}


static gboolean gui_execute_script(gpointer user_data) {
    zend_file_handle *primary_file = user_data;
    php_execute_script(primary_file);
    return FALSE;
}


static int do_gui(int argc, char **argv) /* {{{ */
{
    int c;
    zend_file_handle file_handle;
    int behavior = PHP_MODE_STANDARD;
    char *php_optarg = NULL, *orig_optarg = NULL;
    int php_optind = 1, orig_optind = 1;
    //char *exec_direct=NULL, *exec_run=NULL, *exec_begin=NULL, *exec_end=NULL;
    char *arg_free=NULL, **arg_excp=&arg_free;
    char *script_file=NULL, *translated_path = NULL;
    volatile int request_started = 0;
    volatile int exit_status = SUCCESS;
    int lineno = 0;
    const char *param_error=NULL;
    int hide_argv = 0;

    zend_try {

        // TODO: gtk_init(&argc, &argv);// consume gtk options

        while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
            switch (c) {
            case 'v': /* show php version & quit */
                php_printf("PHP %s (%s) (built: %s %s) ( %s)\n"
                           "Copyright (c) 1997-2018 The PHP Group\n"
                           "%s",
                       PHP_VERSION,
                       gui_sapi_module.name,
                       __DATE__,
                       __TIME__,
#if ZTS
                           "ZTS "
#else
                           "NTS "
#endif
#ifdef COMPILER
                           COMPILER
                           " "
#endif
#ifdef ARCHITECTURE
                           ARCHITECTURE
                           " "
#endif
#if ZEND_DEBUG
                           "DEBUG "
#endif
#ifdef HAVE_GCOV
                           "GCOV "
#endif
                       ,
                       get_zend_version());
                sapi_deactivate();

                goto out;
            default:
                break;
            }
        }

        php_optind = orig_optind;
        php_optarg = orig_optarg;
        while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2)) != -1) {
            switch (c) {
            case 'f': /* parse file */
                if ( behavior == PHP_MODE_PROCESS_STDIN) {
                    param_error = param_mode_conflict;
                    break;
                } else if (script_file) {
                    param_error = "You can use -f only once.\n";
                    break;
                }
                script_file = php_optarg;
                break;
            default:
                break;
            }
        }

        if (param_error) {
            PUTS(param_error);
            exit_status=FAILURE;
            goto err;
        }

        /* only set script_file if not set already and not in direct mode and not at end of parameter list */
        if (argc > php_optind
          && !script_file
          && behavior!=PHP_MODE_PROCESS_STDIN
          && strcmp(argv[php_optind-1],"--"))
        {
            script_file=argv[php_optind];
            php_optind++;
        }

        //@TODO: http://5312910.php.net/
        if (script_file) {
            if (gui_seek_file_begin(&file_handle, script_file, &lineno) != SUCCESS) {
                goto err;
            } else {
                char real_path[MAXPATHLEN];
                if (VCWD_REALPATH(script_file, real_path)) {
                    translated_path = strdup(real_path);
                }
                script_filename = script_file;
            }
        } else {
            /* We could handle PHP_MODE_PROCESS_STDIN in a different manner  */
            /* here but this would make things only more complicated. And it */
            /* is consitent with the way -R works where the stdin file handle*/
            /* is also accessible. */
            file_handle.filename = "Standard input code";
            file_handle.handle.fp = stdin;
        }
        file_handle.type = ZEND_HANDLE_FP;
        file_handle.opened_path = NULL;
        file_handle.free_filename = 0;
        php_self = (char*)file_handle.filename;

        /* before registering argv to module exchange the *new* argv[0] */
        /* we can achieve this without allocating more memory */
        SG(request_info).argc=argc-php_optind+1;
        arg_excp = argv+php_optind-1;
        arg_free = argv[php_optind-1];
        SG(request_info).path_translated = translated_path? translated_path: (char*)file_handle.filename;
        argv[php_optind-1] = (char*)file_handle.filename;
        SG(request_info).argv=argv+php_optind-1;

        if (php_request_startup()==FAILURE) {
            *arg_excp = arg_free;
            fclose(file_handle.handle.fp);
            PUTS("Could not startup.\n");
            goto err;
        }

        request_started = 1;
        CG(start_lineno) = lineno;
        *arg_excp = arg_free; /* reconstuct argv */

        if (hide_argv) {
            int i;
            for (i = 1; i < argc; i++) {
                memset(argv[i], 0, strlen(argv[i]));
            }
        }

        /*zend_bool ret = */zend_is_auto_global_str("_SERVER", 7);
        PG(during_request_startup) = 0;
        switch (behavior) {
        case PHP_MODE_STANDARD:
#if 0
            if (strcmp(file_handle.filename, "Standard input code")) {
                cli_register_file_handles();
            }

            if (interactive && cli_shell_callbacks.cli_shell_run) {
                exit_status = cli_shell_callbacks.cli_shell_run();
            } else {
                php_execute_script(&file_handle);
                exit_status = EG(exit_status);
            }
#else

            // Who to read from Read from STDOUT_LINEO
            //php_gui_load(translated_path);

#ifdef PHP_WRITE_STDOUT
            if ((ctxt.parser = php_gui_parser_create(translated_path)) == NULL) {
                printf("Erreur lors de la création du contexte\n");
                return EXIT_FAILURE;
            }

#else
            if ((ctxt.parser = php_gui_parser_create(stdout)) == NULL) {
                printf("Erreur lors de la création du contexte\n");
                return EXIT_FAILURE;
            }
#endif

            gtk_init(0, NULL);// executor_globals
            g_idle_add((GSourceFunc)gui_execute_script, (gpointer)&file_handle);
            gtk_main();
            // If php error, keep the window ? or close ?
            // TODO MODE_STDI : add idle to check if exit_status( executor_globals.v) FAILLURe and break the .
            exit_status = EG(exit_status);

            /*
            //doc = ctxt->myDoc;
            //res = ctxt->wellFormed;
            xmlFreeParserCtxt(ctxt);
            xmlFreeDoc(doc);
            */

#endif
            break;
        default:
            printf("Internal error: behavior unknow( %d)\n", behavior);
            break;
        }
/*
        printf("%s\n", script_file);
        printf("%s\n", script_filename);
        printf("%s\n", translated_path);
        printf("php_self: %s\n", php_self);
        printf("behavior: %d(%d)\n", behavior, PHP_MODE_STANDARD);
*/



    } zend_end_try();

out:
    if (request_started) {
        php_request_shutdown((void *) 0);
    }
    if (translated_path) {
        free(translated_path);
    }
    if (exit_status == 0) {
        exit_status = EG(exit_status);
    }
    return exit_status;
err:
    sapi_deactivate();
    zend_ini_deactivate();
    exit_status = FAILURE;
    goto out;

}

/**
  +----------------------------------------------------------------------+
  | Entry point                                                          |
  +----------------------------------------------------------------------+
*/

int main(int argc, char *argv[])
{
    int exit_status = SUCCESS;
    char *php_optarg = NULL;
    int php_optind = 1;
    char *ini_entries = NULL;
    size_t ini_entries_len = 0;
    int c;
    sapi_module_struct *sapi_module = &gui_sapi_module;

    zend_signal_startup();

#if 1
    while ((c = php_getopt(argc, argv, OPTIONS, &php_optarg, &php_optind, 0, 2))!=-1) {
        switch (c) {
            case 'h': /* help & quit */
            case '?':
                php_gui_usage(argv[0]);
                goto out;
                break;
        }
    }
#endif

    if (sapi_module == &gui_sapi_module) {
        if (ini_entries) {
            ini_entries = realloc(ini_entries, ini_entries_len + sizeof(HARDCODED_INI));
            memmove(ini_entries + sizeof(HARDCODED_INI) - 2, ini_entries, ini_entries_len + 1);
            memcpy(ini_entries, HARDCODED_INI, sizeof(HARDCODED_INI) - 2);
        } else {
            ini_entries = malloc(sizeof(HARDCODED_INI));
            memcpy(ini_entries, HARDCODED_INI, sizeof(HARDCODED_INI));
        }
        ini_entries_len += sizeof(HARDCODED_INI) - 2;
    }

    sapi_module->ini_entries = ini_entries;

    /* startup after we get the above ini override se we get things right */
    if (sapi_module->startup(sapi_module) == FAILURE) {
        /* there is no way to see if we must call zend_ini_deactivate()
         * since we cannot check if EG(ini_directives) has been initialised
         * because the executor's constructor does not set initialize it.
         * Apart from that there seems no need for zend_ini_deactivate() yet.
         * So we goto out_err.*/
        exit_status = FAILURE;
        goto out;
    }
    //module_started = 1;


    zend_first_try {
        exit_status = do_gui(argc, argv);
    } zend_end_try();
out:


    return exit_status;
}
