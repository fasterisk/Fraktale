#include "TransferFunctions.h"

/*********************************************************************************************
*********************************************************************************************/
void	TransferFunctionGreenToRed::GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue)
{
	assert(fValue >= 0.0f && fValue <= 1.0f);

	rfRed = fValue;
	rfGreen = 1.0f - fValue;
	rfBlue = 0.0f;
}

/*********************************************************************************************
*********************************************************************************************/
void	TransferFunctionCustom1::GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue)
{
	assert(fValue >= 0.0f && fValue <= 1.0f);

	if (fValue <= 0.2f)
	{
		//VIOLETT zu BLAU
		float fNewValue = (fValue - 0.0f) / 0.2f;
		rfRed = 0.5f - fNewValue / 2.0f;
		rfGreen = 0.0f;
		rfBlue = fNewValue;
	}
	else if (fValue <= 0.4f)
	{
		//BLAU zu GRÜN
		float fNewValue = (fValue - 0.2f) / 0.2f;
		rfRed = 0.0f;
		rfGreen = fNewValue;
		rfBlue = 1.0f - fNewValue;
	}
	else if (fValue <= 0.6f)
	{
		//GRÜN zu GELB
		float fNewValue = (fValue - 0.4f) / 0.2f;
		rfRed = fNewValue;
		rfGreen = 1.0f;
		rfBlue = 0.0f;

	}
	else if (fValue <= 0.8f)
	{
		//GELB zu ORANGE
		float fNewValue = (fValue - 0.6f) / 0.2f;
		rfRed = 1.0f;
		rfGreen = 1.0f - fNewValue / 2.0f;
		rfBlue = 0.0f;
	}
	else
	{
		//ORANGE zu ROT
		float fNewValue = (fValue - 0.8f) / 0.2f;
		rfRed = 1.0f;
		rfGreen = 0.5f - fNewValue / 2.0f;
		rfBlue = 0.0f;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	TransferFunctionCustom2::GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue)
{
	assert(fValue >= 0.0f && fValue <= 1.0f);

	if (fValue <= 1.0f/8.0f)
	{
		//BLAU - BLAUGRÜN
		float fNewValue = (fValue - 0.0f) / (1.0f / 8.0f);
		rfRed =0.0f;
		rfGreen = fNewValue / 2.0f;
		rfBlue = 1.0f;
	}
	else if (fValue <= 2.0f / 8.0f)
	{
		//BLAUGRÜN zu GRÜN
		float fNewValue = (fValue - 1.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 0.0f;
		rfGreen = 0.5f + fNewValue / 2.0f;
		rfBlue = 1.0f;
	}
	else if (fValue <= 3.0f / 8.0f)
	{
		//GRÜN zu GELBGRÜN
		float fNewValue = (fValue - 2.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = fNewValue / 2.0f;
		rfGreen = 1.0f;
		rfBlue = 1.0f - fNewValue;
	}
	else if (fValue <= 4.0f / 8.0f)
	{
		//GELBGRÜN zu GELB
		float fNewValue = (fValue - 3.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 0.5f + fNewValue / 2.0f;
		rfGreen = 1.0f;
		rfBlue = 0.0f;
	}
	else if (fValue <= 5.0f / 8.0f)
	{
		//GELB zu ORANGE1
		float fNewValue = (fValue - 4.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 1.0f;
		rfGreen = 1.0f - fNewValue / 4.0f;
		rfBlue = 0.0f;
	}
	else if (fValue <= 6.0f / 8.0f)
	{
		//ORANGE1 zu ORANGE2
		float fNewValue = (fValue - 5.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 1.0f;
		rfGreen = 0.75f - fNewValue / 4.0f;
		rfBlue = 0.0f;
	}
	else if (fValue <= 7.0f / 8.0f)
	{
		//ORANGE2 zu ORANGE3
		float fNewValue = (fValue - 6.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 1.0f;
		rfGreen = 0.5f - fNewValue / 4.0f;
		rfBlue = 0.0f - fNewValue;
	}
	else
	{
		//ORANGE3 zu ROT
		float fNewValue = (fValue - 7.0f / 8.0f) / (1.0f / 8.0f);
		rfRed = 1.0f;
		rfGreen = 0.25f - fNewValue / 4.0f;
		rfBlue = 0.0f;
	}
}

/*********************************************************************************************
*********************************************************************************************/
void	TransferFunctionHSVToRGB::GetColor(float fValue, float & rfRed, float & rfGreen, float & rfBlue)
{

	float H = fValue * 360.0f;
	float S = 1.0f;
	float V = 1.0f;

	assert(H >= 0.0f && H <= 360.0f);

	int iH = (int)(H / 60.0f);
	float f = H / 60.0f - iH;

	float p = V * (1.0f - S);
	float q = V * (1.0f - S * f);
	float t = V * (1.0f - S * (1.0f - f));

	switch (iH)
	{
	case 1:
		rfRed = q;
		rfGreen = V;
		rfBlue = p;
		break;
	case 2:
		rfRed = p;
		rfGreen = V;
		rfBlue = t;
		break;
	case 3:
		rfRed = p;
		rfGreen = q;
		rfBlue = V;
		break;
	case 4:
		rfRed = t;
		rfGreen = p;
		rfBlue = V;
		break;
	case 5:
		rfRed = V;
		rfGreen = p;
		rfBlue = q;
		break;
	default:
		rfRed = V;
		rfGreen = t;
		rfBlue = p;
		break;
	}
}
