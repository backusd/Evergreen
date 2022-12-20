#include "pch.h"
#include "ColorHelper.h"

namespace Evergreen
{
	D2D1_COLOR_F ColorFromString(const std::string& colorName)
	{
		static const std::unordered_map<std::string, const D2D1::ColorF::Enum> colorNameMap = {
			{ "AliceBlue", D2D1::ColorF::AliceBlue },
			{ "AntiqueWhite", D2D1::ColorF::AntiqueWhite },
			{ "Aqua", D2D1::ColorF::Aqua },
			{ "Aquamarine", D2D1::ColorF::Aquamarine },
			{ "Azure", D2D1::ColorF::Azure },
			{ "Beige", D2D1::ColorF::Beige },
			{ "Bisque", D2D1::ColorF::Bisque },
			{ "Black", D2D1::ColorF::Black },
			{ "BlanchedAlmond", D2D1::ColorF::BlanchedAlmond },
			{ "Blue", D2D1::ColorF::Blue },
			{ "BlueViolet", D2D1::ColorF::BlueViolet },
			{ "Brown", D2D1::ColorF::Brown },
			{ "BurlyWood", D2D1::ColorF::BurlyWood },
			{ "CadetBlue", D2D1::ColorF::CadetBlue },
			{ "Chartreuse", D2D1::ColorF::Chartreuse },
			{ "Chocolate", D2D1::ColorF::Chocolate },
			{ "Coral", D2D1::ColorF::Coral },
			{ "CornflowerBlue", D2D1::ColorF::CornflowerBlue },
			{ "Cornsilk", D2D1::ColorF::Cornsilk },
			{ "Crimson", D2D1::ColorF::Crimson },
			{ "Cyan", D2D1::ColorF::Cyan },
			{ "DarkBlue", D2D1::ColorF::DarkBlue },
			{ "DarkCyan", D2D1::ColorF::DarkCyan },
			{ "DarkGoldenrod", D2D1::ColorF::DarkGoldenrod },
			{ "DarkGray", D2D1::ColorF::DarkGray },
			{ "DarkGreen", D2D1::ColorF::DarkGreen },
			{ "DarkKhaki", D2D1::ColorF::DarkKhaki },
			{ "DarkMagenta", D2D1::ColorF::DarkMagenta },
			{ "DarkOliveGreen", D2D1::ColorF::DarkOliveGreen },
			{ "DarkOrange", D2D1::ColorF::DarkOrange },
			{ "DarkOrchid", D2D1::ColorF::DarkOrchid },
			{ "DarkRed", D2D1::ColorF::DarkRed },
			{ "DarkSalmon", D2D1::ColorF::DarkSalmon },
			{ "DarkSeaGreen", D2D1::ColorF::DarkSeaGreen },
			{ "DarkSlateBlue", D2D1::ColorF::DarkSlateBlue },
			{ "DarkSlateGray", D2D1::ColorF::DarkSlateGray },
			{ "DarkTurquoise", D2D1::ColorF::DarkTurquoise },
			{ "DarkViolet", D2D1::ColorF::DarkViolet },
			{ "DeepPink", D2D1::ColorF::DeepPink },
			{ "DeepSkyBlue", D2D1::ColorF::DeepSkyBlue },
			{ "DimGray", D2D1::ColorF::DimGray },
			{ "DodgerBlue", D2D1::ColorF::DodgerBlue },
			{ "Firebrick", D2D1::ColorF::Firebrick },
			{ "FloralWhite", D2D1::ColorF::FloralWhite },
			{ "ForestGreen", D2D1::ColorF::ForestGreen },
			{ "Fuchsia", D2D1::ColorF::Fuchsia },
			{ "Gainsboro", D2D1::ColorF::Gainsboro },
			{ "GhostWhite", D2D1::ColorF::GhostWhite },
			{ "Gold", D2D1::ColorF::Gold },
			{ "Goldenrod", D2D1::ColorF::Goldenrod },
			{ "Gray", D2D1::ColorF::Gray },
			{ "Green", D2D1::ColorF::Green },
			{ "GreenYellow", D2D1::ColorF::GreenYellow },
			{ "Honeydew", D2D1::ColorF::Honeydew },
			{ "HotPink", D2D1::ColorF::HotPink },
			{ "IndianRed", D2D1::ColorF::IndianRed },
			{ "Indigo", D2D1::ColorF::Indigo },
			{ "Ivory", D2D1::ColorF::Ivory },
			{ "Khaki", D2D1::ColorF::Khaki },
			{ "Lavender", D2D1::ColorF::Lavender },
			{ "LavenderBlush", D2D1::ColorF::LavenderBlush },
			{ "LawnGreen", D2D1::ColorF::LawnGreen },
			{ "LemonChiffon", D2D1::ColorF::LemonChiffon },
			{ "LightBlue", D2D1::ColorF::LightBlue },
			{ "LightCoral", D2D1::ColorF::LightCoral },
			{ "LightCyan", D2D1::ColorF::LightCyan },
			{ "LightGoldenrodYellow", D2D1::ColorF::LightGoldenrodYellow },
			{ "LightGray", D2D1::ColorF::LightGray },
			{ "LightGreen", D2D1::ColorF::LightGreen },
			{ "LightPink", D2D1::ColorF::LightPink },
			{ "LightSalmon", D2D1::ColorF::LightSalmon },
			{ "LightSeaGreen", D2D1::ColorF::LightSeaGreen },
			{ "LightSkyBlue", D2D1::ColorF::LightSkyBlue },
			{ "LightSlateGray", D2D1::ColorF::LightSlateGray },
			{ "LightSteelBlue", D2D1::ColorF::LightSteelBlue },
			{ "LightYellow", D2D1::ColorF::LightYellow },
			{ "Lime", D2D1::ColorF::Lime },
			{ "LimeGreen", D2D1::ColorF::LimeGreen },
			{ "Linen", D2D1::ColorF::Linen },
			{ "Magenta", D2D1::ColorF::Magenta },
			{ "Maroon", D2D1::ColorF::Maroon },
			{ "MediumAquamarine", D2D1::ColorF::MediumAquamarine },
			{ "MediumBlue", D2D1::ColorF::MediumBlue },
			{ "MediumOrchid", D2D1::ColorF::MediumOrchid },
			{ "MediumPurple", D2D1::ColorF::MediumPurple },
			{ "MediumSeaGreen", D2D1::ColorF::MediumSeaGreen },
			{ "MediumSlateBlue", D2D1::ColorF::MediumSlateBlue },
			{ "MediumSpringGreen", D2D1::ColorF::MediumSpringGreen },
			{ "MediumTurquoise", D2D1::ColorF::MediumTurquoise },
			{ "MediumVioletRed", D2D1::ColorF::MediumVioletRed },
			{ "MidnightBlue", D2D1::ColorF::MidnightBlue },
			{ "MintCream", D2D1::ColorF::MintCream },
			{ "MistyRose", D2D1::ColorF::MistyRose },
			{ "Moccasin", D2D1::ColorF::Moccasin },
			{ "NavajoWhite", D2D1::ColorF::NavajoWhite },
			{ "Navy", D2D1::ColorF::Navy },
			{ "OldLace", D2D1::ColorF::OldLace },
			{ "Olive", D2D1::ColorF::Olive },
			{ "OliveDrab", D2D1::ColorF::OliveDrab },
			{ "Orange", D2D1::ColorF::Orange },
			{ "OrangeRed", D2D1::ColorF::OrangeRed },
			{ "Orchid", D2D1::ColorF::Orchid },
			{ "PaleGoldenrod", D2D1::ColorF::PaleGoldenrod },
			{ "PaleGreen", D2D1::ColorF::PaleGreen },
			{ "PaleTurquoise", D2D1::ColorF::PaleTurquoise },
			{ "PaleVioletRed", D2D1::ColorF::PaleVioletRed },
			{ "PapayaWhip", D2D1::ColorF::PapayaWhip },
			{ "PeachPuff", D2D1::ColorF::PeachPuff },
			{ "Peru", D2D1::ColorF::Peru },
			{ "Pink", D2D1::ColorF::Pink },
			{ "Plum", D2D1::ColorF::Plum },
			{ "PowderBlue", D2D1::ColorF::PowderBlue },
			{ "Purple", D2D1::ColorF::Purple },
			{ "Red", D2D1::ColorF::Red },
			{ "RosyBrown", D2D1::ColorF::RosyBrown },
			{ "RoyalBlue", D2D1::ColorF::RoyalBlue },
			{ "SaddleBrown", D2D1::ColorF::SaddleBrown },
			{ "Salmon", D2D1::ColorF::Salmon },
			{ "SandyBrown", D2D1::ColorF::SandyBrown },
			{ "SeaGreen", D2D1::ColorF::SeaGreen },
			{ "SeaShell", D2D1::ColorF::SeaShell },
			{ "Sienna", D2D1::ColorF::Sienna },
			{ "Silver", D2D1::ColorF::Silver },
			{ "SkyBlue", D2D1::ColorF::SkyBlue },
			{ "SlateBlue", D2D1::ColorF::SlateBlue },
			{ "SlateGray", D2D1::ColorF::SlateGray },
			{ "Snow", D2D1::ColorF::Snow },
			{ "SpringGreen", D2D1::ColorF::SpringGreen },
			{ "SteelBlue", D2D1::ColorF::SteelBlue },
			{ "Tan", D2D1::ColorF::Tan },
			{ "Teal", D2D1::ColorF::Teal },
			{ "Thistle", D2D1::ColorF::Thistle },
			{ "Tomato", D2D1::ColorF::Tomato },
			{ "Turquoise", D2D1::ColorF::Turquoise },
			{ "Violet", D2D1::ColorF::Violet },
			{ "Wheat", D2D1::ColorF::Wheat },
			{ "White", D2D1::ColorF::White },
			{ "WhiteSmoke", D2D1::ColorF::WhiteSmoke },
			{ "Yellow", D2D1::ColorF::Yellow },
			{ "YellowGreen", D2D1::ColorF::YellowGreen }
		};

		if (colorNameMap.find(colorName) != colorNameMap.end())
			return D2D1::ColorF(colorNameMap.at(colorName), 1.0f);

		throw ColorException(__LINE__, __FILE__, std::format("Invalid color string: {}", colorName));
	}




ColorException::ColorException(int line, const char* file, const std::string& message) noexcept :
	BaseException(line, file),
	m_message(message)
{
}
const char* ColorException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
	return m_whatBuffer.c_str();
}
const char* ColorException::GetType() const noexcept
{
	return "Color Exception";
}
std::string ColorException::GetErrorInfo() const noexcept
{
	return m_message;
}
}