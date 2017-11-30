#include <Exception.hpp>

namespace dof
{

	Exception::Exception(const eCode code, const std::string & message) :
		std::exception(message.c_str()),
		m_Code(code)
	{
	}

	const std::string & Exception::GetMessage() const
	{
		return what();
	}

	Exception::eCode Exception::GetCode() const
	{
		return m_Code;
	}

}