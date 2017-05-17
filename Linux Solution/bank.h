#include "includes.h"
class bank {
private:
	pthread_mutex_t lock;
	int *available, *maximum, *allocation, *need;
	int customers, resources;
	bool *finished;

public:
	bank(int total_resources[], int * cust_needs, int custs, int res) {
		available = new int[res];
		maximum = new int[res * custs];
		allocation = new int[res * custs];
		need = new int[res * custs];
		finished = new bool[custs];

		customers = custs;
		resources = res;

		for (int i = 0; i < res; i++) {
			for (int j = 0; j < custs; j++) {
				allocation[i * custs + j] = 0;
				maximum[i * custs + j] = need[i * custs + j] = cust_needs[i * custs + j];
			}
		}

		for (int i = 0; i < res; i++) {
			available[i] = total_resources[i];
		}

		for (int i = 0; i < custs; i++)
			finished[i] = false;
	}

	bool is_safe(int cust_id, int * res_req) {
		bool *can_finish = new bool[customers];
		for (int i = 0; i < customers; i++)
			can_finish[i] = 0;

		if (finished[cust_id] == false) {
			for (int i = 0; i < resources; i++)
				if (*(res_req + i) > available[i])
					return false;

			for (int i = 0; i < resources; i++) {
				available[i] -= *(res_req + i);
				need[cust_id * customers + i] -= *(res_req + i);
				allocation[cust_id * customers + i] += *(res_req + i);
			}

			for (int i = 0; i < customers; i++) {
				for (int j = 0; j < customers; j++) {
					if (!can_finish[j]) {
						bool check = true;

						for (int k = 0; k < resources; k++) {
							if (need[i * customers + j] > available[k])
								check = false;
						}

						if (check == true)
							can_finish[j] = true;
					}
				}
			}


			if (can_finish[cust_id] == false) {
				printf("Unsafe state detected.\n");
				for (int i = 0; i < resources; i++) {
					available[i] += res_req[i];
					need[cust_id * customers + i] += *(res_req + i);
					allocation[cust_id * customers + i] -= *(res_req + i);
				}
			}
		}

		finished[cust_id] = 1;

		for (int i = 0; i < resources; i++) {
			if (need[cust_id * customers + i] > 0) {
				finished[cust_id] = 0;
				break;
			}
		}

		return true;
	}
	
	bool request_resources(int cust_id, int res_request[]) {
		pthread_mutex_lock(&lock);
		if (is_safe(cust_id, res_request)) {
			pthread_mutex_unlock(&lock);
			return true;
		}

		pthread_mutex_unlock(&lock);
		return false;
	}

	bool release_resources(int cust_id, int res_release[]) {
		pthread_mutex_lock(&lock);
		for (int i = 0; i < resources; i++) {
			available[i] += *(res_release + i);
			allocation[cust_id * customers + i] -= *(res_release + i);
		}

		pthread_mutex_unlock(&lock);
		return true;
	}

	int get_num_customers() {
		return customers;
	}

	int get_num_resources() {
		return resources;
	}

};