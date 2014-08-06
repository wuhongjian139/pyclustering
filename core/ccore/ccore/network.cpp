#include "network.h"

#include <cmath>
#include <exception>
#include <stdexcept>

network::network(const unsigned int number_oscillators, const conn_type connection_type) {
	num_osc = number_oscillators;

	osc_conn = new std::vector<std::vector<unsigned int> * >(number_oscillators, NULL);
	for (unsigned int index = 0; index < number_oscillators; index++) {
		(*osc_conn)[index] = new std::vector<unsigned int>(number_oscillators, 0);
	}

	create_structure(connection_type);
}

network::~network() {
	if (osc_conn != NULL) {
		for (std::vector<std::vector<unsigned int> *>::iterator iter = osc_conn->begin(); iter != osc_conn->end(); iter++) {
			delete (*iter); 
			(*iter) = NULL;
		}

		delete osc_conn;
		osc_conn = NULL;
	}
}

std::vector<unsigned int> * network::get_neighbors(const unsigned int index) const {
	std::vector<unsigned int> * result = new std::vector<unsigned int>();
	for (std::vector<unsigned int>::const_iterator iter = (*osc_conn)[index]->begin(); iter != (*osc_conn)[index]->end(); iter++) {
		if ((*iter) > 0) {
			result->push_back(*iter);
		}
	}

	return result;
}

void network::create_structure(const conn_type connection_structure) {
	switch(connection_structure) {
		case conn_type::NONE:
			create_none_connections();
			break;
		case conn_type::ALL_TO_ALL:
			create_all_to_all_connections();
			break;
		case conn_type::GRID_FOUR:
			create_grid_four_connections();
			break;
		case conn_type::GRID_EIGHT:
			create_grid_eight_connections();
			break;
		case conn_type::LIST_BIDIR:
			create_list_bidir_connections();
			break;
		default:
			throw std::runtime_error("Unknown type of connection");
	}
}

void network::create_none_connections() { return; }

void network::create_all_to_all_connections() {
	for (unsigned int row = 0; row < num_osc; row++) {
		for (unsigned int col = row + 1; col < num_osc; col++) {
			(*(*osc_conn)[row])[col] = 1;
			(*(*osc_conn)[col])[row] = 1;
		}
	}
}

void network::create_list_bidir_connections() {
	for (unsigned int index = 1; index < num_osc; index++) {
		if (index > 0) {
			(*(*osc_conn)[index])[index - 1] = 1;
		}

		if (index < (num_osc - 1)) {
			(*(*osc_conn)[index])[index + 1] = 1;
		}
	}
}

void network::create_grid_four_connections() {
	const double conv_side_size = std::sqrt((double) num_osc);
	if (conv_side_size - std::floor(conv_side_size) > 0) {
		throw std::runtime_error("Invalid number of oscillators in the network for the grid structure");
	}

	const unsigned int side_size = (unsigned int) conv_side_size;

	for (unsigned int index = 0; index < num_osc; index++) {
		const int upper_index = index - side_size;
		const int lower_index = index + side_size;
		const int left_index = index - 1;
		const int right_index = index + 1;

		unsigned int node_row_index = std::ceil(index / side_size);
		if (upper_index >= 0) {
			(*(*osc_conn)[index])[upper_index] = 1;
		}

		if (lower_index < num_osc) {
			(*(*osc_conn)[index])[lower_index] = 1;
		}

		if ( (left_index >= 0) && (std::ceil(left_index / side_size) == node_row_index) ) {
			(*(*osc_conn)[index])[left_index] = 1;
		}

		if ( (right_index < num_osc) && (std::ceil(right_index / side_size) == node_row_index) ) {
			(*(*osc_conn)[index])[right_index] = 1;
		}
	}
}

void network::create_grid_eight_connections() {
	create_grid_four_connections();	/* create connection with right, upper, left, lower neighbor */
	
	const unsigned int side_size = (unsigned int) std::sqrt((double) num_osc);

	for (unsigned int index = 0; index < num_osc; index++) {
        const unsigned int upper_index = index - side_size;
        const unsigned int upper_left_index = index - side_size - 1;
        const unsigned int upper_right_index = index - side_size + 1;
            
        const unsigned int lower_index = index + side_size;
        const unsigned int lower_left_index = index + side_size - 1;
        const unsigned int lower_right_index = index + side_size + 1;
            
        const unsigned int left_index = index - 1;
        const unsigned int right_index = index + 1;
            
        const unsigned int node_row_index = std::floor(index / side_size);
        const unsigned int upper_row_index = node_row_index - 1;
        const unsigned int lower_row_index = node_row_index + 1;

		if ( (upper_left_index >= 0) && (std::floor(upper_left_index / side_size) == upper_row_index) ) {
			(*(*osc_conn)[index])[upper_left_index] = 1;
		}

		if ( (upper_right_index >= 0) && (std::floor(upper_right_index / side_size) == upper_row_index) ) {
			(*(*osc_conn)[index])[upper_right_index] = 1;
		}

		if ( (lower_left_index < num_osc) && (std::floor(lower_left_index / side_size) == lower_row_index) ) {
			(*(*osc_conn)[index])[lower_left_index] = 1;
		}

		if ( (lower_right_index < num_osc) && (std::floor(lower_right_index / side_size) == lower_row_index) ) {
			(*(*osc_conn)[index])[lower_right_index] = 1;
		}
	}
}