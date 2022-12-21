#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"
#include "Evergreen/UI/Brushes/Brushes.h"

/*
namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Button : public Control
{
public:
	Button(std::shared_ptr<DeviceResources> deviceResources) noexcept;


private:


	... Look into FillRoundedRectangle ...
};
#pragma warning( pop )


}
*/