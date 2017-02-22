#include "Debug.h"

#include <iostream>
#include <valarray>

const std::string Debug::DEFAULT_HEADER_STRING = "Debug::";


// --------------------------------- METRICS REALIZATION ---------------------------------
Debug::Metrics::Metrics(int w) {
    if(w<2)
        w=2;
    setWidth(w);
}

void Debug::Metrics::setWidth(int width) {
    if(width & 1)
        ++width;

    m_tableWidth = width;
    m_contentWidth = m_tableWidth-2;
}

int Debug::Metrics::getContentWidth() const {
    return m_contentWidth;
}

int Debug::Metrics::getTableWidth() const {
    return m_tableWidth;
}

// ------------------------------------------------- SECTION REALIZATON -----------------------------------------------


Debug::Section::Section(const Metrics &m,
                        bool visibleSeparator,
                        const std::string &title)
                        : m_metrics(m),
                          m_maxLineLength(0),
                          m_visibleSeparator(visibleSeparator),
                          m_title(title),
                          m_alignment(CENTER)
{
    Debug::FilterHeader(m_title);
}

void Debug::Section::addLine(const std::string &s) {

    if(s.size()>(unsigned)m_maxLineLength) {
        m_maxLineLength = s.size();
    }

    m_lines.push_back(s);
}

void Debug::Section::Markup() {

    EmptyLine();

    int leftPadding = CalcLeftPadding();

    if(m_visibleSeparator) {
        MakeTitle();
    }

    for(auto &i:m_lines) {
        PrintLine(i, leftPadding);
    }
}

inline void Debug::Section::PrintLine(const std::string &s, int leftPadding) {
    std::cout<<"|";
    MakePadding(leftPadding);
    std::cout<<s;
    MakePadding(m_metrics.getContentWidth()-leftPadding-s.size());
    std::cout<<"|\n";
}

inline void Debug::Section::EmptyLine() {
    std::cout<<"|";
    MakePadding(m_metrics.getContentWidth());
    std::cout<<"|\n";
}

inline void Debug::Section::MakeTitle() {
    std::cout<<"|";

    int strSize = m_title.size();
    int lineLengthLeft = (m_metrics.getContentWidth() - strSize)/2;
    int lineLengthRight = m_metrics.getContentWidth() - strSize - lineLengthLeft;

    PrintSeparatorLine(lineLengthLeft);
    std::cout<<m_title;
    PrintSeparatorLine(lineLengthRight);

    std::cout<<"|\n";
    EmptyLine();
}

void Debug::Section::MakePadding(int val) {
    char texture = ' ';
    for(int i = 0; i<val; ++i) {
        std::cout<<texture;
    }
}

void Debug::Section::PrintSeparatorLine(int val) {
    char texture = '-';
    for(int i = 0; i < val; ++i) {
        std::cout<<texture;
    }
}

inline int Debug::Section::CalcLeftPadding() {
    int leftPadding = (m_metrics.getContentWidth() - m_maxLineLength)/2;

    if(leftPadding>1) {

        switch(m_alignment) {

        case CENTER:

        break;

        case LEFT:
            leftPadding = 1;
        break;

        case RIGHT:
            leftPadding = m_metrics.getContentWidth()-m_maxLineLength-1;
        break;

        }
    }

    return leftPadding;
}

//говнокодище
void Debug::Section::SolveTabulations() {
    int valArrAmount = INT_MAX;
    int valArrSize = 0;
    
	std::vector<std::vector<size_t>> tabPositions;
    std::vector<size_t> stringSizes;

    for(auto &i:m_lines) {
        tabPositions.push_back(GetTabsPositions(i));
        stringSizes.push_back(i.size());

        int s = tabPositions.rbegin()->size();
        if(s!=0) {
            if(s<valArrAmount)
                valArrAmount = s;

            ++valArrSize;
        }
    }

    if(valArrSize == 0)
        return;

    std::vector<std::vector<size_t>> workVector;

    int t = 0;
    for(auto &i:tabPositions) {
        if(i.size()>0 && i.at(0)==0) {
            std::vector<size_t> temp;

            for(size_t c = 1; c<i.size(); ++c) {
                temp.push_back(i.at(c));
            }

            temp.push_back(stringSizes.at(t));
            workVector.push_back(temp);
        }
        else {
            workVector.push_back(i);
        }
        ++t;
    }

    std::vector<std::valarray<size_t>> tabPositionsInColumns;
    for(int i = 0; i<valArrAmount; ++i) {
        t = 0;
        tabPositionsInColumns.push_back(std::valarray<size_t>(valArrSize));
        for(auto &it:workVector) {
            if(it.size()>0) {
                tabPositionsInColumns.at(i)[t] = it.at(i);
                ++t;
            }
        }
    }

    for(size_t i = 0; i<tabPositionsInColumns.size(); ++i) {
        tabPositionsInColumns.at(i) = tabPositionsInColumns.at(i).max() - tabPositionsInColumns.at(i);

        for(t = i+1; (size_t)t<tabPositionsInColumns.size(); ++t) {
            tabPositionsInColumns.at(t)+=tabPositionsInColumns.at(i);
        }
    }

    workVector.clear();

    for(int i = 0; i<valArrSize; ++i) {
        workVector.push_back(std::vector<size_t>());

        for(auto &it:tabPositionsInColumns) {
            workVector.rbegin()->push_back(it[i]);
        }
    }

    t = 0;
    auto iter = workVector.begin();
    for(auto &i:m_lines) {
        if(!tabPositions.at(t).empty()) {
            ReplaceTabulations(i, *iter);

            if(i.size()>m_maxLineLength)
                m_maxLineLength = i.size();

            ++iter;
        }

        ++t;
    }
}
//конец говнокодища

int Debug::Section::GetMaxLineLength() const {
	return m_maxLineLength > m_title.size() ? m_maxLineLength : m_title.size();
}

void Debug::Section::ReplaceTabulations(std::string &s, const std::vector<size_t> amountOfSpaces) {
    std::size_t i = 0;
    std::size_t start = 0, end = 0;

    while ((end = s.find('\t', start)) != std::string::npos) {
        s.at(end)=' ';

        if(i<amountOfSpaces.size()) {
            std::string inserter(amountOfSpaces.at(i), ' ');
            s.insert(end, inserter);
        }
        ++i;
        start = end+1;
    }
}

std::vector<std::size_t> Debug::Section::GetTabsPositions(const std::string &target) {
    std::size_t start = 0, end = 0;
    std::vector<size_t> ret;
    while ((end = target.find('\t', start)) != std::string::npos) {
        ret.push_back(end);
        start = end+1;
    }
    return ret;
}
// ------------------------------------------------- DEBUG REALIZATION -----------------------------------------------

Debug::Debug(const std::string &subject):m_subject(subject), m_enableAutoSpacing(true) {
    m_subject = DEFAULT_HEADER_STRING + m_subject;
    m_metrics.setWidth(DEFAULT_LINE_LENGTH + subject.size() + DEFAULT_HEADER_STRING.size());
    m_sections.push_back(Section(m_metrics));
    m_itCurrentSection = m_sections.begin();

    InLineResetQuantity();
    Debug::FilterHeader(m_subject);
}

void Debug::NewSection(bool enableSubheader, const std::string &title) {
    InLineEnd();
    m_sections.push_back(Section(m_metrics, enableSubheader, title));
    ++m_itCurrentSection;
}

void Debug::End() {
    InLineEnd();

    for(auto &i:m_sections) {
        i.SolveTabulations();
    }

    Calibrating();

    printHeader();

    for(auto &i:m_sections) {
        i.Markup();
    }
    printFooter();

    m_metrics.setWidth(DEFAULT_LINE_LENGTH);
    m_sections.clear();
    m_sections.push_back(Section(m_metrics));
    m_itCurrentSection = m_sections.begin();
}

inline void Debug::printHeaderBoldLine(int n) {
    char texture = '=';
    for(int i = 0; i < n; ++i)
        std::cout<<texture;
}

inline void Debug::printHeaderLeftLine(int n) {
    std::string line = "--- ";
    printHeaderBoldLine(n-line.size());
    std::cout<<line;
}

inline void Debug::printHeaderRightLine(int n) {
    std::string line = " ---";
    std::cout<<line;
    printHeaderBoldLine(n-line.size());

}

void Debug::calibrateMetrics(int size) {
    if(size>m_metrics.getContentWidth()) {
        m_metrics.setWidth(size+4); //adding to spaces and two borders
    }
}

void Debug::printHeader() {

    int tableWidth      = m_metrics.getTableWidth();
    int wholeLineLength = tableWidth - m_subject.size();

    int leftLineLength  = wholeLineLength/2;
    int rightLineLength = wholeLineLength - leftLineLength;

    std::cout<<"\n";
    printHeaderLeftLine(leftLineLength);
    std::cout<<m_subject;
    printHeaderRightLine(rightLineLength);
    std::cout<<"\n";
}

void Debug::printFooter() {
    char texture = '=';
    for(int i = 0; i<m_metrics.getTableWidth(); ++i) {
        std::cout<<texture;
    }

    std::cout<<"\n";
}

std::vector<std::string> Debug::SplitString(const std::string &text, char delim) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;

    while ((end = text.find(delim, start)) != std::string::npos) {

        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }

    return tokens;
}


void Debug::FilterHeader(std::string &target) {
    std::size_t start = 0, end = 0;

    while ((end = target.find_first_of("\n\t", start)) != std::string::npos) {
        target.at(end)=' ';
        start = end + 1;
    }

}

void Debug::SaveInput(const std::string &input) {
    std::vector<std::string> strings = SplitString(input, '\n');

    for(auto &i:strings) {
        m_itCurrentSection->addLine(i);
    }
}

void Debug::InLineEnd() {
    if(!m_inlineBuffer.empty()) {
        SaveInput(m_inlineBuffer);
        m_inlineBuffer.clear();
    }
}

void Debug::Calibrating() {
    int maxSize = 0;

    for(auto &i:m_sections) {
        if(i.GetMaxLineLength() > maxSize)
            maxSize = i.GetMaxLineLength();
    }

    calibrateMetrics(maxSize);
}
