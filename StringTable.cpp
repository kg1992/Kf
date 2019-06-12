#include "Pch.h"
#include "StringTable.h"

const std::wstring Undefined = L"[[UNDEFINED]]";

StringTable::StringTable()
{
    m_tables[SI_MainMenu_Top] =
    {
        {"en", L"Press number key to select"},
        {"ko", L"����Ű�� ���� �����ϼ���"}
    };

    m_tables[SI_MainMenu_Infinite] =
    {
        {"en", L"1. Infinite Mode"},
        {"ko", L"1. ���Ѹ��"}
    };

    m_tables[SI_MainMenu_Sprint] =
    {
        {"en", L"2. Sprint 40 Mode"},
        {"ko", L"2. ������Ʈ 40��"}
    };

    m_tables[SI_MainMenu_DigDown] =
    {
        {"en", L"3. Dig Down Mode"},
        {"ko", L"3. �ĳ�����"}
    };

    //m_tables[SI_MainMenu_Battle] =
    //{
    //    {"en", L"4. Veresus"},
    //    {"ko", L"4. ����"}
    //};
    
    m_tables[SI_MainMenu_Options] =
    {
        {"en", L"4. Options"},
        {"ko", L"4. ����"}
    };

    m_tables[SI_MainMenu_Quit] =
    {
        {"en", L"5. Quit Game"},
        {"ko", L"5. ��������"}
    };

    m_tables[SI_Options_Resolution] =
    {
        {"en", L"1. Resolution : %dX%d"},
        {"ko", L"1. �ػ� : %dX%d"}
    };

    m_tables[SI_Options_Toggle_Fullscreen] =
    {
        {"en", L"2. Toggle Fullscreen"},
        {"ko", L"2. Ǯ��ũ�� ��ȯ"}
    };

    m_tables[SI_Options_Language] =
    {
        {"en", L"3. Language : %hs"},
        {"ko", L"3. ��� : %hs"}
    };

    m_tables[SI_Options_Return] =
    {
        {"en", L"4. Return"},
        {"ko", L"4. ������"}
    };

    m_tables[SI_LineClear] =
    {
        {"en", L"LINES CLEARED"},
        {"ko", L"���� ��"},
    };

    m_tables[SI_Level] =
    {
        {"en", L"LEVEL"},
        {"ko", L"����"},
    };

    m_tables[SI_Bonus] =
    {
        {"en", L"BONUS"},
        {"ko", L"���ʽ�"},
    };

    m_tables[SI_Combo] =
    {
        {"en", L"COMBO"},
        {"ko", L"�޺�"},
    };

    m_tables[SI_Score] =
    {
        {"en", L"SCORE"},
        {"ko", L"����"},
    };

    m_tables[SI_Time] =
    {
        {"en", L"TIME"},
        {"ko", L"�ð�"},
    };

    m_tables[SI_GrowDuration] =
    {
        {"en", L"GARBAGE SPAWN(ms)"},
        {"ko", L"���غ�� ��ȯ�ֱ�(ms)"},
    };

    m_tables[SI_Ready] =
    {
        {"en", L"Press Space To Start"},
        {"ko", L"�����̽��� ���� ����"},
    };

    m_tables[SI_GameOver] =
    {
        {"en", L"Game Over! Press R to restart. Press Q to return to main menu"},
        {"ko", L"���� ����! R�� ���� �����. Q�� ���� �޴��� ������"}
    };

    m_tables[SI_Sprint_Complete]=
    {
        {"en", L"Done! Check the time on left. Press Q to return to main menu"},
        {"ko", L"�Ϸ�! ������ Ÿ�̸ӿ��� �ð��� Ȯ���ϼ���. Q�� ���� �޴��� ������"}
    };
}

const std::wstring& StringTable::GetString(const std::string& language, StringID id)
{
    std::vector<StringRecord>& table = m_tables[id];
    auto iter = std::find_if(table.begin(), table.end(), [&](const StringRecord& sr)
        {
            return sr.language == language;
        });
    if (iter != table.end())
    {
        return iter->content;
    }
    else
    {
        return Undefined;
    }
}
