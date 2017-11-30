#pragma once

#include <exception>
#include <string>

namespace dof
{

	/**
	* @breif Exception thrown by Doforward classes. 
	*
	*/
	class Exception : public std::exception
	{

	public:

		/**
		* @breif Exception code, required by all throws done in Doforward classes.
		*		 Message is being stored in inherited std::exception class.
		*
		*/
		enum eCode
		{

		};

		/**
		* @breif Constructor.
		*
		* @param code Exception code
		* @message Message describing the exception.
		*
		* @see eCode
		*
		*/
		Exception(const eCode code, const std::string & message);

		/**
		* @breif Get description of exception.
		*
		* @return Exception message.
		*
		*/
		const std::string & GetMessage() const;

		/**
		* @breif Get exception code.
		*
		* @return Exception code.
		*
		*/
		eCode GetCode() const;

	private:

		eCode m_Code; //< Exception code.

	};

}