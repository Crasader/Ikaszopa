//
//  LetterCoords.h
//  ikaszopa
//
//  Created by Axier Fernandez on 17/2/15.
//
//

#ifndef __LetterCoords__
#define __LetterCoords__

#include "cocos2d.h"
#include <algorithm>
#include <string>

using namespace cocos2d;
using namespace std;

class LetterCoords : public Object
{
    public:
    
    LetterCoords(){};
    LetterCoords(string word)
    {
        m_Word = word;
        std::transform(m_Word.begin(), m_Word.end(), m_Word.begin(), ::tolower);
        m_Found = false;
    };
    ~LetterCoords(){};
    
        CC_SYNTHESIZE_READONLY(string, m_Word, Word);
        CC_SYNTHESIZE(int, m_Row, Row);
        CC_SYNTHESIZE(int, m_Start, StartColumn);
        CC_SYNTHESIZE(int, m_End, EndColumn);
        CC_SYNTHESIZE(bool, m_Found, Found);
    
};

#endif /* defined(__LetterCoords__) */
