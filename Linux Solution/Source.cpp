#include "customer.h"

//Objects are dynamic, so we only need these definitions for this class
#define CUSTOMERS 5
#define RESOURCES 4
#define SLEEP_SEED 2000 //Determines sleep time somewhat, higher values might yield longer sleep times

void * customer_request_resource(void * ptr) {
	int cust_id = (int)ptr;
	int finished_state;
	customer *cust = (customer *)ptr;
	
	while((finished_state = cust->is_finished()) != 0) {

		//If all the resources have been acquired then we just need to release them
		if(finished_state == 2) {
			cust->rand_sleep(SLEEP_SEED);
			cust->rand_release();
		}
		
		else {
			if(cust->rand_sleep(SLEEP_SEED)) {
				cust->rand_request();
			}

			else {
				cust->rand_release();
			}
		}

		cust->rand_sleep(SLEEP_SEED);
	}
}

int main() {
	//Setting up needs, resources, and data structures
	int customer_needs[CUSTOMERS][RESOURCES] = { {2, 4, 6, 1}, {4, 1, 1, 2}, {5, 1, 2, 3}, {1, 1, 1, 1}, {0, 1, 1, 4} };
	int available_resources[RESOURCES] = { 6, 9, 4, 13 };
	bank the_bank(available_resources, *customer_needs, CUSTOMERS, RESOURCES);
	pthread_t threads[CUSTOMERS];
	std::vector<customer> customer_list;
	srand(time(NULL));

	//Uses a vector instead of an array for preserving objects
	for (int i = 0; i < 5; i++) {
		customer x(i, customer_needs[i], 4, &the_bank);
		customer_list.push_back(x);
	}

	//Calls up the thread passing the object to it as a parameter
	for (int i = 0; i < 5; i++) {
		pthread_create(&threads[i], NULL, customer_request_resource, customer_list.data() + i);
		sleep(1); //Sleep a second to stagger random seed
	}

	for (int i = 0; i < 5; i++) 
		pthread_join(threads[i], NULL);

	printf("All customer needs have been met.\n");

	return 0;
}