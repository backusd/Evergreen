#include "pch.h"
#include "ControlLoader.h"


namespace Evergreen
{
std::optional<RowColumnPosition> ControlLoader::ParseRowColumnPosition(const json& data) noexcept
{
	RowColumnPosition position;
	position.Row = 0;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	if (data.contains("Row"))
	{
		if (!data["Row"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Control with name: '{}'. 'Row' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, m_name, data["Row"].dump(4));
			return std::nullopt;
		}

		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Control with name: '{}'. 'Column' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, m_name, data["Column"].dump(4));
			return std::nullopt;
		}

		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Control with name: '{}'. 'RowSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, m_name, data["RowSpan"].dump(4));
			return std::nullopt;
		}

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Control with name: '{}'. Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__, m_name);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		if (!data["ColumnSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - Control with name: '{}'. 'ColumnSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, m_name, data["ColumnSpan"].dump(4));
			return std::nullopt;
		}

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Control with name: '{}'. Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__, m_name);
			position.ColumnSpan = 1;
		}
	}

	return position;
}

}