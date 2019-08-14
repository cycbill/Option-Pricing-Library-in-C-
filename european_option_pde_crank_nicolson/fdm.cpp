#include <fstream>

#include "fdm.h"
#include "../tridiagonal_matrix_algorithm/tridiagonal_matrix_algorithm.h"

//#include "../tridiagonal_matrix_algorithm/tridiagonal_matrix_algorithm.cpp"

FDMCrankNicolson::FDMCrankNicolson(double _x_dom, unsigned long _J,
	double _t_dom, unsigned long _N,
	ConvectionDiffusionPDE* _pde)
	: FDMBase(_x_dom, _J, _t_dom, _N, _pde)
{
	calculate_step_size();
	set_initial_conditions();
}

void FDMCrankNicolson::calculate_step_size() {
	dx = x_dom / static_cast<double>(J - 1);
	dt = t_dom / static_cast<double>(N - 1);
}

void FDMCrankNicolson::set_initial_conditions() {
	// Spatial settings
	double cur_spot = 0.0;

	old_result.resize(J, 0.0);
	new_result.resize(J, 0.0);
	x_values.resize(J, 0.0);

	for (unsigned long j = 0; j < J; j++) {
		cur_spot = static_cast<double>(j) * dx;
		old_result[j] = pde->init_cond(cur_spot);
		x_values[j] = cur_spot;
	}

	// Temporal settings
	prev_t = 0.0;
	cur_t = 0.0;
}

void FDMCrankNicolson::calculate_boundary_conditions() {
	new_result[0] = pde->boundary_left(prev_t, x_values[0]);
	new_result[J - 1] = pde->boundary_right(prev_t, x_values[J - 1]);
}

void FDMCrankNicolson::calculate_inner_domain() {

	std::vector<double> A(J-2, 0.0), B(J-2, 0.0), C(J-2, 0.0), D(J-2, 0.0), X(J-2, 0.0);

	// Only use inner result indices (1 to J-2)
	for (unsigned long j = 1; j < J - 1; j++) {
		// Temporary variables and throughout
		double dt_sig = 0.5 * dt * (pde->diff_coeff(prev_t, x_values[j]));
		double dt_sig_2 = dt * dx * 0.25 * (pde->conv_coeff(prev_t, x_values[j]));

		// Differencing coefficients (see \alpha, \beta and \gamma in text)
		alpha = dt_sig - dt_sig_2;
		beta = - (2.0 * dt_sig) + (dt * dx * dx * (pde->zero_coeff(prev_t, x_values[j])));
		gamma = dt_sig + dt_sig_2;

		A[j-1] = - alpha;
		B[j-1] = dx * dx - beta;
		C[j-1] = - gamma;

		D[j-1] = (alpha * old_result[j - 1] +
			(dx * dx + beta) * old_result[j] +
			gamma * old_result[j + 1]) / (dx*dx);
	}

	tridiag_mtx_algo(J - 2, A, B, C, D, X);

	for (unsigned long j = 1; j < J - 1; j++) {
		new_result[j] = X[j - 1];
	}
}

void FDMCrankNicolson::step_march() {
	std::ofstream fdm_out("fdm.csv");

	while (cur_t < t_dom) {
		cur_t = prev_t + dt;
		calculate_boundary_conditions();
		calculate_inner_domain();
		for (unsigned long j = 0; j < J; j++) {
			fdm_out << x_values[j] << " " << prev_t << " " << new_result[j] << std::endl;
		}
		old_result = new_result;
		prev_t = cur_t;
	}

	fdm_out.close();
}