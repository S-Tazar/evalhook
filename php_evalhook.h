/* evalhook extension for PHP */

#ifndef PHP_EVALHOOK_H
# define PHP_EVALHOOK_H

extern zend_module_entry evalhook_module_entry;
# define phpext_evalhook_ptr &evalhook_module_entry

# define PHP_EVALHOOK_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_EVALHOOK)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_EVALHOOK_H */