/// @file
///	@ingroup 	minexamples
///	@copyright	Copyright 2018 The Min-DevKit Authors. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <iostream>
#include <fstream>

using namespace c74::min;
using namespace c74::max;

t_jit_err min_jit_testgen_matrix_calc(t_object* x, t_object* inputs, t_object* outputs);
void min_jit_testgen_outputmatrix(max_jit_wrapper *x);

class min_jit_testgen : public object<min_jit_testgen>, public matrix_operator<> {
public:
	MIN_DESCRIPTION	{"Min test of Jitter matrix generator. Object reads a file of vertex data and outputs it as a matrix."};
	MIN_TAGS		{"Jitter"};
	MIN_AUTHOR		{"Rob Ramirez"};
	MIN_RELATED		{"print, jit.print, dict.print"};

	inlet<>  input	{ this, "(bang) read file and output as matrix" };
	outlet<> output	{ this, "(matrix) Output", "matrix" };

	// dummy attr
	attribute<symbol> greeting { this, "greeting", "hello world",
		description {
			"Greeting to be posted. "
			"The greeting will be posted to the Max console when a bang is received."
		}
	};

	// unused
	template<typename matrix_type>
	matrix_type calc_cell(matrix_type input, const matrix_info& info, matrix_coord& position) {
		matrix_type output;
		return output;
	}

	t_jit_err matrix_calc(t_object* out_matrix) {
		t_jit_matrix_info out_minfo;
		void *p;

		// we must use object_method_direct for any method on the out_matrix due to bug in max-api WRT mop decorator methods

		//object_method(out_matrix, _jit_sym_getinfo, &out_minfo);
		object_method_direct(t_jit_err, (t_jit_object *, t_jit_matrix_info *), out_matrix, _jit_sym_getinfo, &out_minfo);

		//object_method(out_matrix, _jit_sym_getdata, &p);
		object_method_direct(t_jit_err, (t_jit_object *, void **), out_matrix, _jit_sym_getdata, &p);

		//auto out_savelock = object_method(out_matrix, _jit_sym_lock, (void*)1);
		auto out_savelock = object_method_direct(t_atom_long, (t_jit_object *, long), out_matrix, _jit_sym_lock, 1);

		// fill it up
		string torusline;

		// path to this external
		path extern_file("min.jit.testgen", path::filetype::external);
		std::string torus_fullpath = extern_file;
		// chop
		torus_fullpath.erase(torus_fullpath.rfind('/'));
		torus_fullpath.erase(torus_fullpath.rfind('/'));
		// path to data file
		torus_fullpath += "/source/projects/min.jit.testgen/torus.txt";
		std::ifstream torusdata(torus_fullpath);

		// read the file and shove it in the matrix
		float *fop = nullptr;
		for (auto j = 0; j < out_minfo.dim[1]; ++j) {
			for (auto i = 0; i < out_minfo.dim[0]; ++i) {
				fop = (float *)((char*)p + (j * out_minfo.dimstride[1] + i * out_minfo.dimstride[0]));

				if(getline (torusdata, torusline)) {
					std::istringstream iss(torusline);
					std::vector<std::string> torusvals(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
					for (auto k = 0; k < out_minfo.planecount && k < torusvals.size(); ++k) {
						fop[k] = stof(torusvals[k]);
					}
				}
				else {
					// not enough data
				}
			}
		}

		//object_method(out_matrix, _jit_sym_lock, out_savelock);
		object_method_direct(t_atom_long, (t_jit_object *, long), out_matrix, _jit_sym_lock, out_savelock);
		return JIT_ERR_NONE;
	}

private:
	// override maxob_setup if we want to set defaults
	/*message<> maxob_setup {this, "maxob_setup", MIN_FUNCTION {
		// get the max object wrapper
		c74::max::t_object* mob = NULL;
		c74::max::object_obex_lookup(maxobj(), symbol("maxwrapper"), &mob);
		t_atom_long dim[] = {20, 20};

		// set our output matrix properties
		c74::max::object_attr_setlong_array(mob, _jit_sym_dim, 2, dim);
		c74::max::object_attr_setlong(mob, _jit_sym_planecount, 8);
		c74::max::object_attr_setsym(mob, _jit_sym_type, _jit_sym_float32);

		return {};
	}};*/

	// override jitclass_setup so we can have our own matrix_calc
	message<> jitclass_setup {this, "jitclass_setup", MIN_FUNCTION {
		t_class* c = args[0];
		// add mop
		auto mop = jit_object_new(_jit_sym_jit_mop, -1, 1);
		jit_class_addadornment(c, mop);

		// add our custom matrix_calc method
		jit_class_addmethod(c, (method)min_jit_testgen_matrix_calc, "matrix_calc", A_CANT, 0);

		return {};
	}};

	// override maxclass_setup so we can have our own outputmatrix
	message<> maxclass_setup {this, "maxclass_setup", MIN_FUNCTION {
		t_class* c = args[0];
		long flags = MAX_JIT_MOP_FLAGS_OWN_OUTPUTMATRIX | MAX_JIT_MOP_FLAGS_OWN_JIT_MATRIX;
		max_jit_class_mop_wrap(c, this_jit_class, flags);
		max_jit_class_wrap_standard(c, this_jit_class, 0);
		class_addmethod(c, (method)max_jit_mop_assist, "assist", A_CANT, 0);
		max_jit_class_addmethod_usurp_low(c, (method)min_jit_testgen_outputmatrix, "outputmatrix");
		return {};
	}};
};

MIN_EXTERNAL(min_jit_testgen);

// add this as a static C function which allows calling from JS
t_jit_err min_jit_testgen_matrix_calc(t_object* x, t_object* inputs, t_object* outputs) {
	if (!x || !inputs || !outputs)
		return JIT_ERR_INVALID_PTR;

	t_jit_err err = JIT_ERR_NONE;
	auto out_matrix = (t_object*)object_method(outputs, _jit_sym_getindex, 0);
	//auto in_matrix = (t_object*)object_method(inputs, _jit_sym_getindex, 0);
	if (/*in_matrix && */out_matrix) {
		minwrap<min_jit_testgen>* job = (minwrap<min_jit_testgen>*)(x);
		err = job->m_min_object.matrix_calc(out_matrix);
	}
	else {
		return JIT_ERR_INVALID_PTR;
	}
	return err;
}


void min_jit_testgen_outputmatrix(max_jit_wrapper *x) {
	long outputmode = max_jit_mop_getoutputmode(x);
	t_object *mop = (t_object*)max_jit_obex_adornment_get(x, _jit_sym_jit_mop);
	t_jit_err err;

	if (outputmode && mop) { //always output unless output mode is none
		if (outputmode==1) {
			err = (t_jit_err)object_method(
				(t_object*)max_jit_obex_jitob_get(x),
				_jit_sym_matrix_calc,
				object_method(mop, _jit_sym_getinputlist),
				object_method(mop,_jit_sym_getoutputlist)
			);
			if(err) {
				jit_error_code(x,err);
			}
			else {
				max_jit_mop_outputmatrix(x);
			}
		}
		else {
			max_jit_mop_outputmatrix(x);
		}
	}
}
