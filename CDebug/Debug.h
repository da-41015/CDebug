#ifndef CLASS_DEBUG_H
#define CLASS_DEBUG_H

#include <list>
#include <string>
#include <sstream>
#include <vector>

class Debug {
    enum ALIGNMENT {CENTER, LEFT, RIGHT};
public:
	// название отлаживаемого участка кода
    Debug(const std::string&);
	// Создать новую секцию(каждая секция имеет свою разметку)
    void NewSection(bool printSubheader = false, const std::string &title="");
	// Ввод с переносом строки
    template <typename Arg, typename... Args>
    void Line(Arg&& arg, Args&&... args);
	// Ввод в текущую строку
	template <typename Arg, typename... Args>
	void InLine(Arg&& arg, Args&&... args);

	void InLineEnd();
	// Завершить ввод. Вывести страницу. Сбросить буфера
    void End();
    //options
	//отключить/включить автоматические пробелы между аргументами Line, InLIne
    void DisableAutoSpacing() {m_enableAutoSpacing = false;}
    void EnableAutoSpacing()  {m_enableAutoSpacing = true;}
	//Установить выравнинвание текущей секции
    void SetSectionAlignment(ALIGNMENT A) {m_itCurrentSection->SetAlignment(A);}
	//Установливает/сбрасывает через сколько вызовов InLine стоит вызывать перенос строки.
	//0 - отключить = default
    void InLineSetQuantity(int a) {m_cInLineCalls = 0; m_inLineQuantity = a;}
	void InLineResetQuantity() {m_cInLineCalls = 0; m_inLineQuantity = 0;}
    //public consts
    static const ALIGNMENT ALIGN_CENTER = CENTER;
    static const ALIGNMENT ALIGN_LEFT   = LEFT;
    static const ALIGNMENT ALIGN_RIGHT  = RIGHT;

private: //definitions
    //определяет ширину всей табилцы и хранит ширину контента
	class Metrics {
    public:
        Metrics(int w=2);

        void setWidth(int width);
        int getContentWidth() const;
        int getTableWidth() const;
    private:
        int m_tableWidth;
        int m_contentWidth;
    };

    class Section {
        typedef std::list<std::string> LineList;
	public:
        Section(const Metrics &m, bool = false, const std::string& = "");
        void SetAlignment(ALIGNMENT A) {m_alignment = A;}
        void addLine(const std::string &);
        void Markup();
        void SolveTabulations();
		int GetMaxLineLength() const;


    private: //service
        void PrintSeparatorLine(int length);
        void PrintLine(const std::string &s, int leftPadding);
        void MakePadding(int val);
        void MakeTitle();
        void EmptyLine();
        int  CalcLeftPadding();
        std::vector<std::size_t> GetTabsPositions(const std::string &);
        void ReplaceTabulations(std::string &s, const std::vector<size_t> amount);

    private: //members
        LineList m_lines;
        const Metrics &m_metrics;
        std::size_t m_maxLineLength;

		bool m_visibleSeparator;
        std::string m_title;
        ALIGNMENT m_alignment;
    };

    typedef std::list<Section> SectionList;
    enum {DEFAULT_LINE_LENGTH = 20,
	      DEFAULT_TABLE_MAXWIDTH = 128};
	
    static const std::string DEFAULT_HEADER_STRING;

private: //service
    //printing
    void printHeaderLeftLine(int n);
    void printHeaderRightLine(int n);
    void printHeaderBoldLine(int);
    void printHeader();
    void printFooter();

    //calibrating
    void Calibrating();
    void calibrateMetrics(int);
    //other
    static std::vector<std::string> SplitString(const std::string &str, char delim);
    static void FilterHeader(std::string &s);
    void SaveInput(const std::string &input);

private: //members
    std::string m_subject;

    SectionList m_sections;
    SectionList::iterator m_itCurrentSection;
    
	Metrics m_metrics;
	
	int m_inLineQuantity;
    int m_cInLineCalls;

    std::string m_inlineBuffer;
    bool m_enableAutoSpacing;

};

template <typename Arg, typename... Args>
void Debug::Line(Arg&& arg, Args&&... args) {
    InLineEnd();
    InLineResetQuantity();


    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(ss<<(m_enableAutoSpacing ? " ":"")
                              << std::forward<Args>(args)),0)...};

    std::string res = ss.str();

    SaveInput(res);
}

template <typename Arg, typename... Args>
void Debug::InLine(Arg&& arg, Args&&... args) {

    if(m_inLineQuantity>0) {
        ++m_cInLineCalls;
    }

    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(ss<<(m_enableAutoSpacing ? " ":"")
                              << std::forward<Args>(args)),0)...};

    m_inlineBuffer+=ss.str();

    if(m_cInLineCalls == m_inLineQuantity && m_inLineQuantity!=0) {
        InLineEnd();
        m_cInLineCalls = 0;
    }
}


#endif
