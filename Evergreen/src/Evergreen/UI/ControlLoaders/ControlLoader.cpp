#include "pch.h"
#include "ControlLoader.h"




namespace Evergreen
{
ControlLoader::ControlLoader() noexcept
{
	// Set default loaders that will be used by all controls
	// ... Row/Column/...

}

bool ControlLoader::ParseRowColumnPosition(json& data, RowColumnPosition& position) noexcept
{
	if (data.contains("Row"))
	{
		if (!data["Row"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'Row' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Row"]);
			return false;
		}

		position.Row = data["Row"].get<unsigned int>();
	}
	else
		position.Row = 0;

	if (data.contains("Column"))
	{
		if (!data["Column"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'Column' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["Column"]);
			return false;
		}

		position.Column = data["Column"].get<unsigned int>();
	}
	else
		position.Column = 0;

	if (data.contains("RowSpan"))
	{
		if (!data["RowSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'RowSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["RowSpan"]);
			return false;
		}

		position.RowSpan = data["RowSpan"].get<unsigned int>();

		if (position.RowSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'RowSpan' with value of 0. Setting rowSpan = 1", __FILE__, __LINE__);
			position.RowSpan = 1;
		}
	}
	else
		position.RowSpan = 1;

	if (data.contains("ColumnSpan"))
	{
		if (!data["ColumnSpan"].is_number_unsigned())
		{
			EG_CORE_ERROR("{}:{} - 'ColumnSpan' value must be an unsigned int. Invalid value: {}", __FILE__, __LINE__, data["ColumnSpan"]);
			return false;
		}

		position.ColumnSpan = data["ColumnSpan"].get<unsigned int>();

		if (position.ColumnSpan == 0)
		{
			EG_CORE_WARN("{}:{} - Found 'ColumnSpan' with value of 0. Setting columnSpan = 1", __FILE__, __LINE__);
			position.ColumnSpan = 1;
		}
	}
	else
		position.ColumnSpan = 1;

	return true;
}

// ----------------------------------------------------------------------------
TextLoader::TextLoader() noexcept
{
	m_keyLoaders["Text"] = [this](Control* textControl, json& data, Layout* parentLayout) -> bool { return this->ParseText(static_cast<Text*>(textControl), data, parentLayout); };
}

bool TextLoader::ParseText(Text* textControl, json& data, Layout* parentLayout) noexcept
{
	return true;
}
}