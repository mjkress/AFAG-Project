#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <string.h>
#include <cstdio>
#include "adjacency.h"

// ACIS header files
#include "acis.hxx"
#include "cstrapi.hxx"
#include "api.hxx"
#include "api.err"
#include "lists.hxx"
#include "kernapi.hxx"

using namespace std;

// Declaration of the ACIS licensing function.
void unlock_spatial_products_XXXX();

// Declaration of our functions.
void get_face_adjacencies();
int my_initialization();
int my_termination();

// Define a macro to check the outcome of API functions.
// In the event of an error this macro will
// print an error message and propagate the error.
#define CHECK_RESULT                                     \
    if (!result.ok()) {                                  \
        err_mess_type err_no = result.error_number();    \
        printf("ACIS ERROR %d: %s\n",                    \
            err_no, find_err_mess(err_no));              \
        sys_error(err_no);                               \
	    }

int main(int argc, char** argv) {

	// Initialize 
	int ret_val = my_initialization();
	if (ret_val)
		return 1;

	get_face_adjacencies();

	ret_val = my_termination();
	if (ret_val)
		return 1;

	printf("Program completed successfully\n\n");

	return 0;
}

void get_face_adjacencies(){

	API_BEGIN

		// Open SAT file

		FILE * part_file = fopen("Proj5 - test_part.3.sat", "r");
		if (part_file == NULL) {
			printf("ERROR opening wire.sat\n");
			sys_error(API_FAILED);
		}

		// Restore entities from .SAT file
		ENTITY_LIST ents;
		result = api_restore_entity_list(part_file, TRUE, ents);
		if (!result.ok()) {
			err_mess_type err_no = result.error_number();
			printf("ERROR in api_restore_entity_list() %d: %s\n",
				err_no, find_err_mess(err_no));
			sys_error(err_no);
		}

		// Close the file
		fclose(part_file);

		// Make face adjacency graph
		adjacency_links(ents);

	API_END

		return;
}

int my_initialization() {

	// Start ACIS.
	outcome result = api_start_modeller(0);
	CHECK_RESULT2

		// Call the licensing function to unlock ACIS.
		unlock_spatial_products_XXXX();

	// Initialize all necessary components. 
	result = api_initialize_constructors();
	CHECK_RESULT2

		return 0;
}

int my_termination() {
	// Terminate all necessary components. 
	outcome result = api_terminate_constructors();
	CHECK_RESULT2

		// Stop ACIS and release any allocated memory.
		result = api_stop_modeller();
	CHECK_RESULT2

		return 0;
}

