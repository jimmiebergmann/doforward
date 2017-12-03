/*
* MIT License
*
* Copyright(c) 2017 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

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
			InvalidType,
			InvalidPointer,
			CannotOpen,
			ValidationError
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