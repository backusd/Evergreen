#include "pch.h"
#include "ControlLoader.h"




namespace Evergreen
{
ControlLoader::ControlLoader() noexcept
{
	// Set default loaders that will be used by all controls
	// ... Row/Column/...

}

std::optional<RowColumnPosition> ControlLoader::ParseRowColumnPosition(json& data) noexcept
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
			EG_CORE_ERROR("{}:{} - 'Row' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Row"]);
			return std::nullopt;
		}

		position.Row = data["Row"].get<unsigned int>();
	}

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'Column' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Column"]);
			return std::nullopt;
		}

		position.Column = data["Column"].get<unsigned int>();
	}

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'RowSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["RowSpan"]);
			return std::nullopt;
		}

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}

	if (data.contains("ColumnSpan"))
	{
		if (!data["ColumnSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'ColumnSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["ColumnSpan"]);
			return std::nullopt;
		}

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}

	return position;
}

// ----------------------------------------------------------------------------


}