#include "Pch.h"
#include "StringTable.h"

const std::wstring Undefined = L"[[UNDEFINED]]";

StringTable::StringTable()
{
    m_tables[SI_MainMenu_Top] =
    {
        {"en", L"Press number key to select"},
        {"ko", L"숫자키를 눌러 선택하세요"}
    };

    m_tables[SI_MainMenu_Infinite] =
    {
        {"en", L"1. Infinite Mode"},
        {"ko", L"1. 무한모드"}
    };

    m_tables[SI_MainMenu_Sprint] =
    {
        {"en", L"2. Sprint 40 Mode"},
        {"ko", L"2. 스프린트 40줄"}
    };

    m_tables[SI_MainMenu_DigDown] =
    {
        {"en", L"3. Dig Down Mode"},
        {"ko", L"3. 파내리기"}
    };

    //m_tables[SI_MainMenu_Battle] =
    //{
    //    {"en", L"4. Veresus"},
    //    {"ko", L"4. 대전"}
    //};
    
    m_tables[SI_MainMenu_Options] =
    {
        {"en", L"4. Options"},
        {"ko", L"4. 설정"}
    };

    m_tables[SI_MainMenu_Quit] =
    {
        {"en", L"5. Quit Game"},
        {"ko", L"5. 게임종료"}
    };

    m_tables[SI_Options_Resolution] =
    {
        {"en", L"1. Resolution : %dX%d"},
        {"ko", L"1. 해상도 : %dX%d"}
    };

    m_tables[SI_Options_Toggle_Fullscreen] =
    {
        {"en", L"2. Toggle Fullscreen"},
        {"ko", L"2. 풀스크린 전환"}
    };

    m_tables[SI_Options_Language] =
    {
        {"en", L"3. Language : %hs"},
        {"ko", L"3. 언어 : %hs"}
    };

    m_tables[SI_Options_Return] =
    {
        {"en", L"4. Return"},
        {"ko", L"4. 나가기"}
    };

    m_tables[SI_LineClear] =
    {
        {"en", L"LINES CLEARED"},
        {"ko", L"지운 줄"},
    };

    m_tables[SI_Level] =
    {
        {"en", L"LEVEL"},
        {"ko", L"레벨"},
    };

    m_tables[SI_Bonus] =
    {
        {"en", L"BONUS"},
        {"ko", L"보너스"},
    };

    m_tables[SI_Combo] =
    {
        {"en", L"COMBO"},
        {"ko", L"콤보"},
    };

    m_tables[SI_Score] =
    {
        {"en", L"SCORE"},
        {"ko", L"점수"},
    };

    m_tables[SI_Time] =
    {
        {"en", L"TIME"},
        {"ko", L"시간"},
    };

    m_tables[SI_GrowDuration] =
    {
        {"en", L"GARBAGE SPAWN(ms)"},
        {"ko", L"방해블록 소환주기(ms)"},
    };

    m_tables[SI_Ready] =
    {
        {"en", L"Press Space To Start"},
        {"ko", L"스페이스를 눌러 시작"},
    };

    m_tables[SI_GameOver] =
    {
        {"en", L"Game Over! Press R to restart. Press Q to return to main menu"},
        {"ko", L"게임 오버! R을 눌러 재시작. Q를 눌러 메뉴로 나가기"}
    };

    m_tables[SI_Sprint_Complete]=
    {
        {"en", L"Done! Check the time on left. Press Q to return to main menu"},
        {"ko", L"완료! 왼쪽의 타이머에서 시간을 확인하세요. Q를 눌러 메뉴로 나가기"}
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
