#pragma once


struct CardId
{
  enum Type { NoStich = -1, Sau = 1, Koenig = 5, Ober = 9, Unter = 13, Zehn = 17, Neun = 21, Acht = 25, Sieben = 29 };
  enum Color { NoColor = -1, Eichel = 0, Gras = 1, Herz = 2, Schellen = 3 };

  Type type{NoStich};
  Color color{NoColor};
};

inline bool operator==(const CardId& lhs, const CardId& rhs)
{
  return lhs.type == rhs.type && lhs.color == rhs.color;
}

inline bool operator!=(const CardId& lhs, const CardId& rhs)
{
  return !(lhs == rhs);
}


struct GameVariant
{
  enum Mode { STICHT, WENZ, GEIER, RUFSPIEL, RAMSCH, DACHS };

  Mode mode{STICHT};
  CardId::Color color{CardId::NoColor};
};
