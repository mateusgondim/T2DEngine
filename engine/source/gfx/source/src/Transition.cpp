#include "Transition.hpp"
#include "Condition.hpp"
#include "Int_condition.hpp"
#include "Bool_condition.hpp"
#include <string>
#include <vector>
#include <memory>


void gfx::Transition::add_integer_condition(const std::string & param_name, const Condition::Cond_operator & op, const int icomp_value)
{
	m_upconditions.push_back(std::make_shared<Int_condition>(param_name, op, icomp_value));
}

void  gfx::Transition::add_boolean_condition(const std::string & param_name, const bool bcomp_value)
{
	m_upconditions.push_back(std::make_shared<Bool_condition>(param_name, bcomp_value));
}

void  gfx::Transition::add_trigger_condition(const std::string & param_name)
{
	add_boolean_condition(param_name, true);
}

std::ostream &  gfx::operator<<(std::ostream & os, const Transition & t)
{
	os << t.m_o_state_name << " -> " << t.m_d_state_name << std::endl;
	for (auto c : t.m_upconditions) {
		os << "\t if(" << c->get_variable_name();
		switch (c->get_operator()) {
		case Condition::Cond_operator::EQUALITY:
			os << " == ";
			break;
		case Condition::Cond_operator::INEQUALITY:
			os << " != ";
			break;
		case Condition::Cond_operator::GREATER_THAN:
			os << " > ";
			break;
		case Condition::Cond_operator::LESS_THAN:
			os << " < ";
			break;
		default:
			os << " invalid operator ";
			break;
		}
		const void * pvalue = c->get_comp_value();
		switch (c->get_type()) {
		case Condition::Type::BOOLEAN:
			os << *(static_cast<const bool *>(pvalue)) << ")" << std::endl;
			break;
		case Condition::Type::INTEGER:
			os << *(static_cast<const int *>(pvalue)) << ")" << std::endl;
			break;
		default:
			os << "Invalid type)" << std::endl;
			break;
		}
	}
	return os;
}