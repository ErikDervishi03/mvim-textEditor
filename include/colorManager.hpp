#include "globals/mvimResources.h"

#define nColor 8
#define get_pair(bg, fr) ((nColor * bg) + fr)
#define get_pair_default(fr) (get_pair(bgColor,fr))

class ColorManager
{
public:
  struct colorSchema
  {
    color keyWord;
    color numberRows;
    color comments;
    color highlightedText;
    color highlightedBg;
    color brackets;
    color preprocessor;
    color background;
    color text;
    color cursor;
  };

  // Constructor that uses the base class constructor
  ColorManager()
  {
    // Background colors to be used
  }

  // Specific methods for color management
  void setKeyWordColor(color pColor)
  {
    keyWordColor = pColor;
  }

  void setCommentColor(color pColor)
  {
    commentsColor = pColor;
  }

  void setNumberRowsColor(color pColor)
  {
    numberRowsColor = pColor;
  }

  void setHighlightedTextColor(color pColor)
  {
    highlightedTextColor = pColor;
  }

  void setBracketsColor(color pColor)
  {
    bracketsColor = pColor;
  }

  void setPreprocessorColor(color pColor)
  {
    preprocessorColor = pColor;
  }

  void setHighlightedBgColor(color pColor)
  {
    highlightedBgColor = pColor;
  }

  void setBgColor(color pColor)
  {
    bgColor = pColor;
  }

  void setTextColor(color pColor)
  {
    textColor = pColor;
  }

  void setCursorColor(color pColor)
  {
    cursorColor = pColor;
  }

  // Sets the color schema using the provided colorSchema structure
  void setColorSchema(struct colorSchema pColorSchema)
  {
    setBgColor(pColorSchema.background);

    setKeyWordColor(get_pair_default(pColorSchema.keyWord));
    setCommentColor(get_pair_default(pColorSchema.comments));
    setNumberRowsColor(get_pair_default(pColorSchema.numberRows));
    setBracketsColor(get_pair_default(pColorSchema.brackets));
    setHighlightedBgColor(pColorSchema.highlightedBg);
    setHighlightedTextColor(get_pair(highlightedBgColor, pColorSchema.highlightedText));
    setPreprocessorColor(get_pair_default(pColorSchema.preprocessor));
    setTextColor(get_pair_default(pColorSchema.text));
    setCursorColor(pColorSchema.cursor);
  }

  // Sets the color schema based on the provided name
  void setColorSchemaByName(const std::string& schemaName)
  {
    if (schemaName == "default")
    {
      setColorSchema({
        COLOR_BLUE,                      // keyWord
        COLOR_YELLOW,                    // numberRows
        COLOR_CYAN,                      // comments
        COLOR_WHITE,                     // highlightedText
        COLOR_RED,                       // highlightedBg
        COLOR_YELLOW,                    // brackets
        COLOR_MAGENTA,                   // preprocessorColor
        COLOR_BLACK,                     // backgroundColor
        COLOR_WHITE,                     // textColor
        COLOR_WHITE,                     // cursor
      });
    }
    else if (schemaName == "dark")
    {
      setColorSchema({
        COLOR_CYAN,                      // keyWord
        COLOR_GREEN,                     // numberRows
        COLOR_YELLOW,                    // comments
        COLOR_BLACK,                     // highlightedText
        COLOR_RED,                       // highlightedBg
        COLOR_MAGENTA,                   // brackets
        COLOR_RED                        // preprocessorColor
      });
    }
    else if (schemaName == "light")
    {
      setColorSchema({
        COLOR_BLACK,                     // keyWord
        COLOR_WHITE,                     // numberRows
        COLOR_BLUE,                      // comments
        COLOR_YELLOW,                    // highlightedText
        COLOR_CYAN,                      // highlightedBg
        COLOR_GREEN,                     // brackets
        COLOR_RED                        // preprocessorColor
      });
    }
    else if (schemaName == "pastel")
    {
      setColorSchema({
        COLOR_MAGENTA,                   // keyWord
        COLOR_CYAN,                      // numberRows
        COLOR_YELLOW,                    // comments
        COLOR_WHITE,                     // highlightedText
        COLOR_GREEN,                     // highlightedBg
        COLOR_BLUE,                      // brackets
        COLOR_RED                        // preprocessorColor
      });
    }
  }
};
