 #pragma once
#include "pch.h"
#include "Controls/Control.h"
#include "Styles/Style.h"


namespace Evergreen
{
struct GlobalJsonData
{
	std::unordered_map<std::string, std::shared_ptr<Style>> m_stylesMap;
	std::unordered_map<std::string, std::shared_ptr<Control>> m_controlsMap; // <-- Not used yet, but could be implemented in the future
};

}