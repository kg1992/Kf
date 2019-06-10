namespace filesystem = std::experimental::filesystem;

struct Options
{
    std::pair<int, int> resolution = {800, 600};
    std::string language = "en";

    // ref : https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
    const std::vector<std::string>& ListLanguages();

    void IncreaseLanguage();

    void Save(const char* const fileName);
    void Save(const filesystem::path& fileName);
    void Save(std::ostream& s);

    void Load(const char* const fileName);
    void Load(const filesystem::path& fileName);
    void Load(std::istream& s);
};
