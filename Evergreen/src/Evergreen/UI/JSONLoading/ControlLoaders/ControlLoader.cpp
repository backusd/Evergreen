#include "pch.h"
#include "ControlLoader.h"


namespace Evergreen
{
RowColumnPosition ControlLoader::ParseRowColumnPosition(const json& data)
{
	RowColumnPosition position;
	position.Row = 0;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	if (data.contains("Row"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Row"].is_number_unsigned(), "'Row' value must be an unsigned int.\nInvalid value: {}", data["Row"].dump(4));
		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["Column"].is_number_unsigned(), "'Column' value must be an unsigned int.\nInvalid value: {}", data["Column"].dump(4));
		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["RowSpan"].is_number_unsigned(), "'RowSpan' value must be an unsigned int.\nInvalid value : {}", data["RowSpan"].dump(4));

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		JSON_LOADER_EXCEPTION_IF_FALSE(data["ColumnSpan"].is_number_unsigned(), "'ColumnSpan' value must be an unsigned int.\nInvalid value : {}", data["ColumnSpan"].dump(4));

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}

	return position;
}

Margin ControlLoader::ParseMargin(const json& data)
{
	Margin margin{ 0 };

	if (data.contains("Margin"))
	{
		const json& marginData = data["Margin"];

		JSON_LOADER_EXCEPTION_IF_FALSE(marginData.is_array(), "Control with name: '{}'. 'Margin' value must be an array. Invalid data: {}", m_name, data.dump(4));

		for (auto& marginValue : marginData)
		{
			JSON_LOADER_EXCEPTION_IF_FALSE(marginValue.is_number(), "Control with name: '{}'. 'Margin' array values must be numbers. Invalid data: {}", m_name, data.dump(4));
		}

		if (marginData.size() == 1)
		{
			margin.Bottom = marginData[0].get<float>();
			margin.Left = margin.Bottom;
			margin.Right = margin.Bottom;
			margin.Top = margin.Bottom;
		}
		else if (marginData.size() == 2)
		{
			margin.Left = marginData[0].get<float>();
			margin.Right = margin.Left;

			margin.Top = marginData[1].get<float>();
			margin.Bottom = margin.Top;
		}
		else if (marginData.size() == 4)
		{
			margin.Left = marginData[0].get<float>();
			margin.Top = marginData[1].get<float>();
			margin.Right = marginData[2].get<float>();
			margin.Bottom = marginData[3].get<float>();
		}
		else
		{
			JSON_LOADER_EXCEPTION("Control with name: '{}'. 'Margin' array must have 1, 2, or 4 values. Invalid data: {}", m_name, data.dump(4));
		}
	}

	return margin;
}

}