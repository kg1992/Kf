#ifndef STRING_TABLE_H_
#define STRING_TABLE_H_

struct StringRecord
{
    StringRecord(std::string language, std::wstring content)
        : language(language)
        , content(content)
    {
    }

    std::string language;
    std::wstring content;
};

class StringTable
{
public:
    enum StringID
    {
        SI_MainMenu_Top,
        SI_MainMenu_Infinite,
        SI_MainMenu_Sprint,
        SI_MainMenu_DigDown,
        SI_MainMenu_Options,
        SI_MainMenu_Quit,

        SI_Options_Resolution,
        SI_Options_Toggle_Fullscreen,
        SI_Options_Language,
        SI_Options_Return,

        SI_LineClear,
        SI_Level,
        SI_Bonus,
        SI_Combo,
        SI_Score,
        SI_Time,
        SI_GrowDuration,

        SI_Ready,
        SI_GameOver,
        SI_Sprint_Complete,
        
        SI_End,
    };

    StringTable();

    const std::wstring& GetString(const std::string& language, StringID id);

private:
    std::vector<StringRecord> m_tables[SI_End];
};

#endif
