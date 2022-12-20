#pragma once
#include "pch.h"
#include "Evergreen/Exceptions/BaseException.h"

namespace Evergreen
{
class FontFamilyException : public BaseException
{
public:
	FontFamilyException(int line, const char* file, const std::string& message) noexcept;
	FontFamilyException(const FontFamilyException&) = delete;
	void operator=(const FontFamilyException&) = delete;
	virtual ~FontFamilyException() noexcept override {}

	const char* what() const noexcept override;

	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;

private:
	std::string m_message;
};



// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class FontFamily
{	
public:
	static const FontFamily Arial;
	static const FontFamily Bahnschrift;
	static const FontFamily Calibri;
	static const FontFamily Cambria;
	static const FontFamily Cambria_Math;
	static const FontFamily Candara;
	static const FontFamily Comic_Sans_MS;
	static const FontFamily Consolas;
	static const FontFamily Constantia;
	static const FontFamily Corbel;
	static const FontFamily Courier_New;
	static const FontFamily Ebrima;
	static const FontFamily Franklin_Gothic;
	static const FontFamily Gabriola;
	static const FontFamily Gadugi;
	static const FontFamily Georgia;
	static const FontFamily Impact;
	static const FontFamily Ink_Free;
	static const FontFamily Javanese_Text;
	static const FontFamily Leelawadee_UI;
	static const FontFamily Lucida_Console;
	static const FontFamily Lucida_Sans_Unicode;
	static const FontFamily Malgun_Gothic;
	static const FontFamily Microsoft_Himalaya;
	static const FontFamily Microsoft_JhengHei;
	static const FontFamily Microsoft_JhengHei_UI;
	static const FontFamily Microsoft_New_Tai_Lue;
	static const FontFamily Microsoft_PhagsPa;
	static const FontFamily Microsoft_Sans_Serif;
	static const FontFamily Microsoft_Tai_Le;
	static const FontFamily Microsoft_YaHei;
	static const FontFamily Microsoft_YaHei_UI;
	static const FontFamily Microsoft_Yi_Baiti;
	static const FontFamily MingLiU_ExtB;
	static const FontFamily PMingLiU_ExtB;
	static const FontFamily MingLiU_HKSCS_ExtB;
	static const FontFamily Mongolian_Baiti;
	static const FontFamily MS_Gothic;
	static const FontFamily MS_UI_Gothic;
	static const FontFamily MS_PGothic;
	static const FontFamily MV_Boli;
	static const FontFamily Myanmar_Text;
	static const FontFamily Nirmala_UI;
	static const FontFamily Palatino_Linotype;
	static const FontFamily Segoe_MDL2_Assets;
	static const FontFamily Segoe_Print;
	static const FontFamily Segoe_Script;
	static const FontFamily Segoe_UI;
	static const FontFamily Segoe_UI_Emoji;
	static const FontFamily Segoe_UI_Historic;
	static const FontFamily Segoe_UI_Symbol;
	static const FontFamily SimSun;
	static const FontFamily NSimSun;
	static const FontFamily SimSun_ExtB;
	static const FontFamily Sitka_Small;
	static const FontFamily Sitka_Text;
	static const FontFamily Sitka_Subheading;
	static const FontFamily Sitka_Heading;
	static const FontFamily Sitka_Display;
	static const FontFamily Sitka_Banner;
	static const FontFamily Sylfaen;
	static const FontFamily Symbol;
	static const FontFamily Tahoma;
	static const FontFamily Times_New_Roman;
	static const FontFamily Trebuchet_MS;
	static const FontFamily Verdana;
	static const FontFamily Webdings;
	static const FontFamily Wingdings;
	static const FontFamily Yu_Gothic;
	static const FontFamily Yu_Gothic_UI;
	static const FontFamily Hololens_MDL2_Assets;
	static const FontFamily Agency_FB;
	static const FontFamily Algerian;
	static const FontFamily Book_Antiqua;
	static const FontFamily Arial_Rounded_MT;
	static const FontFamily Baskerville_Old_Face;
	static const FontFamily Bauhaus_93;
	static const FontFamily Bell_MT;
	static const FontFamily Bernard_MT;
	static const FontFamily Bodoni_MT;
	static const FontFamily Bodoni_MT_Poster;
	static const FontFamily Bookman_Old_Style;
	static const FontFamily Bradley_Hand_ITC;
	static const FontFamily Britannic;
	static const FontFamily Berlin_Sans_FB;
	static const FontFamily Broadway;
	static const FontFamily Brush_Script_MT;
	static const FontFamily Bookshelf_Symbol_7;
	static const FontFamily Californian_FB;
	static const FontFamily Calisto_MT;
	static const FontFamily Castellar;
	static const FontFamily Century_Schoolbook;
	static const FontFamily Centaur;
	static const FontFamily Century;
	static const FontFamily Chiller;
	static const FontFamily Colonna_MT;
	static const FontFamily Cooper;
	static const FontFamily Copperplate_Gothic;
	static const FontFamily Curlz_MT;
	static const FontFamily Dubai;
	static const FontFamily Elephant;
	static const FontFamily Engravers_MT;
	static const FontFamily Eras_ITC;
	static const FontFamily Felix_Titling;
	static const FontFamily Forte;
	static const FontFamily Franklin_Gothic_Book;
	static const FontFamily Freestyle_Script;
	static const FontFamily Freestyle_Script_MT;
	static const FontFamily Footlight_MT;
	static const FontFamily Garamond;
	static const FontFamily Gigi;
	static const FontFamily Gill_Sans_MT;
	static const FontFamily Gill_Sans;
	static const FontFamily Gloucester_MT;
	static const FontFamily Century_Gothic;
	static const FontFamily Goudy_Old_Style;
	static const FontFamily Goudy_Stout;
	static const FontFamily Harlow_Solid;
	static const FontFamily Harrington;
	static const FontFamily Haettenschweiler;
	static const FontFamily High_Tower_Text;
	static const FontFamily Imprint_MT_Shadow;
	static const FontFamily Informal_Roman;
	static const FontFamily Blackadder_ITC;
	static const FontFamily Edwardian_Script_ITC;
	static const FontFamily Kristen_ITC;
	static const FontFamily Jokerman;
	static const FontFamily Juice_ITC;
	static const FontFamily Kunstler_Script;
	static const FontFamily Wide_Latin;
	static const FontFamily Lucida_Bright;
	static const FontFamily Lucida_Calligraphy;
	static const FontFamily Lucida_Fax;
	static const FontFamily Lucida_Handwriting;
	static const FontFamily Lucida_Sans;
	static const FontFamily Lucida_Sans_Typewriter;
	static const FontFamily Magneto;
	static const FontFamily Maiandra_GD;
	static const FontFamily Matura_MT_Script_Capitals;
	static const FontFamily Mistral;
	static const FontFamily Modern_No_20;
	static const FontFamily Monotype_Corsiva;
	static const FontFamily MT_Extra;
	static const FontFamily Niagara_Engraved;
	static const FontFamily Niagara_Solid;
	static const FontFamily OCR_A;
	static const FontFamily Old_English_Text_MT;
	static const FontFamily Onyx;
	static const FontFamily MS_Outlook;
	static const FontFamily Palace_Script_MT;
	static const FontFamily Papyrus;
	static const FontFamily Parchment;
	static const FontFamily Perpetua;
	static const FontFamily Perpetual_Titling_MT;
	static const FontFamily Playbill;
	static const FontFamily Poor_Richard;
	static const FontFamily Pristina;
	static const FontFamily Rage;
	static const FontFamily Ravie;
	static const FontFamily MS_Reference_Sans_Serif;
	static const FontFamily MS_Reference_Specialty;
	static const FontFamily Rockwell;
	static const FontFamily Script_MT;
	static const FontFamily Showcard_Gothic;
	static const FontFamily Snap_ITC;
	static const FontFamily Stencil;
	static const FontFamily Tw_Cen_MT;
	static const FontFamily Tempus_Sans_ITC;
	static const FontFamily Viner_Hand_ITC;
	static const FontFamily Vivaldi;
	static const FontFamily Vladimir_Script;
	static const FontFamily Wingdings_2;
	static const FontFamily Wingdings_3;
	static const FontFamily Marlett;
	
public:
	FontFamily(const FontFamily&) noexcept;
	void operator=(const FontFamily&) noexcept;

	std::string Get() { return m_value; }

	static bool IsValidFontFamily(std::string fontFamily) noexcept { return m_fontFamilyNameMap.find(fontFamily) != m_fontFamilyNameMap.end(); }
	static const FontFamily& GetFontFamily(const std::string& name);
	

private:
	// This constructor is private so that a user cannot create an invalid FontFamily object.
	// This must use one of the built-in static instances
	FontFamily(const std::string& name) noexcept;

	std::string m_value;
	static const std::unordered_map<std::string, const FontFamily> m_fontFamilyNameMap;

	
};
#pragma warning ( pop )


}