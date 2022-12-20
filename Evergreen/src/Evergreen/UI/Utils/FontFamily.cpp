#include "pch.h"
#include "FontFamily.h"

namespace Evergreen
{
FontFamilyException::FontFamilyException(int line, const char* file, const std::string& message) noexcept :
	BaseException(line, file),
	m_message(message)
{
}
const char* FontFamilyException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
	return m_whatBuffer.c_str();
}
const char* FontFamilyException::GetType() const noexcept
{
	return "FontFamily Exception";
}
std::string FontFamilyException::GetErrorInfo() const noexcept
{
	return m_message;
}



const FontFamily FontFamily::Arial("Arial");
const FontFamily FontFamily::Bahnschrift("Bahnschrift");
const FontFamily FontFamily::Calibri("Calibri");
const FontFamily FontFamily::Cambria("Cambria");
const FontFamily FontFamily::Cambria_Math("Cambria Math");
const FontFamily FontFamily::Candara("Candara");
const FontFamily FontFamily::Comic_Sans_MS("Comic Sans MS");
const FontFamily FontFamily::Consolas("Consolas");
const FontFamily FontFamily::Constantia("Constantia");
const FontFamily FontFamily::Corbel("Corbel");
const FontFamily FontFamily::Courier_New("Courier New");
const FontFamily FontFamily::Ebrima("Ebrima");
const FontFamily FontFamily::Franklin_Gothic("Franklin Gothic");
const FontFamily FontFamily::Gabriola("Gabriola");
const FontFamily FontFamily::Gadugi("Gadugi");
const FontFamily FontFamily::Georgia("Georgia");
const FontFamily FontFamily::Impact("Impact");
const FontFamily FontFamily::Ink_Free("Ink Free");
const FontFamily FontFamily::Javanese_Text("Javanese Text");
const FontFamily FontFamily::Leelawadee_UI("Leelawadee UI");
const FontFamily FontFamily::Lucida_Console("Lucida Console");
const FontFamily FontFamily::Lucida_Sans_Unicode("Lucida Sans Unicode");
const FontFamily FontFamily::Malgun_Gothic("Malgun Gothic");
const FontFamily FontFamily::Microsoft_Himalaya("Microsoft Himalaya");
const FontFamily FontFamily::Microsoft_JhengHei("Microsoft JhengHei");
const FontFamily FontFamily::Microsoft_JhengHei_UI("Microsoft JhengHei UI");
const FontFamily FontFamily::Microsoft_New_Tai_Lue("Microsoft New Tai Lue");
const FontFamily FontFamily::Microsoft_PhagsPa("Microsoft PhagsPa");
const FontFamily FontFamily::Microsoft_Sans_Serif("Microsoft Sans Serif");
const FontFamily FontFamily::Microsoft_Tai_Le("Microsoft Tai Le");
const FontFamily FontFamily::Microsoft_YaHei("Microsoft YaHei");
const FontFamily FontFamily::Microsoft_YaHei_UI("Microsoft YaHei UI");
const FontFamily FontFamily::Microsoft_Yi_Baiti("Microsoft Yi Baiti");
const FontFamily FontFamily::MingLiU_ExtB("MingLiU-ExtB");
const FontFamily FontFamily::PMingLiU_ExtB("PMingLiU-ExtB");
const FontFamily FontFamily::MingLiU_HKSCS_ExtB("MingLiU_HKSCS-ExtB");
const FontFamily FontFamily::Mongolian_Baiti("Mongolian Baiti");
const FontFamily FontFamily::MS_Gothic("MS Gothic");
const FontFamily FontFamily::MS_UI_Gothic("MS UI Gothic");
const FontFamily FontFamily::MS_PGothic("MS PGothic");
const FontFamily FontFamily::MV_Boli("MV Boli");
const FontFamily FontFamily::Myanmar_Text("Myanmar Text");
const FontFamily FontFamily::Nirmala_UI("Nirmala UI");
const FontFamily FontFamily::Palatino_Linotype("Palatino Linotype");
const FontFamily FontFamily::Segoe_MDL2_Assets("Segoe MDL2 Assets");
const FontFamily FontFamily::Segoe_Print("Segoe Print");
const FontFamily FontFamily::Segoe_Script("Segoe Script");
const FontFamily FontFamily::Segoe_UI("Segoe UI");
const FontFamily FontFamily::Segoe_UI_Emoji("Segoe UI Emoji");
const FontFamily FontFamily::Segoe_UI_Historic("Segoe UI Historic");
const FontFamily FontFamily::Segoe_UI_Symbol("Segoe UI Symbol");
const FontFamily FontFamily::SimSun("SimSun");
const FontFamily FontFamily::NSimSun("NSimSun");
const FontFamily FontFamily::SimSun_ExtB("SimSun-ExtB");
const FontFamily FontFamily::Sitka_Small("Sitka Small");
const FontFamily FontFamily::Sitka_Text("Sitka Text");
const FontFamily FontFamily::Sitka_Subheading("Sitka Subheading");
const FontFamily FontFamily::Sitka_Heading("Sitka Heading");
const FontFamily FontFamily::Sitka_Display("Sitka Display");
const FontFamily FontFamily::Sitka_Banner("Sitka Banner");
const FontFamily FontFamily::Sylfaen("Sylfaen");
const FontFamily FontFamily::Symbol("Symbol");
const FontFamily FontFamily::Tahoma("Tahoma");
const FontFamily FontFamily::Times_New_Roman("Times New Roman");
const FontFamily FontFamily::Trebuchet_MS("Trebuchet MS");
const FontFamily FontFamily::Verdana("Verdana");
const FontFamily FontFamily::Webdings("Webdings");
const FontFamily FontFamily::Wingdings("Wingdings");
const FontFamily FontFamily::Yu_Gothic("Yu Gothic");
const FontFamily FontFamily::Yu_Gothic_UI("Yu Gothic UI");
const FontFamily FontFamily::Hololens_MDL2_Assets("Hololens MDL2 Assets");
const FontFamily FontFamily::Agency_FB("Agency FB");
const FontFamily FontFamily::Algerian("Algerian");
const FontFamily FontFamily::Book_Antiqua("Book Antiqua");
const FontFamily FontFamily::Arial_Rounded_MT("Arial Rounded MT");
const FontFamily FontFamily::Baskerville_Old_Face("Baskerville Old Face");
const FontFamily FontFamily::Bauhaus_93("Bauhaus 93");
const FontFamily FontFamily::Bell_MT("Bell MT");
const FontFamily FontFamily::Bernard_MT("Bernard MT");
const FontFamily FontFamily::Bodoni_MT("Bodoni MT");
const FontFamily FontFamily::Bodoni_MT_Poster("Bodoni MT Poster");
const FontFamily FontFamily::Bookman_Old_Style("Bookman Old Style");
const FontFamily FontFamily::Bradley_Hand_ITC("Bradley Hand ITC");
const FontFamily FontFamily::Britannic("Britannic");
const FontFamily FontFamily::Berlin_Sans_FB("Berlin Sans FB");
const FontFamily FontFamily::Broadway("Broadway");
const FontFamily FontFamily::Brush_Script_MT("Brush Script MT");
const FontFamily FontFamily::Bookshelf_Symbol_7("Bookshelf Symbol 7");
const FontFamily FontFamily::Californian_FB("Californian FB");
const FontFamily FontFamily::Calisto_MT("Calisto MT");
const FontFamily FontFamily::Castellar("Castellar");
const FontFamily FontFamily::Century_Schoolbook("Century Schoolbook");
const FontFamily FontFamily::Centaur("Centaur");
const FontFamily FontFamily::Century("Century");
const FontFamily FontFamily::Chiller("Chiller");
const FontFamily FontFamily::Colonna_MT("Colonna MT");
const FontFamily FontFamily::Cooper("Cooper");
const FontFamily FontFamily::Copperplate_Gothic("Copperplate Gothic");
const FontFamily FontFamily::Curlz_MT("Curlz MT");
const FontFamily FontFamily::Dubai("Dubai");
const FontFamily FontFamily::Elephant("Elephant");
const FontFamily FontFamily::Engravers_MT("Engravers MT");
const FontFamily FontFamily::Eras_ITC("Eras ITC");
const FontFamily FontFamily::Felix_Titling("Felix Titling");
const FontFamily FontFamily::Forte("Forte");
const FontFamily FontFamily::Franklin_Gothic_Book("Franklin Gothic Book");
const FontFamily FontFamily::Freestyle_Script("Freestyle Script");
const FontFamily FontFamily::Freestyle_Script_MT("Freestyle Script MT");
const FontFamily FontFamily::Footlight_MT("Footlight MT");
const FontFamily FontFamily::Garamond("Garamond");
const FontFamily FontFamily::Gigi("Gigi");
const FontFamily FontFamily::Gill_Sans_MT("Gill Sans MT");
const FontFamily FontFamily::Gill_Sans("Gill Sans");
const FontFamily FontFamily::Gloucester_MT("Gloucester MT");
const FontFamily FontFamily::Century_Gothic("Century Gothic");
const FontFamily FontFamily::Goudy_Old_Style("Goudy Old Style");
const FontFamily FontFamily::Goudy_Stout("Goudy Stout");
const FontFamily FontFamily::Harlow_Solid("Harlow Solid");
const FontFamily FontFamily::Harrington("Harrington");
const FontFamily FontFamily::Haettenschweiler("Haettenschweiler");
const FontFamily FontFamily::High_Tower_Text("High Tower Text");
const FontFamily FontFamily::Imprint_MT_Shadow("Imprint MT Shadow");
const FontFamily FontFamily::Informal_Roman("Informal Roman");
const FontFamily FontFamily::Blackadder_ITC("Blackadder ITC");
const FontFamily FontFamily::Edwardian_Script_ITC("Edwardian Script ITC");
const FontFamily FontFamily::Kristen_ITC("Kristen ITC");
const FontFamily FontFamily::Jokerman("Jokerman");
const FontFamily FontFamily::Juice_ITC("Juice ITC");
const FontFamily FontFamily::Kunstler_Script("Kunstler Script");
const FontFamily FontFamily::Wide_Latin("Wide Latin");
const FontFamily FontFamily::Lucida_Bright("Lucida Bright");
const FontFamily FontFamily::Lucida_Calligraphy("Lucida Calligraphy");
const FontFamily FontFamily::Lucida_Fax("Lucida Fax");
const FontFamily FontFamily::Lucida_Handwriting("Lucida Handwriting");
const FontFamily FontFamily::Lucida_Sans("Lucida Sans");
const FontFamily FontFamily::Lucida_Sans_Typewriter("Lucida Sans Typewriter");
const FontFamily FontFamily::Magneto("Magneto");
const FontFamily FontFamily::Maiandra_GD("Maiandra GD");
const FontFamily FontFamily::Matura_MT_Script_Capitals("Matura MT Script Capitals");
const FontFamily FontFamily::Mistral("Mistral");
const FontFamily FontFamily::Modern_No_20("Modern No. 20");
const FontFamily FontFamily::Monotype_Corsiva("Monotype Corsiva");
const FontFamily FontFamily::MT_Extra("MT Extra");
const FontFamily FontFamily::Niagara_Engraved("Niagara Engraved");
const FontFamily FontFamily::Niagara_Solid("Niagara Solid");
const FontFamily FontFamily::OCR_A("OCR A");
const FontFamily FontFamily::Old_English_Text_MT("Old English Text MT");
const FontFamily FontFamily::Onyx("Onyx");
const FontFamily FontFamily::MS_Outlook("MS Outlook");
const FontFamily FontFamily::Palace_Script_MT("Palace Script MT");
const FontFamily FontFamily::Papyrus("Papyrus");
const FontFamily FontFamily::Parchment("Parchment");
const FontFamily FontFamily::Perpetua("Perpetua");
const FontFamily FontFamily::Perpetual_Titling_MT("Perpetual Titling MT");
const FontFamily FontFamily::Playbill("Playbill");
const FontFamily FontFamily::Poor_Richard("Poor Richard");
const FontFamily FontFamily::Pristina("Pristina");
const FontFamily FontFamily::Rage("Rage");
const FontFamily FontFamily::Ravie("Ravie");
const FontFamily FontFamily::MS_Reference_Sans_Serif("MS Reference Sans Serif");
const FontFamily FontFamily::MS_Reference_Specialty("MS Reference Specialty");
const FontFamily FontFamily::Rockwell("Rockwell");
const FontFamily FontFamily::Script_MT("Script MT");
const FontFamily FontFamily::Showcard_Gothic("Showcard Gothic");
const FontFamily FontFamily::Snap_ITC("Snap ITC");
const FontFamily FontFamily::Stencil("Stencil");
const FontFamily FontFamily::Tw_Cen_MT("Tw Cen MT");
const FontFamily FontFamily::Tempus_Sans_ITC("Tempus Sans ITC");
const FontFamily FontFamily::Viner_Hand_ITC("Viner Hand ITC");
const FontFamily FontFamily::Vivaldi("Vivaldi");
const FontFamily FontFamily::Vladimir_Script("Vladimir Script");
const FontFamily FontFamily::Wingdings_2("Wingdings 2");
const FontFamily FontFamily::Wingdings_3("Wingdings 3"); 
const FontFamily FontFamily::Marlett("Marlett");

const std::unordered_map<std::string, const FontFamily> FontFamily::m_fontFamilyNameMap = {
	{ "Arial", FontFamily::Arial },
	{ "Bahnschrift", FontFamily::Bahnschrift },
	{ "Calibri", FontFamily::Calibri },
	{ "Cambria", FontFamily::Cambria },
	{ "Cambria Math", FontFamily::Cambria_Math },
	{ "Candara", FontFamily::Candara },
	{ "Comic Sans MS", FontFamily::Comic_Sans_MS },
	{ "Consolas", FontFamily::Consolas },
	{ "Constantia", FontFamily::Constantia },
	{ "Corbel", FontFamily::Corbel },
	{ "Courier New", FontFamily::Courier_New },
	{ "Ebrima", FontFamily::Ebrima },
	{ "Franklin Gothic", FontFamily::Franklin_Gothic },
	{ "Gabriola", FontFamily::Gabriola },
	{ "Gadugi", FontFamily::Gadugi },
	{ "Georgia", FontFamily::Georgia },
	{ "Impact", FontFamily::Impact },
	{ "Ink Free", FontFamily::Ink_Free },
	{ "Javanese Text", FontFamily::Javanese_Text },
	{ "Leelawadee UI", FontFamily::Leelawadee_UI },
	{ "Lucida Console", FontFamily::Lucida_Console },
	{ "Lucida Sans Unicode", FontFamily::Lucida_Sans_Unicode },
	{ "Malgun Gothic", FontFamily::Malgun_Gothic },
	{ "Microsoft Himalaya", FontFamily::Microsoft_Himalaya },
	{ "Microsoft JhengHei", FontFamily::Microsoft_JhengHei },
	{ "Microsoft JhengHei UI", FontFamily::Microsoft_JhengHei_UI },
	{ "Microsoft New Tai Lue", FontFamily::Microsoft_New_Tai_Lue },
	{ "Microsoft PhagsPa", FontFamily::Microsoft_PhagsPa },
	{ "Microsoft Sans Serif", FontFamily::Microsoft_Sans_Serif },
	{ "Microsoft Tai Le", FontFamily::Microsoft_Tai_Le },
	{ "Microsoft YaHei", FontFamily::Microsoft_YaHei },
	{ "Microsoft YaHei UI", FontFamily::Microsoft_YaHei_UI },
	{ "Microsoft Yi Baiti", FontFamily::Microsoft_Yi_Baiti },
	{ "MingLiU-ExtB", FontFamily::MingLiU_ExtB },
	{ "PMingLiU-ExtB", FontFamily::PMingLiU_ExtB },
	{ "MingLiU_HKSCS-ExtB", FontFamily::MingLiU_HKSCS_ExtB },
	{ "Mongolian Baiti", FontFamily::Mongolian_Baiti },
	{ "MS Gothic", FontFamily::MS_Gothic },
	{ "MS UI Gothic", FontFamily::MS_UI_Gothic },
	{ "MS PGothic", FontFamily::MS_PGothic },
	{ "MV Boli", FontFamily::MV_Boli },
	{ "Myanmar Text", FontFamily::Myanmar_Text },
	{ "Nirmala UI", FontFamily::Nirmala_UI },
	{ "Palatino Linotype", FontFamily::Palatino_Linotype },
	{ "Segoe MDL2 Assets", FontFamily::Segoe_MDL2_Assets },
	{ "Segoe Print", FontFamily::Segoe_Print },
	{ "Segoe Script", FontFamily::Segoe_Script },
	{ "Segoe UI", FontFamily::Segoe_UI },
	{ "Segoe UI Emoji", FontFamily::Segoe_UI_Emoji },
	{ "Segoe UI Historic", FontFamily::Segoe_UI_Historic },
	{ "Segoe UI Symbol", FontFamily::Segoe_UI_Symbol },
	{ "SimSun", FontFamily::SimSun },
	{ "NSimSun", FontFamily::NSimSun },
	{ "SimSun-ExtB", FontFamily::SimSun_ExtB },
	{ "Sitka Small", FontFamily::Sitka_Small },
	{ "Sitka Text", FontFamily::Sitka_Text },
	{ "Sitka Subheading", FontFamily::Sitka_Subheading },
	{ "Sitka Heading", FontFamily::Sitka_Heading },
	{ "Sitka Display", FontFamily::Sitka_Display },
	{ "Sitka Banner", FontFamily::Sitka_Banner },
	{ "Sylfaen", FontFamily::Sylfaen },
	{ "Symbol", FontFamily::Symbol },
	{ "Tahoma", FontFamily::Tahoma },
	{ "Times New Roman", FontFamily::Times_New_Roman },
	{ "Trebuchet MS", FontFamily::Trebuchet_MS },
	{ "Verdana", FontFamily::Verdana },
	{ "Webdings", FontFamily::Webdings },
	{ "Wingdings", FontFamily::Wingdings },
	{ "Yu Gothic", FontFamily::Yu_Gothic },
	{ "Yu Gothic UI", FontFamily::Yu_Gothic_UI },
	{ "Hololens MDL2 Assets", FontFamily::Hololens_MDL2_Assets },
	{ "Agency FB", FontFamily::Agency_FB },
	{ "Algerian", FontFamily::Algerian },
	{ "Book Antiqua", FontFamily::Book_Antiqua },
	{ "Arial Rounded MT", FontFamily::Arial_Rounded_MT },
	{ "Baskerville Old Face", FontFamily::Baskerville_Old_Face },
	{ "Bauhaus 93", FontFamily::Bauhaus_93 },
	{ "Bell MT", FontFamily::Bell_MT },
	{ "Bernard MT", FontFamily::Bernard_MT },
	{ "Bodoni MT", FontFamily::Bodoni_MT },
	{ "Bodoni MT Poster", FontFamily::Bodoni_MT_Poster },
	{ "Bookman Old Style", FontFamily::Bookman_Old_Style },
	{ "Bradley Hand ITC", FontFamily::Bradley_Hand_ITC },
	{ "Britannic", FontFamily::Britannic },
	{ "Berlin Sans FB", FontFamily::Berlin_Sans_FB },
	{ "Broadway", FontFamily::Broadway },
	{ "Brush Script MT", FontFamily::Brush_Script_MT },
	{ "Bookshelf Symbol 7", FontFamily::Bookshelf_Symbol_7 },
	{ "Californian FB", FontFamily::Californian_FB },
	{ "Calisto MT", FontFamily::Calisto_MT },
	{ "Castellar", FontFamily::Castellar },
	{ "Century Schoolbook", FontFamily::Century_Schoolbook },
	{ "Centaur", FontFamily::Centaur },
	{ "Century", FontFamily::Century },
	{ "Chiller", FontFamily::Chiller },
	{ "Colonna MT", FontFamily::Colonna_MT },
	{ "Cooper", FontFamily::Cooper },
	{ "Copperplate Gothic", FontFamily::Copperplate_Gothic },
	{ "Curlz MT", FontFamily::Curlz_MT },
	{ "Dubai", FontFamily::Dubai },
	{ "Elephant", FontFamily::Elephant },
	{ "Engravers MT", FontFamily::Engravers_MT },
	{ "Eras ITC", FontFamily::Eras_ITC },
	{ "Felix Titling", FontFamily::Felix_Titling },
	{ "Forte", FontFamily::Forte },
	{ "Franklin Gothic Book", FontFamily::Franklin_Gothic_Book },
	{ "Freestyle Script", FontFamily::Freestyle_Script },
	{ "Freestyle Script MT", FontFamily::Freestyle_Script_MT },
	{ "Footlight MT", FontFamily::Footlight_MT },
	{ "Garamond", FontFamily::Garamond },
	{ "Gigi", FontFamily::Gigi },
	{ "Gill Sans MT", FontFamily::Gill_Sans_MT },
	{ "Gill Sans", FontFamily::Gill_Sans },
	{ "Gloucester MT", FontFamily::Gloucester_MT },
	{ "Century Gothic", FontFamily::Century_Gothic },
	{ "Goudy Old Style", FontFamily::Goudy_Old_Style },
	{ "Goudy Stout", FontFamily::Goudy_Stout },
	{ "Harlow Solid", FontFamily::Harlow_Solid },
	{ "Harrington", FontFamily::Harrington },
	{ "Haettenschweiler", FontFamily::Haettenschweiler },
	{ "High Tower Text", FontFamily::High_Tower_Text },
	{ "Imprint MT Shadow", FontFamily::Imprint_MT_Shadow },
	{ "Informal Roman", FontFamily::Informal_Roman },
	{ "Blackadder ITC", FontFamily::Blackadder_ITC },
	{ "Edwardian Script ITC", FontFamily::Edwardian_Script_ITC },
	{ "Kristen ITC", FontFamily::Kristen_ITC },
	{ "Jokerman", FontFamily::Jokerman },
	{ "Juice ITC", FontFamily::Juice_ITC },
	{ "Kunstler Script", FontFamily::Kunstler_Script },
	{ "Wide Latin", FontFamily::Wide_Latin },
	{ "Lucida Bright", FontFamily::Lucida_Bright },
	{ "Lucida Calligraphy", FontFamily::Lucida_Calligraphy },
	{ "Lucida Fax", FontFamily::Lucida_Fax },
	{ "Lucida Handwriting", FontFamily::Lucida_Handwriting },
	{ "Lucida Sans", FontFamily::Lucida_Sans },
	{ "Lucida Sans Typewriter", FontFamily::Lucida_Sans_Typewriter },
	{ "Magneto", FontFamily::Magneto },
	{ "Maiandra GD", FontFamily::Maiandra_GD },
	{ "Matura MT Script Capitals", FontFamily::Matura_MT_Script_Capitals },
	{ "Mistral", FontFamily::Mistral },
	{ "Modern No. 20", FontFamily::Modern_No_20 },
	{ "Monotype Corsiva", FontFamily::Monotype_Corsiva },
	{ "MT Extra", FontFamily::MT_Extra },
	{ "Niagara Engraved", FontFamily::Niagara_Engraved },
	{ "Niagara Solid", FontFamily::Niagara_Solid },
	{ "OCR A", FontFamily::OCR_A },
	{ "Old English Text MT", FontFamily::Old_English_Text_MT },
	{ "Onyx", FontFamily::Onyx },
	{ "MS Outlook", FontFamily::MS_Outlook },
	{ "Palace Script MT", FontFamily::Palace_Script_MT },
	{ "Papyrus", FontFamily::Papyrus },
	{ "Parchment", FontFamily::Parchment },
	{ "Perpetua", FontFamily::Perpetua },
	{ "Perpetual Titling MT", FontFamily::Perpetual_Titling_MT },
	{ "Playbill", FontFamily::Playbill },
	{ "Poor Richard", FontFamily::Poor_Richard },
	{ "Pristina", FontFamily::Pristina },
	{ "Rage", FontFamily::Rage },
	{ "Ravie", FontFamily::Ravie },
	{ "MS Reference Sans Serif", FontFamily::MS_Reference_Sans_Serif },
	{ "MS Reference Specialty", FontFamily::MS_Reference_Specialty },
	{ "Rockwell", FontFamily::Rockwell },
	{ "Script MT", FontFamily::Script_MT },
	{ "Showcard Gothic", FontFamily::Showcard_Gothic },
	{ "Snap ITC", FontFamily::Snap_ITC },
	{ "Stencil", FontFamily::Stencil },
	{ "Tw Cen MT", FontFamily::Tw_Cen_MT },
	{ "Tempus Sans ITC", FontFamily::Tempus_Sans_ITC },
	{ "Viner Hand ITC", FontFamily::Viner_Hand_ITC },
	{ "Vivaldi", FontFamily::Vivaldi },
	{ "Vladimir Script", FontFamily::Vladimir_Script },
	{ "Wingdings 2", FontFamily::Wingdings_2 },
	{ "Wingdings 3", FontFamily::Wingdings_3 },
	{ "Marlett", FontFamily::Marlett }
};





FontFamily::FontFamily(const std::string& name) noexcept :
	m_value(name)
{
}
FontFamily::FontFamily(const FontFamily& rhs) noexcept :
	m_value(rhs.m_value)
{
}
void FontFamily::operator=(const FontFamily& rhs) noexcept
{
	m_value = rhs.m_value;
}

const FontFamily& FontFamily::GetFontFamily(const std::string& name)
{
	if (!IsValidFontFamily(name))
		throw FontFamilyException(__LINE__, __FILE__, std::format("Invalid FontFamilly string: {}", name));

	return m_fontFamilyNameMap.at(name);
}

}