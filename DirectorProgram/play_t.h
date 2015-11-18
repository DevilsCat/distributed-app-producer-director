// play_t.h -- this files declare some type definition for this play program which makes data type more meaning.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao
//
#ifndef PLAY_T_H
#define PLAY_T_H
#include <string>

namespace play_t {
	typedef unsigned int					number_t;
	typedef std::string						name_t;
	typedef std::string						input_t;
	typedef std::string						content_t;
	typedef std::pair<name_t, content_t>	dialog_t;
	typedef std::pair<number_t, dialog_t>	line_t;
	typedef std::pair<name_t, input_t>		config_t;
	typedef std::pair<number_t, content_t>	num_cont_t;
}

#endif
