#include "xre_unit.h"
#include "xre_stringf.h"
#include <string.h>
#include <stdio.h>

// int main(void)
// {
// 	char *src1;
// 	char dst1[10];
// 	int ret1;

// 	char *src2;
// 	char dst2[10];
// 	int ret2;

// 	src1 = "coucou";
// 	src2 = "coucou";

// 	printf ("\ntest 1:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1 + 1, src1, 0);
// 	ret2 = slcpyf(dst2 + 1, 0, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %c : %c\n", *dst1, *dst2);
	
// 	printf ("\ntest 2:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1, src1, 1);
// 	ret2 = slcpyf(dst2, 1, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %x %c : %x %c\n", dst1[0], dst1[1], dst2[0], dst2[1]);

// 	printf ("\ntest 3:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1, src1, 2);
// 	ret2 = slcpyf(dst2, 2, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %c %x %c: %c %x %c\n", dst1[0], dst1[1], dst1[2], dst2[0], dst2[1], dst2[2]);

// 	printf ("\ntest 4:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1, src1, 6);
// 	ret2 = slcpyf(dst2, 6, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %d : %d\n", memcmp(src1, dst1, 5), memcmp(src2, dst2, 5));
// 	printf (" - %x %c : %x %c\n", dst1[5], dst1[1], dst2[5], dst2[1]);

// 	printf ("\ntest 5:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1, src1, 7);
// 	ret2 = slcpyf(dst2, 7, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %d : %d\n", memcmp(src1, dst1, 7), memcmp(src2, dst2, 7));

// 	printf ("\ntest 6:\n");
// 	memset(dst1, 'A', 10);
// 	memset(dst2, 'A', 10);
// 	ret1 = strlcpy(dst1, src1, 8);
// 	ret2 = slcpyf(dst2, 8, src2);
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %d : %d\n", memcmp(src1, dst1, 7), memcmp(src2, dst2, 7));

// 	printf ("\ntest 7:\n");
// 	memset(dst1, 0, 10);
// 	memset(dst2, 0, 10);
// 	ret1 = strlcpy(dst1, "1", 0);
// 	ret2 = slcpyf(dst2, 0, "1");
// 	printf (" - %d : %d\n", ret1, ret2);
// 	printf (" - %x : %x\n", *dst1, *dst2);
// 	return (0);
// }

// [
// 	[
// 		'"%10d", INT_MAX',   
// 		"2147483647(10)"
// 	], 
// 	[
// 		'"%12d", INT_MIN',    
// 		" -2147483648(12)"
// 	], 
// 	[
// 		'"%6d", 0', 
// 		"     0(6)"
// 	],   
// 	[
// 		'"%d", -0', 
// 		"0(1)"
// 	],    
// 	[
// 		'"%x", INT_MAX',
// 		"7fffffff(8)"
// 	], 
// 	[
// 		'"%x", INT_MIN', 
// 		"80000000(8)"
// 	],  
// 	[
// 		'"%2x", 0', 
// 		" 0(2)"
// 	],   
// 	[
// 		'"%x", -0', 
// 		"0(1)",
// 	],   
// 	[
// 		'"%x", 1', 
// 		"1(1)"
// 	],
// 	[
// 		'"%3x", -1',       
// 		"ffffffff(8)"
// 	],
// 	[
// 		'"%3s", ""', 
// 		"   (3)"
// 	],  
// 	[
// 		'"%1s ", "-"', 
// 		"- (2)"
// 	],   
// 	[
// 		'"%s %s", "", "-"', 
// 		 " -(2)"
// 	],       
// 	[
// 		'"%t %s %4  s%s% %s", "1", "2", "3"', 
// 		"%t 1 %4  s2% 3(14)"
// 	]              
// ]

static bool __test_001__(void) {

	char * src1 = "coucou";
	char * src2 = "coucou";
	char dst1[10];
	char dst2[10];
	int ret1;
	int ret2;

	memset(dst1, 'A', 10);
	memset(dst2, 'A', 10);

	ret1 = slcpyf(dst2 + 1, 0, src2);
	ret2 = strlcpy(dst1 + 1, src1, 0);
	ASSERT_NUM_EQUAL(ret1, ret2,"%d");

	ASSERT_NUM_EQUAL((char)*dst2, (char)*dst1, "%c");
	return (true);
}

static bool __test_002__(void) {

	char * src1 = "coucou";
	char * src2 = "coucou";
	char dst1[10];
	char dst2[10];
	int ret1;
	int ret2;

	memset(dst1, 'A', 10);
	memset(dst2, 'A', 10);

	ret1 = slcpyf(dst2 + 1, 0, src2);
	ret2 = strlcpy(dst1 + 1, src1, 0);
	ASSERT_NUM_EQUAL(ret1, ret2,"%d");

	ASSERT_NUM_EQUAL((char)*dst2, (char)*dst1, "%c");
	return (true);
}

TEST_FUNCTION void xre_slcpyf_specs(void) {
	__test_start__;

	run_test(&__test_001__, "Creating array only insert");


	__test_end__;
}