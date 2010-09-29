#include <stdio.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>

#include "test_config_reader.h"

int main()
{
	CU_pSuite config_reader_suite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

	/* add a suite to the registry */
	config_reader_suite = CU_add_suite("Config reader",               test_config_reader_init_suite, test_config_reader_clean_suite);
	
	if (NULL == config_reader_suite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* add the tests to the suite */
	if ((NULL == CU_add_test(config_reader_suite, "parse configuration", test_config_reader_PARSE_CONFIG))) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	/*CU_automated_run_tests();
	CU_list_tests_to_file();*/
	
	
	CU_cleanup_registry();
	return CU_get_error();
}