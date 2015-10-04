#ifndef _TRANSFERFUNCTIONS_H_
#define _TRANSFERFUNCTIONS_H_


#include "CommonIncludes.h"

class ITransferFunction
{
public:

	/// CONSTRUCTION / DESTRUCTION
	/// Destructor
	virtual ~ITransferFunction() {}

	/// PUBLIC OPERATIONS

	/// Returns the color for a value between 0 and 1
	virtual void	GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue) = 0;
};


class TransferFunctionGreenToRed : public ITransferFunction
{
public:

	/// PUBLIC OPERATIONS

	/// Returns the color for a value between 0 and 1
	virtual void	GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue);
};

class TransferFunctionCustom1 : public ITransferFunction
{
public:

	/// PUBLIC OPERATIONS

	/// Returns the color for a value between 0 and 1
	virtual void	GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue);
};

class TransferFunctionCustom2 : public ITransferFunction
{
public:

	/// PUBLIC OPERATIONS

	/// Returns the color for a value between 0 and 1
	virtual void	GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue);
};


#endif //_TRANSFERFUNCTIONS_H_