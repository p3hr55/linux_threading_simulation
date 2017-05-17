#include "bank.h"

class customer {
private:
	int customer_id, resources;
	int *max_resources; //pointer used so objects array sizes are dynamic
	int *current_resources;
	bank * the_bank;

public:
	//Passes what thread it is, the needed resources, how many resources the bank has, and a pointer to the bank
	customer(int id, int * max_res, int num_resources, bank * glob_bank) {

		//srand(time(NULL) + customer_id);
		customer_id = id;
		resources = num_resources;
		the_bank = glob_bank;

		max_resources = new int[num_resources];
		current_resources = new int[num_resources];

		//Weird pointer arithmetic is used to get to the array indexs
		for (int i = 0; i < num_resources; i++) {
			max_resources[i] = *(max_res + i);
			current_resources[i] = 0;
		}
	}

	//Serves two puroses, sleeps and returns a 0 or 1 dictating which operation the customer should do next, request or release
	bool rand_sleep(int s) {
		int sleep_time = (rand() + 1) % (s + 1);
		//printf("Customer #%d is sleeping for %d seconds.\n", customer_id, sleep_time / 1000);
		sleep((sleep_time / 1000) + 1);

		return sleep_time % 2;
	}

	//Creates an array of random requests, submits to the bank, and does an action on success
	void rand_request() {
		static int* gen_request = new int[resources];
		printf("Customer #%d is requesting:\n", customer_id + 1);
		for (int i = 0; i < resources; i++) {
			gen_request[i] = (rand() % (max_resources[i] + 1));
			printf("\tresource[%d] = %d\n", i, gen_request[i]);
		}

		if (the_bank->request_resources(customer_id, gen_request) == true) {
			printf("Bank validated the transaction.\n\n");
			for (int i = 0; i < resources; i++) {
				max_resources[i] -= gen_request[i];
				current_resources[i] += gen_request[i];
			}
		}

		else
			printf("Bank declined the transaction.\n\n");
	}

	//Identicle to the random request except resources are being released so current resources array is subtracted
	//the-bank->release_resource always returns true, so the declined message is solely for longjevity of application
	void rand_release() {
		static int* gen_release = new int[resources];
		printf("Customer #%d is releasing:\n", customer_id + 1);
		for (int i = 0; i < resources; i++) {
			gen_release[i] = (rand() % (current_resources[i] + 1));
			printf("\tresource[%d] = %d\n", i, gen_release[i]);
		}

		if (the_bank->release_resources(customer_id, gen_release) == true) {
			printf("Bank released the resources.\n\n");
			for (int i = 0; i < resources; i++) {
				current_resources[i] -= gen_release[i];
			}
		}

		else
			printf("Bank declined the release.\n\n");
	}

	//3 options, either still needs resources, is done completely, or has resources to return
	int is_finished() {
		int finish_state = 0;
		for (int i = 0; i < resources; i++)
			if (max_resources[i] > 0)
				return 1;

		for (int i = 0; i < resources; i++)
			if (current_resources[i] > 0)
				return 2;
		
		return 0; //all done
	}

	int get_customer_id() {
		return customer_id;
	}

	void test() {
		printf("Currently occupying customer #%d\n", customer_id);
	}

};