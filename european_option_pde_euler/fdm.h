#pragma once

#include "pde.h"
#include <vector>

// Finite Difference Method - Abstract Base Class
class FDMBase
{
protected:
	ConvectionDiffusionPDE* pde;

	// Space discretisation
	double x_dom;		// Spatial extent [0.0, x_dom]
	unsigned long J;	// Number of spatial differencing points
	double dx;			// Spatial step size (calculated from above)
	std::vector<double> x_values;	// Stores the coordinates of the x dimension

	// Time discretisation
	double t_dom;		// Temporal extent [0.0, t_dom]
	unsigned long N;	// Number of temporal differencing points
	double dt;			// Temporal step size (calculated from above)
	
	// Time-marching
	double prev_t, cur_t;	// Previous and current times

	// Differencing coefficients
	double alpha, beta, gamma;

	// Storage
	std::vector<double> new_result;		// New solution (become N+1)
	std::vector<double> old_result;		// Old solution (become N)

	// Constructor
	FDMBase(double _x_dom, unsigned long _J,
			double _t_dom, unsigned long N,
			ConvectionDiffusionPDE* _pde);

	// Override these virtual methods in derived classes for
	// specific FDM techniques, such as explicit 
};

