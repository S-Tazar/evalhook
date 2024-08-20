/* evalhook extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_evalhook.h"
#include "evalhook_arginfo.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

static zend_op_array *(*orig_compile_string)(zend_string *source_string, const char *filename, zend_compile_position position);
static zend_bool evalhook_hooked = 0;

static zend_op_array *evalhook_compile_string(zend_string *source_string, const char *filename, zend_compile_position position)
{
	int c, len;
	char *copy;

	/* Ignore non string eval() */
	if (ZSTR_LEN(source_string) == 0) {
        return orig_compile_string(source_string, filename, position);
    }

	len = ZSTR_LEN(source_string);
    copy = estrndup(ZSTR_VAL(source_string), len);
	if (len > strlen(copy)) {
		for (c=0; c<len; c++) if (copy[c] == 0) copy[c] == '?';
	}

	php_printf("\n--------- start decoding ------------\n");
    php_printf("%s\n", copy);
    php_printf("--------- end decoding ------------\n");

	return orig_compile_string(source_string, filename, position);
}

PHP_MINIT_FUNCTION(evalhook)
{
	if (evalhook_hooked == 0) {
		evalhook_hooked = 1;
		orig_compile_string = zend_compile_string;
		zend_compile_string = evalhook_compile_string;
	}
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(evalhook)
{
	if (evalhook_hooked == 1) {
		evalhook_hooked = 0;
		zend_compile_string = orig_compile_string;
	}
	return SUCCESS;
}

PHP_MINFO_FUNCTION(evalhook)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "evalhook support", "enabled");
	php_info_print_table_row(2, "Author", "Stefan Esser, Sophia.T");
	php_info_print_table_end();
}

zend_module_entry evalhook_module_entry = {
	STANDARD_MODULE_HEADER,
	"evalhook",						/* Extension name */
	NULL,							/* zend_function_entry */
	PHP_MINIT(evalhook),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(evalhook),		/* PHP_MSHUTDOWN - Module shutdown */
	NULL,			                /* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(evalhook),			/* PHP_MINFO - Module info */
	"1.0.0",						/* Version */
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_EVALHOOK
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(evalhook)
#endif